#include "Box2dManager.h"

#include <iostream>

Box2dManager::Box2dManager() {
    // create world
    worldDef.gravity = (b2Vec2){0.0f, 60.0f};
    worldId = b2CreateWorld(&worldDef);

    // create ground, static body
    groundBodyDef.position = (b2Vec2){0.0f, 720.0f};
    groundId = b2CreateBody(worldId, &groundBodyDef);
    groundBox = b2MakeBox(50.0f, 10.0f);
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // create player, dynamic body
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.position = (b2Vec2){0.0f, 40.0f};
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

st_rectangle Box2dManager::get_player_box()
{
    b2Vec2 position = b2Body_GetPosition(playerBodyId);
    return st_rectangle(position.x, position.y, 100, 50);
}

void Box2dManager::add_stactic_body_rectangles(std::vector<st_rectangle> rectangles)
{
    for (st_rectangle rectangle : rectangles) {
        static_object_struct object;
        object.bodyDef.position = (b2Vec2){rectangle.x, rectangle.y};
        object.id = b2CreateBody(worldId, &object.bodyDef);
        object.box = b2MakeBox(rectangle.w, rectangle.h);
        b2CreatePolygonShape(object.id, &object.shapeDef, &object.box);
        std::cout << "Added static body at[" << object.bodyDef.position.x << "][" << object.bodyDef.position.y << "]" << std::endl;
        staticObjects.push_back(object);
        break;
    }
}

void Box2dManager::add_stactic_body_polygon(std::vector<st_float_position> points)
{
    static_object_struct object;
    object.bodyDef.position = (b2Vec2){0.0f, -10.0f};
    object.id = b2CreateBody(worldId, &groundBodyDef);
    object.box = b2MakeBox(50.0f, 10.0f);
    b2CreatePolygonShape(object.id, &object.shapeDef, &object.box);
    staticObjects.push_back(object);
}


