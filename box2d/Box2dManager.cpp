#include "Box2dManager.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "data/SharedPlayerData.hpp"

CharacterBox2dData& Box2dManager::_player_data()
{
	return _characters.at(PLAYER_CHARACTER_ID);
}

Box2dManager::Box2dManager() : groundBox() {
	// create world
	worldDef.gravity = b2Vec2{0.0f, GRAVITY};
	worldId = b2CreateWorld(&worldDef);

	add_character(
		st_float_position(25.0f * PIXELS_PER_METER, 0.0f),
		st_size(54, 160)
	);
}

Box2dManager::~Box2dManager()
{
	b2DestroyWorld(worldId);
}

int Box2dManager::add_character(st_float_position position, st_size size)
{
	CharacterBox2dData data;

	data.w = static_cast<float>(size.width) / PIXELS_PER_METER;
	data.h = static_cast<float>(size.height) / PIXELS_PER_METER;

	data.bodyDef.type = b2_dynamicBody;
	data.bodyDef.fixedRotation = true;
	data.bodyDef.position = b2Vec2{position.x / PIXELS_PER_METER, position.y / PIXELS_PER_METER};
	data.bodyId = b2CreateBody(worldId, &data.bodyDef);

	data.shapeDef.density = PLAYER_DENSITY;
	data.shapeDef.material.friction = PLAYER_FRICTION;

	data.capsule.center1 = b2Vec2(data.w / 2, -data.h / 2);
	data.capsule.center2 = b2Vec2(data.w / 2, 0.0f);
	data.capsule.radius = data.w / 2;
	b2CreateCapsuleShape(data.bodyId, &data.shapeDef, &data.capsule);

	int id = _next_character_id++;
	_characters[id] = data;
	return id;
}

void Box2dManager::execute()
{
	auto& player = _player_data();
	player.vel_before_step = b2Body_GetLinearVelocity(player.bodyId);
	b2Vec2 posBefore = b2Body_GetPosition(player.bodyId);
	b2World_Step(worldId, timeStep, subStepCount);
	if (player.freeze_position) {
		b2Body_SetTransform(player.bodyId, posBefore, b2Rot_identity);
		b2Body_SetLinearVelocity(player.bodyId, {0.0f, 0.0f});
	}

	// Check landing for all characters
	// Only resets jumps when BOTH: button was released AND player reached ground
	for (auto& [id, data] : _characters) {
		if (data.jump_started && data.jump_button_released) {
			b2Vec2 vel = b2Body_GetLinearVelocity(data.bodyId);
			if (vel.y >= 0.0f && is_character_touching_ground(data)) {
				#ifdef DEBUG_BOX2D
				std::cout << "Box2dManager::execute - Resetting jumps for character " << id << std::endl;
				#endif
				character_reset_jumps(id);
			}
		}
	}
}

st_rectangle Box2dManager::get_ground_box()
{
	return st_rectangle(0, 0, 0, 0);
}

st_rectangle Box2dManager::get_player_box() {
	auto& player = _player_data();
	b2Vec2 position = b2Body_GetPosition(player.bodyId);

	st_rectangle res = st_rectangle((position.x - player.w/2) * PIXELS_PER_METER, (position.y - player.h/2) * PIXELS_PER_METER, player.w * PIXELS_PER_METER, player.h * PIXELS_PER_METER);

	player_data::SharedPlayerData::get_instance()->setPosition(st_float_position((position.x - player.w) * PIXELS_PER_METER, (position.y - player.h) * PIXELS_PER_METER));
	return res;
}

void Box2dManager::add_static_body_rectangles(std::vector<st_rectangle> rectangles) {
	for (st_rectangle rectangle : rectangles) {
		static_object_struct object;
		float calc_half_w = rectangle.w/PIXELS_PER_METER/2;
		float calc_half_h = rectangle.h/PIXELS_PER_METER/2;
		float calc_pos_x = rectangle.x/PIXELS_PER_METER + calc_half_w;
		float calc_pos_y = rectangle.y/PIXELS_PER_METER + calc_half_h;
		object.bodyDef.position = b2Vec2{calc_pos_x, calc_pos_y};
		object.id = b2CreateBody(worldId, &object.bodyDef);
		object.box = b2MakeBox(calc_half_w, calc_half_h);
		b2CreatePolygonShape(object.id, &object.shapeDef, &object.box);
		object.shapeDef.material.restitution = 0.0f;
		object.shapeDef.material.friction = SCENARIO_FRICTION;
		staticObjects.push_back(object);
	}
}

