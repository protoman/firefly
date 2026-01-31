#include "Box2dManager.h"

#include <iostream>


Box2dManager::Box2dManager() {
    // create world
    worldDef.gravity = (b2Vec2){0.0f, 9.8f};
    worldId = b2CreateWorld(&worldDef);
    b2World_SetMaximumLinearSpeed(worldId, 6.0f);

    // create ground, static body
    //groundBodyDef.position = (b2Vec2){0.0f, 720.0f};
    //groundId = b2CreateBody(worldId, &groundBodyDef);
    //groundBox = b2MakeBox(50.0f, 10.0f);
    //b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // create player, dynamic body
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.position = (b2Vec2){10.0f, 0.0f};
    playerBodyId = b2CreateBody(worldId, &playerBodyDef);
    playerShapeDef.density = 1.0f;
    playerShapeDef.material.friction = 0.3f;
    b2CreatePolygonShape(playerBodyId, &playerShapeDef, &dynamicBox);
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
                if (areFloatsEqual(contactData.manifold.points[i].point.y, player_feet, 0.2f)) {
                    jump_started = false;
                    std::cout << "Box2dManager::get_player_box - finish-jump - contactData.pointsCount[" << contactData.manifold.pointCount << "], point[" << i << "].y[" << contactData.manifold.points[i].point.y << "], player_feet[" << player_feet << "]" << std::endl;
                    break;
                }
            }
            if (jump_started == false) {
                break;
            }
        }
    }


    st_rectangle res = st_rectangle(position.x * PIXELS_PER_METER, position.y * PIXELS_PER_METER, player_w * PIXELS_PER_METER, player_h * PIXELS_PER_METER);
    //std::cout << "Box2dManager::get_player_box[" << res.x << "][" << res.y << "][" << res.w << "][" << res.h << "]" << std::endl;
    return res;
}

void Box2dManager::add_stactic_body_rectangles(std::vector<st_rectangle> rectangles) {
    std::cout << ">> Box2dManager::add_stactic_body_rectangles.size[" << rectangles.size() << "]" << std::endl;
    for (st_rectangle rectangle : rectangles) {
        static_object_struct object;
        object.bodyDef.position = (b2Vec2){rectangle.x/PIXELS_PER_METER, rectangle.y/PIXELS_PER_METER};
        object.id = b2CreateBody(worldId, &object.bodyDef);
        object.box = b2MakeBox(rectangle.w/PIXELS_PER_METER, rectangle.h/PIXELS_PER_METER);
        b2CreatePolygonShape(object.id, &object.shapeDef, &object.box);
        std::cout << "Added static body at[" << object.bodyDef.position.x << "][" << object.bodyDef.position.y << "]" << std::endl;
        staticObjects.push_back(object);
    }
}

void Box2dManager::add_stactic_body_polygon(std::vector<st_float_position> points) {
    static_object_struct object;
    object.bodyDef.position = (b2Vec2){0.0f, -10.0f};
    object.id = b2CreateBody(worldId, &groundBodyDef);
    object.box = b2MakeBox(50.0f, 10.0f);
    b2CreatePolygonShape(object.id, &object.shapeDef, &object.box);
    staticObjects.push_back(object);
}

void Box2dManager::change_player_position(st_float_position inc)  {
    b2Vec2 vel = b2Body_GetLinearVelocity(playerBodyId);
    b2Vec2 velocity;
    //velocity.x = b2MaxFloat( vel.x + 0.1f, 5.0f );
    velocity.x = inc.x;
    //velocity.y = worldDef.gravity.y;
    velocity.y = 0.0f;
    b2Body_SetLinearVelocity(playerBodyId, velocity);
    //b2Body_ApplyLinearImpulseToCenter(playerBodyId, velocity, true);
}

void Box2dManager::player_jump()
{
    if (jump_started == false) {
        jump_started = true;
        std::cout << ">>> Box2dManager::player_jump::START" << std::endl;
        // check that velocity is not already applied
        b2Vec2 currentVelocity = b2Body_GetLinearVelocity(playerBodyId);
        if (currentVelocity.y < -10.0f) {
            return;
        }

        b2Vec2 velocity;
        velocity.x = 0.0f;
        velocity.y = -22.0f;
        b2Body_ApplyLinearImpulseToCenter(playerBodyId, velocity, true);
    }
}

void Box2dManager::run_debug_draw(b2DebugDraw *draw)
{
    b2World_Draw(worldId, draw);
}

bool Box2dManager::areFloatsEqual(float a, float b, float tolerance)
{
    return std::abs(a - b) < tolerance;
}



