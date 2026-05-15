#ifndef BOX2DMANAGER_H
#define BOX2DMANAGER_H

#include <box2d/box2d.h>
#include <map>
#include "data/st_common.h"

#define PIXELS_PER_METER 40
#define DEBUG_BOX2D
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

struct CharacterBox2dData {
	b2BodyDef bodyDef = b2DefaultBodyDef();
	b2BodyId bodyId{};
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	b2Capsule capsule;
	float w;
	float h;

	// State
	bool jump_started = false;
	int jumps_remaining = 1;
	int max_jumps = 1;
	float jump_velocity = -22.0f;
	bool jump_button_released = true;
	bool force_jump = false;
	bool is_big_jump = false;
	b2Vec2 last_slope_normal = {0.0f, 0.0f};
	bool freeze_position = false;
	int settle_counter = 0;
	bool is_sliding = false;
	bool slide_coasting = false;
	b2Vec2 vel_before_step = {0.0f, 0.0f};
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
	int add_character(st_float_position position, st_size size);
	void change_player_position(st_float_position inc);
	void player_jump();
	void character_jump(int character_id, bool big_jump = false);
	void character_jump_interrupt(int character_id);
	bool character_can_jump(int character_id);
	void character_reset_jumps(int character_id);
	bool is_character_jumping(int character_id);
	void set_character_max_jumps(int character_id, int max);
	void set_character_jumps_remaining(int character_id, int remaining);
	void set_character_force_jump(int character_id, bool force);
	void set_character_super_jump(int character_id, bool super);
	void set_jump_button_released(int character_id);
	float get_character_vertical_speed(int character_id);
	b2BodyId get_character_body(int character_id);
	void run_debug_draw(b2DebugDraw* draw);
	e_player_on_ground is_player_on_ground();
	bool is_on_slope(b2Vec2& normal);
	bool is_player_touching_ground();
	void updatePlayerCollision(st_size size);

	bool is_character_touching_ground(const CharacterBox2dData& data);

	friend class Box2dManagerTest;

	static constexpr int PLAYER_CHARACTER_ID = 0;

private:
	bool areFloatsEqual(float a, float b, float tolerance = 1e-5f);

private:
	// configuration
	float timeStep = 1.0f / 60.0f;
	int subStepCount = 4;
	float GRAVITY = 39.6f;
	float HORIZONTAL_SPEED_LIMIT = 10.0f;
	float SLIDE_DOWN_SPEED_MULTIPLIER = 2.0f;
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

	// characters
	std::map<int, CharacterBox2dData> _characters;
	int _next_character_id = 0;

	// static objects
	std::vector<static_object_struct> staticObjects;

	CharacterBox2dData& _player_data();
};

#endif // BOX2DMANAGER_H
