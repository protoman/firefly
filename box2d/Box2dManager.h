#ifndef BOX2DMANAGER_H
#define BOX2DMANAGER_H

#include <box2d/box2d.h>
#include "data/st_common.h"

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
    void add_stactic_body_rectangles(std::vector<st_rectangle> rectangles);
    void add_stactic_body_polygon(std::vector<st_float_position> points);

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
    b2BodyDef playerBodyDef = b2DefaultBodyDef();
    b2BodyId playerBodyId;
    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
    b2ShapeDef playerShapeDef = b2DefaultShapeDef();


    // static objects
    std::vector<static_object_struct> staticObjects;
};

#endif // BOX2DMANAGER_H
