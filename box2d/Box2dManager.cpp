#include "Box2dManager.h"

#include "data/SharedPlayerData.hpp"

Box2dManager::Box2dManager() : groundBox(), _last_slope_normal(0.0f, 0.0f) { // Initialize _last_slope_normal
    // create world
    worldDef.gravity = (b2Vec2){0.0f, GRAVITY};
    worldId = b2CreateWorld(&worldDef);
    b2World_SetMaximumLinearSpeed(worldId, MAX_SPEED);

    // create ground, static body
    //groundBodyDef.position = (b2Vec2){0.0f, 720.0f};
    //groundId = b2CreateBody(worldId, &groundBodyDef);
    //groundBox = b2MakeBox(50.0f, 10.0f);
    //b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // create player, dynamic body
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.fixedRotation = true; // Set this to true to prevent rotation
    //playerBodyDef.linearDamping = 1.0f;
    playerBodyDef.position = (b2Vec2){25.0f, 0.0f};
    playerBodyId = b2CreateBody(worldId, &playerBodyDef);
    playerShapeDef.density = PLAYER_DENSITY;
    playerShapeDef.material.friction = PLAYER_FRICTION;

    /*
    b2Vec2 vertices[6];
    vertices[0] = {player_w/2 - 0.1f, 0.0f }; // top-left-center
    vertices[1] = {player_w/2 + 0.1f, 0.0f }; // top-right-center
    vertices[2] = {0.0f, player_h/2 }; // left-middle
    vertices[3] = {player_w, player_h/2 }; // right-middle
    vertices[4] = {player_w/2 - 0.1f, player_h }; // bottom-left-center
    vertices[5] = {player_w/2 + 0.1f, player_h }; // bottom-right-center
    b2Hull hull = b2ComputeHull(vertices, 6);
    playerPolygonShape = b2MakePolygon(&hull, 0.0f); // 0.0f radius for sharp corners
    b2CreatePolygonShape(playerBodyId, &playerShapeDef, &playerPolygonShape);
    */


    playerCapsule.center1 = b2Vec2(player_w/2, -player_h/2); // Bottom center
    playerCapsule.center2 = b2Vec2(player_w/2, 0.0f); // Top center
    playerCapsule.radius = player_w/2;                // Radius of the semicircles
    b2CreateCapsuleShape(playerBodyId, &playerShapeDef, &playerCapsule);
}

Box2dManager::~Box2dManager()
{
    b2DestroyWorld(worldId);
}

void Box2dManager::execute()
{
    b2Vec2 posBefore = b2Body_GetPosition(playerBodyId);
    b2World_Step(worldId, timeStep, subStepCount);
    if (_freeze_position) {
        b2Body_SetTransform(playerBodyId, posBefore, b2Rot_identity);
        b2Body_SetLinearVelocity(playerBodyId, {0.0f, 0.0f});
    }
}

st_rectangle Box2dManager::get_ground_box()
{
    return st_rectangle(0, 0, 0, 0);
}

st_rectangle Box2dManager::get_player_box() {
    b2Vec2 position = b2Body_GetPosition(playerBodyId);

    // TODO - move to an 'execute' method
    // check contact events
    if (jump_started == true) {
        b2ContactData contactDataArray[10];
        int count = b2Body_GetContactData(playerBodyId, contactDataArray, 10);
        float player_feet = position.y + (player_h/2);
        for (b2ContactData contactData : contactDataArray) {
            if (contactData.manifold.pointCount != 2) {
                continue;
            }
            for (int i=0; i<2; i++) {
                if (areFloatsEqual(contactData.manifold.points[i].point.y, player_feet, 0.9f)) {
                    jump_started = false;
                    //std::cout << "Box2dManager::get_player_box - finish-jump - contactData.pointsCount[" << contactData.manifold.pointCount << "], point[" << i << "].y[" << contactData.manifold.points[i].point.y << "], player_feet[" << contactData.manifold.points[i].point.y << "]" << std::endl;
                    break;
                }
            }
            if (jump_started == false) {
                break;
            }
        }
    }


    st_rectangle res = st_rectangle((position.x - player_w/2) * PIXELS_PER_METER, (position.y - player_h/2) * PIXELS_PER_METER, player_w * PIXELS_PER_METER, player_h * PIXELS_PER_METER);
    //std::cout << "Box2dManager::get_player_box[" << res.x << "][" << res.y << "][" << res.w << "][" << res.h << "]" << std::endl;

    player_data::SharedPlayerData::get_instance()->setPosition(st_float_position((position.x - player_w) * PIXELS_PER_METER, (position.y - player_h) * PIXELS_PER_METER));
    return res;
}