void Box2dManager::add_static_body_polygon(std::vector<std::vector<st_float_position>> points) {
	if (points.size() == 0) {
		return;
	}
	for (int i=0; i<points.size(); i++) {
		unsigned int points_size = points.at(i).size();
		std::vector<b2Vec2> vertices(points_size);
		for (int j=0; j<points.at(i).size(); j++) {
			float vertice_x = points.at(i).at(j).x/PIXELS_PER_METER;
			float vertice_y = points.at(i).at(j).y/PIXELS_PER_METER;
			vertices[j] = {vertice_x, vertice_y};
		}
		b2Hull hull = b2ComputeHull(vertices.data(), points_size);
		b2Polygon polygonShape = b2MakePolygon(&hull, 0.0f);
		staticObjects.push_back(static_object_struct());

		staticObjects.back().bodyDef.position = {points.at(i).front().x/PIXELS_PER_METER, points.at(i).front().y/PIXELS_PER_METER};
		staticObjects.back().bodyDef.type = b2BodyType::b2_staticBody;
		staticObjects.back().id = b2CreateBody(worldId, &groundBodyDef);

		staticObjects.back().shapeDef.density = 1.0f;
		staticObjects.back().shapeDef.material.friction = SCENARIO_FRICTION;
		staticObjects.back().shapeDef.material.restitution = 0.0f;

		b2CreatePolygonShape(staticObjects.back().id, &staticObjects.back().shapeDef, &polygonShape);
	}
}

