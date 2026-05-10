#ifndef BOX2DMANAGER_H
#define BOX2DMANAGER_H

#include <box2d/box2d.h>
#include "data/st_common.h"

#define PIXELS_PER_METER 40
#define SLOPE_CLIMB_SPEED_FACTOR 0.5f


enum e_player_on_ground {
    PLAYER_GROUND_NONE,
    PLAYER_GROUND_LINEAR,
    PLAYER_GROUND_SLOPE
};

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
    void add_static_body_polygon(std::vector<std::vector<st_float_position>> points);
    void change_player_position(st_float_position inc);
    void player_jump();
    void run_debug_draw(b2DebugDraw* draw);
    e_player_on_ground is_player_on_ground();
    bool is_on_slope(b2Vec2& normal);
    bool is_player_touching_ground();
    void execute_player_physics();
    void updatePlayerCollision(st_size size);

    friend class Box2dManagerTest;

private:
    bool areFloatsEqual(float a, float b, float tolerance = 1e-5f);

private:
    // configuration
    float timeStep = 1.0f / 60.0f;
    int subStepCount = 4;
    float GRAVITY = 39.6f;
    float MAX_SPEED = 45.0f;
    float HORIZONTAL_SPEED_LIMIT = 10.0f;
    float HORIZONTAL_MOVE_FORCE = 4.0f;
    float PLAYER_DENSITY = 1.0f;
    float PLAYER_FRICTION = 1.0f;
    float SCENARIO_FRICTION = 0.5f;
    float PLAYER_JUMP_VELOCITY = -22.0f;

    // world
    b2WorldDef worldDef = b2DefaultWorldDef();
    b2WorldId worldId{};
    // ground
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    b2BodyId id;
    b2Polygon groundBox;
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    // player
    float player_w = 54 / PIXELS_PER_METER;
    float player_h = 160 / PIXELS_PER_METER;
    b2BodyDef playerBodyDef = b2DefaultBodyDef();
    b2BodyId playerBodyId{};
    b2Polygon dynamicBox = b2MakeBox(player_w/2, player_h/2);
    b2Polygon playerPolygonShape;
    b2ShapeDef playerShapeDef = b2DefaultShapeDef();

    b2Capsule playerCapsule;


    bool jump_started = false;
    b2Vec2 _last_slope_normal;
    bool _freeze_position = false;
    int _settle_counter = 0;
    b2Vec2 _last_settle_pos = {0.0f, 0.0f};

    // static objects
    std::vector<static_object_struct> staticObjects;
};

#endif // BOX2DMANAGER_H