void Box2dManager::add_static_body_rectangles(std::vector<st_rectangle> rectangles) {
    //std::cout << ">> Box2dManager::add_static_body_rectangles.size[" << rectangles.size() << "]" << std::endl;
    for (st_rectangle rectangle : rectangles) {
        static_object_struct object;
        float calc_half_w = rectangle.w/PIXELS_PER_METER/2;
        float calc_half_h = rectangle.h/PIXELS_PER_METER/2;
        float calc_pos_x = rectangle.x/PIXELS_PER_METER + calc_half_w;
        float calc_pos_y = rectangle.y/PIXELS_PER_METER + calc_half_h;
        object.bodyDef.position = (b2Vec2){calc_pos_x, calc_pos_y};
        object.id = b2CreateBody(worldId, &object.bodyDef);
        object.box = b2MakeBox(calc_half_w, calc_half_h);
        b2CreatePolygonShape(object.id, &object.shapeDef, &object.box);
        object.shapeDef.material.restitution = 0.0f;
        object.shapeDef.material.friction = SCENARIO_FRICTION;
        //std::cout << "Added static body at[" << object.bodyDef.position.x << "][" << object.bodyDef.position.y << "][" << ", rectangle[" << rectangle.x << "][" << rectangle.y << "][" << rectangle.w << "][" << rectangle.h << "]" << std::endl;
        staticObjects.push_back(object);
    }
}

