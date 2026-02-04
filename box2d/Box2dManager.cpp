#include "Box2dManager.h"

#include <iostream>


Box2dManager::Box2dManager() : groundId(), groundBox() {
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
                    //std::cout << "Box2dManager::get_player_box - finish-jump - contactData.pointsCount[" << contactData.manifold.pointCount << "], point[" << i << "].y[" << contactData.manifold.points[i].point.y << "], player_feet[" << player_feet << "]" << std::endl;
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
        staticObjects.back().shapeDef.material.friction = 1.0f;
        staticObjects.back().shapeDef.material.restitution = 0.0f;

        b2CreatePolygonShape(staticObjects.back().id, &staticObjects.back().shapeDef, &polygonShape);
    }
}

void Box2dManager::change_player_position(st_float_position inc) {
    if (inc.x == 0.0f) {
        e_player_on_ground ground_type = is_player_on_ground();
        if (ground_type == PLAYER_GROUND_SLOPE && jump_started == false) {
            b2Body_SetLinearVelocity(playerBodyId, {0.0f, 0.0f});
        }
        //playerShapeDef.material.friction = 1.0f;
        //b2Body_SetLinearVelocity(playerBodyId, {0.0f, 1.0f});
        //std::cout << "Box2dManager::change_player_position - no move, friction[" << playerShapeDef.material.friction << "]" << std::endl;
        return;
    }
    b2Vec2 vel = b2Body_GetLinearVelocity(playerBodyId);
    if (inc.x < 0 && vel.x < -HORIZONTAL_SPEED_LIMIT) {
        return;
    }
    if (inc.x > 0 && vel.x > HORIZONTAL_SPEED_LIMIT) {
        return;
    }
    b2Vec2 velocity;
    if (inc.x > 0) {
        velocity.x = HORIZONTAL_MOVE_FORCE;
    } else if (inc.x < 0) {
        velocity.x = -HORIZONTAL_MOVE_FORCE;
    } else {
        velocity.x = 0.0f;
    }
    velocity.y = 0.0f;
    b2Body_ApplyLinearImpulseToCenter(playerBodyId, velocity, true);
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
    // check contact events
    b2ContactData contactDataArray[20];
    int contact_count = b2Body_GetContactData(playerBodyId, contactDataArray, 20);
    float player_feet = position.y + (player_h/2);
    //std::cout << "###################################################################################" << std::endl;

    for (int n=0; n<contact_count; n++) {
        b2ContactData contactData = contactDataArray[n];
        //std::cout << "Box2dManager::is_player_on_ground - contact[" << n << "], pointCount[" << contactData.manifold.pointCount << "], normal[" << contactData.manifold.normal.x << "][" << contactData.manifold.normal.y << "]" << std::endl;
        if (contactData.manifold.pointCount != 2) {
            continue;
        }
        for (int i=0; i<2; i++) {
            if (areFloatsEqual(contactData.manifold.points[i].point.y, player_feet, 0.9f)) {
                if (contactData.manifold.normal.x > 0.0f && contactData.manifold.normal.x < 1.0f && contactData.manifold.normal.y > 0.0f && contactData.manifold.normal.y < 1.0f) {
                    std::cout << "#1 - Box2dManager::is_player_on_ground - Is on slope[TRUE] - normal[" << contactData.manifold.normal.x << "][" << contactData.manifold.normal.y << "]" << std::endl;
                    return PLAYER_GROUND_SLOPE;
                } else if (contactData.manifold.normal.x < 0.0f && contactData.manifold.normal.x > -1.0f && contactData.manifold.normal.y > 0.0f && contactData.manifold.normal.y < 1.0f) {
                    //std::cout << "#2 - Box2dManager::is_player_on_ground - Is on slope[TRUE] - normal[" << contactData.manifold.normal.x << "][" << contactData.manifold.normal.y << "]" << std::endl;
                    return PLAYER_GROUND_SLOPE;
                } else {
                    return PLAYER_GROUND_LINEAR;
                }
            }
        }
    }
    return PLAYER_GROUND_NONE;
}

void Box2dManager::execute_player_physics() {
}

bool Box2dManager::areFloatsEqual(float a, float b, float tolerance)
{
    float diff = std::abs(a - b) - tolerance;
    bool result = diff < tolerance;
    //std::cout << "Box2dManager::areFloatsEqual - a[" << a << "], b[" << b << "], tolerance[" << tolerance << "], diff[" << diff << "], result[" << result << "]" << std::endl;
    return result;
}