void Box2dManager::change_player_position(st_float_position inc) {
	auto& player = _player_data();
	b2Vec2 slopeNormal;
	bool onSlope = is_on_slope(slopeNormal);
	e_player_on_ground groundType = is_player_on_ground();
	b2Vec2 currentVelocity = b2Body_GetLinearVelocity(player.bodyId);

	if (onSlope) {
		player.last_slope_normal = slopeNormal;
	}

	// === SLIDE STATE MACHINE ===
	if (player.is_sliding) {
		float expected_speed = HORIZONTAL_SPEED_LIMIT * SLIDE_DOWN_SPEED_MULTIPLIER;
		b2Vec2 vel = b2Body_GetLinearVelocity(player.bodyId);
		float actual_speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

		if (!onSlope) {
			player.is_sliding = false;
			if (groundType == PLAYER_GROUND_LINEAR) {
				player.slide_coasting = true;
			}
		} else if (actual_speed < expected_speed * 0.3f) {
			player.is_sliding = false;
		} else {
			player.slide_coasting = false;
			player.settle_counter = 0;
			player.freeze_position = false;
			b2Body_SetGravityScale(player.bodyId, 1.0f);
			b2Vec2 downhill_dir = {-slopeNormal.y, slopeNormal.x};
			b2Body_SetLinearVelocity(player.bodyId, {downhill_dir.x * expected_speed, downhill_dir.y * expected_speed});
			return;
		}
	}

	// Slide trigger: press DOWN on a slope
	if (inc.y > 0.0f && onSlope && !player.jump_started) {
		player.is_sliding = true;
		player.slide_coasting = false;
		player.settle_counter = 0;
		player.freeze_position = false;
		b2Body_SetGravityScale(player.bodyId, 1.0f);
		float slide_speed = HORIZONTAL_SPEED_LIMIT * SLIDE_DOWN_SPEED_MULTIPLIER;
		b2Vec2 downhill_dir = {-slopeNormal.y, slopeNormal.x};
		b2Body_SetLinearVelocity(player.bodyId, {downhill_dir.x * slide_speed, downhill_dir.y * slide_speed});
		return;
	}

	if (inc.x == 0.0f && onSlope && !player.jump_started) {
		b2Vec2 vel = b2Body_GetLinearVelocity(player.bodyId);
		player.settle_counter++;
		if (player.settle_counter >= 5) {
			player.freeze_position = true;
			b2Body_SetGravityScale(player.bodyId, 0.0f);
			b2Body_SetLinearVelocity(player.bodyId, {0.0f, 0.0f});
			return;
		}
		player.freeze_position = false;
		b2Body_SetGravityScale(player.bodyId, 1.0f);
		b2Body_SetLinearVelocity(player.bodyId, {0.0f, vel.y});
		return;
	}

	player.settle_counter = 0;
	player.freeze_position = false;
	b2Body_SetGravityScale(player.bodyId, 1.0f);

	// Stop coasting when player gives any horizontal input
	if (inc.x != 0.0f) {
		player.slide_coasting = false;
	}

	if (inc.x == 0.0f) {
		if (groundType == PLAYER_GROUND_LINEAR && !player.jump_started) {
			if (player.slide_coasting) {
				b2Vec2 vel = b2Body_GetLinearVelocity(player.bodyId);
				float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
				if (speed < 1.0f) {
					player.slide_coasting = false;
					b2Body_SetLinearVelocity(player.bodyId, {0.0f, 0.0f});
					return;
				}
				return;
			}
			b2Body_SetLinearVelocity(player.bodyId, {0.0f, 0.0f});
			return;
		}
		return;
	}

	// Prevent wall contact from altering vertical speed during jumps/falls
	if (player.jump_started && !onSlope && groundType == PLAYER_GROUND_NONE) {
		b2Vec2 vel = b2Body_GetLinearVelocity(player.bodyId);
		float expected_vy = player.vel_before_step.y + GRAVITY * timeStep;
		if (std::fabs(vel.y - expected_vy) > 0.1f) {
			b2Body_SetLinearVelocity(player.bodyId, {vel.x, expected_vy});
		}
	}

	float target_horizontal_speed = (inc.x > 0) ? HORIZONTAL_SPEED_LIMIT : -HORIZONTAL_SPEED_LIMIT;

	if (onSlope && !player.jump_started) {
		bool climbing_right = (inc.x > 0 && slopeNormal.x > 0.1f);
		bool climbing_left = (inc.x < 0 && slopeNormal.x < -0.1f);
		bool descending_right = (inc.x > 0 && slopeNormal.x < -0.1f);
		bool descending_left = (inc.x < 0 && slopeNormal.x > 0.1f);

		if (climbing_right || climbing_left || descending_right || descending_left) {
			float slope_multiplier = -slopeNormal.x / slopeNormal.y;
			float target_vx = (inc.x > 0) ? HORIZONTAL_SPEED_LIMIT : -HORIZONTAL_SPEED_LIMIT;
			if (climbing_right || climbing_left) {
				target_vx *= SLOPE_CLIMB_SPEED_FACTOR;
			}
			float target_vy = target_vx * slope_multiplier;

			b2Body_SetLinearVelocity(player.bodyId, {target_vx, target_vy});
			return;
		}
	}

	if ((inc.x > 0 && currentVelocity.x < target_horizontal_speed) ||
		(inc.x < 0 && currentVelocity.x > target_horizontal_speed)) {
		float impulse_x = (inc.x > 0) ? HORIZONTAL_MOVE_FORCE : -HORIZONTAL_MOVE_FORCE;

		if (!onSlope && !player.jump_started && player.last_slope_normal.x != 0.0f) {
			bool moving_into_slope = (inc.x > 0 && player.last_slope_normal.x > 0.1f) ||
									 (inc.x < 0 && player.last_slope_normal.x < -0.1f);
			if (moving_into_slope && currentVelocity.y >= -0.5f) {
				float vy_boost = -player.last_slope_normal.x / player.last_slope_normal.y * 0.3f;
				b2Body_ApplyLinearImpulseToCenter(player.bodyId, {impulse_x, vy_boost}, true);
				return;
			}
		}

		b2Body_ApplyLinearImpulseToCenter(player.bodyId, {impulse_x, 0.0f}, true);
	}
}

void Box2dManager::player_jump()
{
	auto& player = _player_data();
	if (player.jump_started == false) {
		player.jump_started = true;

		player.freeze_position = false;
		b2Body_SetGravityScale(player.bodyId, 1.0f);

		b2Vec2 currentVelocity = b2Body_GetLinearVelocity(player.bodyId);
		if (currentVelocity.y < -10.0f) {
			return;
		}

		b2Vec2 velocity;
		velocity.x = 0.0f;
		velocity.y = PLAYER_JUMP_VELOCITY;
		b2Body_ApplyLinearImpulseToCenter(player.bodyId, velocity, true);
	}
}