void Box2dManager::add_static_body_polygon(std::vector<std::vector<st_float_position>> points) {
    if (points.size() == 0) {
        return;
    }
    for (int i=0; i<points.size(); i++) {
        unsigned int points_size = points.at(i).size();
        b2Vec2 vertices[points_size];
        for (int j=0; j<points.at(i).size(); j++) {
            float vertice_x = points.at(i).at(j).x/PIXELS_PER_METER;
            float vertice_y = points.at(i).at(j).y/PIXELS_PER_METER;
            vertices[j] = {vertice_x, vertice_y};
        }
        b2Hull hull = b2ComputeHull(vertices, points_size);
        b2Polygon polygonShape = b2MakePolygon(&hull, 0.0f); // 0.0f radius for sharp corners
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
    b2Vec2 slopeNormal;
    bool onSlope = is_on_slope(slopeNormal);
    e_player_on_ground groundType = is_player_on_ground();
    b2Vec2 currentVelocity = b2Body_GetLinearVelocity(playerBodyId);

    if (onSlope) {
        _last_slope_normal = slopeNormal;
    }

    if (inc.x == 0.0f && onSlope && !jump_started) {
        b2Vec2 vel = b2Body_GetLinearVelocity(playerBodyId);
        _settle_counter++;
        if (_settle_counter >= 5) {
            _freeze_position = true;
            b2Body_SetGravityScale(playerBodyId, 0.0f);
            b2Body_SetLinearVelocity(playerBodyId, {0.0f, 0.0f});
            return;
        }
        _freeze_position = false;
        b2Body_SetGravityScale(playerBodyId, 1.0f);
        b2Body_SetLinearVelocity(playerBodyId, {0.0f, vel.y});
        return;
    }

    _settle_counter = 0;
    _freeze_position = false;
    b2Body_SetGravityScale(playerBodyId, 1.0f);

    if (inc.x == 0.0f) {
        if (groundType == PLAYER_GROUND_LINEAR && !jump_started) {
            b2Body_SetLinearVelocity(playerBodyId, {0.0f, 0.0f});
            return;
        }
        return;
    }

    float target_horizontal_speed = (inc.x > 0) ? HORIZONTAL_SPEED_LIMIT : -HORIZONTAL_SPEED_LIMIT;

    if (onSlope && !jump_started) {
        bool climbing_right = (inc.x > 0 && slopeNormal.x > 0.1f);
        bool climbing_left = (inc.x < 0 && slopeNormal.x < -0.1f);

        if (climbing_right || climbing_left) {
            float slope_multiplier = -slopeNormal.x / slopeNormal.y;
            float target_vx = (inc.x > 0) ? HORIZONTAL_SPEED_LIMIT * SLOPE_CLIMB_SPEED_FACTOR : -HORIZONTAL_SPEED_LIMIT * SLOPE_CLIMB_SPEED_FACTOR;
            float target_vy = target_vx * slope_multiplier;

            b2Body_SetLinearVelocity(playerBodyId, {target_vx, target_vy});
            return;
        }
    }

    if ((inc.x > 0 && currentVelocity.x < target_horizontal_speed) ||
        (inc.x < 0 && currentVelocity.x > target_horizontal_speed)) {
        float impulse_x = (inc.x > 0) ? HORIZONTAL_MOVE_FORCE : -HORIZONTAL_MOVE_FORCE;

        if (!onSlope && !jump_started && _last_slope_normal.x != 0.0f) {
            bool moving_into_slope = (inc.x > 0 && _last_slope_normal.x > 0.1f) ||
                                     (inc.x < 0 && _last_slope_normal.x < -0.1f);
            if (moving_into_slope && currentVelocity.y >= -0.5f) {
                float vy_boost = -_last_slope_normal.x / _last_slope_normal.y * 0.3f;
                b2Body_ApplyLinearImpulseToCenter(playerBodyId, {impulse_x, vy_boost}, true);
                return;
            }
        }

        b2Body_ApplyLinearImpulseToCenter(playerBodyId, {impulse_x, 0.0f}, true);
    }
}

void Box2dManager::player_jump()
{
    if (jump_started == false) {
        jump_started = true;

        _freeze_position = false;
        b2Body_SetGravityScale(playerBodyId, 1.0f);

        b2Vec2 currentVelocity = b2Body_GetLinearVelocity(playerBodyId);
        if (currentVelocity.y < -10.0f) {
            return;
        }

        b2Vec2 velocity;
        velocity.x = 0.0f;
        velocity.y = PLAYER_JUMP_VELOCITY;
        b2Body_ApplyLinearImpulseToCenter(playerBodyId, velocity, true);
    }
}

void Box2dManager::run_debug_draw(b2DebugDraw *draw)
{
    b2World_Draw(worldId, draw);
}

e_player_on_ground Box2dManager::is_player_on_ground() {
    b2Vec2 normal;
    if (is_on_slope(normal)) {
        _last_slope_normal = normal;
        return PLAYER_GROUND_SLOPE;
    }

    b2Vec2 position = b2Body_GetPosition(playerBodyId);
    float rayLength = 0.05f; // Extremely short ray for flat ground detection
    b2Vec2 start = {position.x, position.y + player_w / 2.0f - 0.02f}; // Start at capsule bottom
    b2Vec2 translation = {0.0f, 0.02f + rayLength}; // Check just a few pixels below feet

    struct RayCastContext {
        b2Vec2 normal;
        bool hit;
        b2BodyId playerBodyId;
    } context = {{0, 0}, false, playerBodyId};

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
    b2Vec2 position = b2Body_GetPosition(playerBodyId);
    float bottom_y = position.y + player_w / 2.0f - 0.02f;
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

    // Cast 3 rays across the player's width for robust slope detection
    float ray_offsets[3] = {player_w * 0.2f, player_w * 0.5f, player_w * 0.8f};
    for (int i = 0; i < 3; i++) {
        RayCastContext context = {{0, 0}, false, playerBodyId};
        b2Vec2 start = {position.x + ray_offsets[i], bottom_y};
        b2Vec2 translation = {0.0f, 0.02f + rayLength};
        b2World_CastRay(worldId, start, translation, b2DefaultQueryFilter(), callback, &context);

        if (context.hit && std::abs(context.normal.x) > 0.1f && context.normal.y < -0.1f) {
            out_normal = {-context.normal.x, -context.normal.y};
            return true;
        }
    }

    // Fallback: check contact normals from Box2D contacts for slope detection
    b2ContactData contactDataArray[10];
    int count = b2Body_GetContactData(playerBodyId, contactDataArray, 10);
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
    b2Vec2 position = b2Body_GetPosition(playerBodyId);
    b2ContactData contactDataArray[10];
    int count = b2Body_GetContactData(playerBodyId, contactDataArray, 10);
    float player_feet = position.y + (player_h/2);

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

void Box2dManager::execute_player_physics() {
    b2Vec2 currentVelocity = b2Body_GetLinearVelocity(playerBodyId);
    
    if (!is_player_touching_ground()) {
        jump_started = true;
    } else {
        if (currentVelocity.y >= -0.1f) {
            jump_started = false;
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
    //std::cout << "Box2dManager::areFloatsEqual - a[" << a << "], b[" << b << "], tolerance[" << tolerance << "], diff[" << diff << "], result[" << result << "]" << std::endl;
    return result;
}
