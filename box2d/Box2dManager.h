#ifndef BOX2DMANAGER_H
#define BOX2DMANAGER_H

#include <box2d/box2d.h>
#include "data/st_common.h"

#define PIXELS_PER_METER 40

struct static_object_struct {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    b2BodyId id;
    b2Polygon box;
    b2ShapeDef shapeDef = b2DefaultShapeDef();
};

class Box2dManager
{
public:
    Box2dManager();
    ~Box2dManager();
    void execute();
    st_rectangle get_ground_box();
    st_rectangle get_player_box();
    void add_static_body_rectangles(std::vector<st_rectangle> rectangles);
    void add_static_body_polygon(std::vector<st_float_position> points);
    void change_player_position(st_float_position inc);
    void player_jump();
    void run_debug_draw(b2DebugDraw* draw);

private:
    bool areFloatsEqual(float a, float b, float tolerance = 1e-5f);

private:
    // configuration
    float timeStep = 1.0f / 60.0f;
    int subStepCount = 4;
    // world
    b2WorldDef worldDef = b2DefaultWorldDef();
    b2WorldId worldId;
    // ground
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    b2BodyId groundId;
    b2Polygon groundBox;
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    // player
    float player_w = 0.9f;
    float player_h = 1.8f;
    b2BodyDef playerBodyDef = b2DefaultBodyDef();
    b2BodyId playerBodyId;
    b2Polygon dynamicBox = b2MakeBox(player_w/2, player_h/2);
    b2ShapeDef playerShapeDef = b2DefaultShapeDef();

    bool jump_started = false;
    // static objects
    std::vector<static_object_struct> staticObjects;
};

#endif // BOX2DMANAGER_H