void Box2dManager::character_jump(int character_id, bool big_jump)
{
	auto& data = _characters.at(character_id);
	data.freeze_position = false;
	b2Body_SetGravityScale(data.bodyId, 1.0f);

	b2Vec2 currentVelocity = b2Body_GetLinearVelocity(data.bodyId);

	data.jump_started = true;
	data.jump_button_released = false;
	data.is_big_jump = big_jump;
	data.jumps_remaining--;

	float velocity_y = data.jump_velocity;
	if (big_jump) {
		velocity_y *= 1.4f;
	}

	b2Body_SetLinearVelocity(data.bodyId, {currentVelocity.x, velocity_y});
}

void Box2dManager::character_jump_interrupt(int character_id)
{
	auto& data = _characters.at(character_id);
	if (!data.jump_started) {
		return;
	}
	if (data.force_jump) {
		return;
	}
	b2Vec2 vel = b2Body_GetLinearVelocity(data.bodyId);
	if (vel.y < 0.0f) {
		b2Body_SetLinearVelocity(data.bodyId, {vel.x, 0.0f});
	}
}

bool Box2dManager::character_can_jump(int character_id)
{
	auto& data = _characters.at(character_id);
	if (data.jumps_remaining > 0) {
		return true;
	}
	return false;
}

void Box2dManager::character_reset_jumps(int character_id)
{
	auto& data = _characters.at(character_id);
	data.jumps_remaining = data.max_jumps;
	data.jump_started = false;
	data.force_jump = false;
	data.is_big_jump = false;
	data.jump_button_released = true;
}

void Box2dManager::set_jump_button_released(int character_id)
{
	_characters.at(character_id).jump_button_released = true;
}

bool Box2dManager::is_character_jumping(int character_id)
{
	return _characters.at(character_id).jump_started;
}

void Box2dManager::set_character_max_jumps(int character_id, int max)
{
	_characters.at(character_id).max_jumps = max;
}

void Box2dManager::set_character_jumps_remaining(int character_id, int remaining)
{
	_characters.at(character_id).jumps_remaining = remaining;
}

void Box2dManager::set_character_force_jump(int character_id, bool force)
{
	_characters.at(character_id).force_jump = force;
}

void Box2dManager::set_character_super_jump(int character_id, bool super)
{
	_characters.at(character_id).is_big_jump = super;
}

float Box2dManager::get_character_vertical_speed(int character_id)
{
	return b2Body_GetLinearVelocity(_characters.at(character_id).bodyId).y;
}

b2BodyId Box2dManager::get_character_body(int character_id)
{
	return _characters.at(character_id).bodyId;
}

void Box2dManager::run_debug_draw(b2DebugDraw *draw)
{
	b2World_Draw(worldId, draw);
}

e_player_on_ground Box2dManager::is_player_on_ground() {
	auto& player = _player_data();
	b2Vec2 normal;
	if (is_on_slope(normal)) {
		player.last_slope_normal = normal;
		return PLAYER_GROUND_SLOPE;
	}

	b2Vec2 position = b2Body_GetPosition(player.bodyId);
	float rayLength = 0.05f;
	b2Vec2 start = {position.x, position.y + player.w / 2.0f - 0.02f};
	b2Vec2 translation = {0.0f, 0.02f + rayLength};

	struct RayCastContext {
		b2Vec2 normal;
		bool hit;
		b2BodyId playerBodyId;
	} context = {{0, 0}, false, player.bodyId};

	auto callback = [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* contextPtr) -> float {
		RayCastContext* ctx = static_cast<RayCastContext*>(contextPtr);
		b2BodyId hitBodyId = b2Shape_GetBody(shapeId);
		if (hitBodyId.index1 == ctx->playerBodyId.index1 && hitBodyId.world0 == ctx->playerBodyId.world0) return -1.0f;
		ctx->hit = true;
		ctx->normal = normal;
		return fraction;
	};

	b2World_CastRay(worldId, start, translation, b2DefaultQueryFilter(), callback, &context);

	if (context.hit && context.normal.y < -0.9f) {
		return PLAYER_GROUND_LINEAR;
	}

	return PLAYER_GROUND_NONE;
}

