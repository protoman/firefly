#include "Box2dManager.h"

#include <iostream>
#include "data/SharedPlayerData.hpp"

#define SLOPE_REDUCED_SPEED_FACTOR 0.2f // For sliding down when no input

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
    playerBodyDef.position = (b2Vec2){10.0f, 0.0f};
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
    b2World_Step(worldId, timeStep, subStepCount);
    //b2Vec2 position = b2Body_GetPosition(playerBodyId);
    //b2Rot rotation = b2Body_GetRotation(playerBodyId);
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
    e_player_on_ground ground_type = is_player_on_ground();
    b2Vec2 currentVelocity = b2Body_GetLinearVelocity(playerBodyId);

    if (inc.x == 0.0f) {
        if (ground_type == PLAYER_GROUND_SLOPE && jump_started == false) {
            // Player is on a slope and no horizontal input, make them slide down
            b2Vec2 desiredVelocity = {-_last_slope_normal.x * SLOPE_REDUCED_SPEED_FACTOR, currentVelocity.y};
            b2Body_SetLinearVelocity(playerBodyId, desiredVelocity);
            return;
        } else if (ground_type == PLAYER_GROUND_LINEAR && jump_started == false) {
            // Player is on flat ground and no horizontal input, stop them
            b2Body_SetLinearVelocity(playerBodyId, {0.0f, 0.0f});
            return;
        }
        // If not on ground or jumping, let gravity handle vertical movement, no horizontal impulse
        return;
    }

    // Player has horizontal input (inc.x != 0.0f)
    float target_horizontal_speed = 0.0f;
    float max_speed_limit = HORIZONTAL_SPEED_LIMIT;
    bool is_climbing_slope = false;

    if (ground_type == PLAYER_GROUND_SLOPE && jump_started == false) {
        // Check if trying to climb the slope
        // A slope normal n = {nx, ny}. If nx < 0, slope goes up-right. If nx > 0, slope goes up-left.
        bool climbing_right = (inc.x > 0 && _last_slope_normal.x > 0.1f); 
        bool climbing_left = (inc.x < 0 && _last_slope_normal.x < -0.1f);

        if (climbing_right || climbing_left) {
            is_climbing_slope = true;
            max_speed_limit *= SLOPE_CLIMB_SPEED_FACTOR; // Reduce speed limit for climbing
        }
    }

    if (inc.x > 0) {
        target_horizontal_speed = max_speed_limit;
    } else { // inc.x < 0
        target_horizontal_speed = -max_speed_limit;
    }

    if (is_climbing_slope) {
        // When climbing, project the horizontal velocity onto the slope surface
        // The surface tangent T is {-ny, nx} or {ny, -nx}. 
        // We want to move along T.
        b2Vec2 tangent = {_last_slope_normal.y, -_last_slope_normal.x};
        if (inc.x * tangent.x < 0) {
            tangent = {-tangent.x, -tangent.y};
        }
        
        b2Vec2 projectedVelocity = {
            tangent.x * max_speed_limit,
            tangent.y * max_speed_limit
        };
        b2Body_SetLinearVelocity(playerBodyId, projectedVelocity);
    } else {
        // For normal movement (not climbing a steep slope, or moving down a slope), apply impulse to accelerate
        // Only apply impulse if we are below the target speed limit
        if ((inc.x > 0 && currentVelocity.x < target_horizontal_speed) ||
            (inc.x < 0 && currentVelocity.x > target_horizontal_speed)) { // Note: target_horizontal_speed is negative for left
            float impulse_x = (inc.x > 0) ? HORIZONTAL_MOVE_FORCE : -HORIZONTAL_MOVE_FORCE;
            b2Body_ApplyLinearImpulseToCenter(playerBodyId, {impulse_x, 0.0f}, true);
        }
    }
}

void Box2dManager::player_jump()
{
    if (jump_started == false) {
        jump_started = true;
        //std::cout << ">>> Box2dManager::player_jump::START" << std::endl;
        // check that velocity is not already applied
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
    b2Vec2 position = b2Body_GetPosition(playerBodyId);
    float rayLength = 0.5f; // Increased ray length
    b2Vec2 start = {position.x, position.y}; // Start from center
    b2Vec2 translation = {0.0f, player_h / 2.0f + rayLength};

    struct RayCastContext {
        b2Vec2 normal;
        bool hit;
        b2BodyId playerBodyId;
    } context = {{0, 0}, false, playerBodyId};

    auto callback = [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* contextPtr) -> float {
        RayCastContext* ctx = static_cast<RayCastContext*>(contextPtr);
        b2BodyId hitBodyId = b2Shape_GetBody(shapeId);
        if (hitBodyId.index1 == ctx->playerBodyId.index1 && hitBodyId.world0 == ctx->playerBodyId.world0) {
            return -1.0f; // Continue raycast
        }
        ctx->hit = true;
        ctx->normal = normal;
        return fraction; // Terminate raycast at closest hit
    };

    b2World_CastRay(worldId, start, translation, b2DefaultQueryFilter(), callback, &context);

    if (context.hit) {
        if (std::abs(context.normal.x) > 0.01f) {
            _last_slope_normal = {-context.normal.x, -context.normal.y};
            return PLAYER_GROUND_SLOPE;
        } else if (context.normal.y < -0.9f) {
            return PLAYER_GROUND_LINEAR;
        }
    }
    return PLAYER_GROUND_NONE;
}

void Box2dManager::execute_player_physics() {
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