bool Box2dManager::is_on_slope(b2Vec2& out_normal) {
	auto& player = _player_data();
	b2Vec2 position = b2Body_GetPosition(player.bodyId);
	float bottom_y = position.y + player.w / 2.0f - 0.02f;
	float rayLength = 0.15f;

	struct RayCastContext {
		b2Vec2 normal;
		bool hit;
		b2BodyId playerBodyId;
	};

	auto callback = [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* contextPtr) -> float {
		RayCastContext* ctx = static_cast<RayCastContext*>(contextPtr);
		b2BodyId hitBodyId = b2Shape_GetBody(shapeId);
		if (hitBodyId.index1 == ctx->playerBodyId.index1 && hitBodyId.world0 == ctx->playerBodyId.world0) return -1.0f;
		ctx->hit = true;
		ctx->normal = normal;
		return fraction;
	};

	float ray_offsets[3] = {player.w * 0.2f, player.w * 0.5f, player.w * 0.8f};
	for (int i = 0; i < 3; i++) {
		RayCastContext context = {{0, 0}, false, player.bodyId};
		b2Vec2 start = {position.x + ray_offsets[i], bottom_y};
		b2Vec2 translation = {0.0f, 0.02f + rayLength};
		b2World_CastRay(worldId, start, translation, b2DefaultQueryFilter(), callback, &context);

		if (context.hit && std::abs(context.normal.x) > 0.1f && context.normal.y < -0.1f) {
			out_normal = {-context.normal.x, -context.normal.y};
			return true;
		}
	}

	b2ContactData contactDataArray[10];
	int count = b2Body_GetContactData(player.bodyId, contactDataArray, 10);
	for (int n = 0; n < count; n++) {
		b2ContactData& contact = contactDataArray[n];
		if (contact.manifold.pointCount > 0) {
			b2Vec2 contactNormal = contact.manifold.normal;
			if (std::abs(contactNormal.x) > 0.1f && contactNormal.y < -0.1f) {
				out_normal = {-contactNormal.x, -contactNormal.y};
				return true;
			}
		}
	}

	return false;
}

bool Box2dManager::is_player_touching_ground() {
	auto& player = _player_data();
	b2Vec2 position = b2Body_GetPosition(player.bodyId);
	b2ContactData contactDataArray[10];
	int count = b2Body_GetContactData(player.bodyId, contactDataArray, 10);
	float player_feet = position.y + (player.w / 2.0f);

	for (int n=0; n<count; n++) {
		b2ContactData contactData = contactDataArray[n];
		if (contactData.manifold.pointCount > 0) {
			for (int i=0; i<contactData.manifold.pointCount; i++) {
				if (areFloatsEqual(contactData.manifold.points[i].point.y, player_feet, 0.15f)) return true;
			}
		}
	}
	return false;
}

bool Box2dManager::is_character_touching_ground(const CharacterBox2dData& data) {
	b2Vec2 position = b2Body_GetPosition(data.bodyId);
	b2ContactData contactDataArray[10];
	int count = b2Body_GetContactData(data.bodyId, contactDataArray, 10);
	float feet = position.y + (data.w / 2.0f);

	for (int n=0; n<count; n++) {
		b2ContactData contactData = contactDataArray[n];
		if (contactData.manifold.pointCount > 0) {
			for (int i=0; i<contactData.manifold.pointCount; i++) {
				if (areFloatsEqual(contactData.manifold.points[i].point.y, feet, 0.15f)) return true;
			}
		}
	}
	return false;
}

void Box2dManager::execute_player_physics() {
	auto& player = _player_data();
	b2Vec2 currentVelocity = b2Body_GetLinearVelocity(player.bodyId);

	if (!is_player_touching_ground()) {
		player.jump_started = true;
	} else {
		if (currentVelocity.y >= -0.1f) {
			player.jump_started = false;
		}
	}
}

void Box2dManager::updatePlayerCollision(st_size size) {
	// TBD //
}

bool Box2dManager::areFloatsEqual(float a, float b, float tolerance)
{
	float diff = std::abs(a - b) - tolerance;
	bool result = diff < tolerance;
	return result;
}
