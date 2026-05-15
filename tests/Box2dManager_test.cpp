#include "gtest/gtest.h"
#include "box2d/Box2dManager.h"
#include "data/st_common.h"
#include "data/SharedPlayerData.hpp"
#include <cmath>
#include <iostream>

class Box2dManagerTest : public ::testing::Test {
protected:
	Box2dManager* box2dManager;

	void SetUp() override {
		box2dManager = new Box2dManager();
		auto& player = box2dManager->_characters.at(Box2dManager::PLAYER_CHARACTER_ID);
		b2Body_SetTransform(player.bodyId, {1.0f, 1.0f}, b2Rot_identity);
		b2Body_SetLinearVelocity(player.bodyId, {0.0f, 0.0f});
		b2Body_SetAngularVelocity(player.bodyId, 0.0f);
	}

	void TearDown() override {
		delete box2dManager;
		box2dManager = nullptr;
	}

	void createSlope(float start_x_pixels, float start_y_pixels, float end_x_pixels, float end_y_pixels) {
		std::vector<std::vector<st_float_position>> polygons;
		std::vector<st_float_position> slope_points;

		slope_points.push_back(st_float_position(start_x_pixels, start_y_pixels));
		slope_points.push_back(st_float_position(end_x_pixels, end_y_pixels));
		slope_points.push_back(st_float_position(end_x_pixels, start_y_pixels));

		polygons.push_back(slope_points);
		box2dManager->add_static_body_polygon(polygons);
	}

	void setPlayerTransform(b2Vec2 position, b2Rot rotation) {
		auto& player = box2dManager->_characters.at(Box2dManager::PLAYER_CHARACTER_ID);
		b2Body_SetTransform(player.bodyId, position, rotation);
	}

	void setPlayerVelocity(b2Vec2 velocity) {
		auto& player = box2dManager->_characters.at(Box2dManager::PLAYER_CHARACTER_ID);
		b2Body_SetLinearVelocity(player.bodyId, velocity);
	}

	b2Vec2 getPlayerPosition() {
		auto& player = box2dManager->_characters.at(Box2dManager::PLAYER_CHARACTER_ID);
		return b2Body_GetPosition(player.bodyId);
	}

	b2Vec2 getPlayerVelocity() {
		auto& player = box2dManager->_characters.at(Box2dManager::PLAYER_CHARACTER_ID);
		return b2Body_GetLinearVelocity(player.bodyId);
	}

	float getPlayerHeight() {
		return box2dManager->_characters.at(Box2dManager::PLAYER_CHARACTER_ID).h;
	}

	float getHorizontalSpeedLimit() {
		return box2dManager->HORIZONTAL_SPEED_LIMIT;
	}
};

TEST_F(Box2dManagerTest, PlayerMovesUp45DegreeSlope) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);

	setPlayerTransform({1.0f, 7.0f}, b2Rot_identity);

	st_float_position move_inc(1.0f, 0.0f);

	float initial_x = getPlayerPosition().x;
	float initial_y = getPlayerPosition().y;

	for (int i = 0; i < 30; ++i) {
		box2dManager->change_player_position(move_inc);
		box2dManager->execute();
	}

	float final_x = getPlayerPosition().x;
	float final_y = getPlayerPosition().y;

	ASSERT_GT(final_x, initial_x);
	ASSERT_LT(final_y, initial_y);
}

TEST_F(Box2dManagerTest, PlayerMovesUp60DegreeSlope) {
	createSlope(0.0f, 400.0f, 230.8f, 0.0f);

	setPlayerTransform({1.0f, 6.268f}, b2Rot_identity);

	st_float_position move_inc(1.0f, 0.0f);

	float initial_x = getPlayerPosition().x;
	float initial_y = getPlayerPosition().y;

	for (int i = 0; i < 30; ++i) {
		box2dManager->change_player_position(move_inc);
		box2dManager->execute();
	}

	float final_x = getPlayerPosition().x;
	float final_y = getPlayerPosition().y;

	ASSERT_GT(final_x, initial_x);
	ASSERT_LT(final_y, initial_y);
}

TEST_F(Box2dManagerTest, PlayerFallsNormally) {
	std::vector<st_rectangle> floors;
	floors.push_back(st_rectangle(0, 400, 800, 40));
	box2dManager->add_static_body_rectangles(floors);

	setPlayerTransform({5.0f, 2.0f}, b2Rot_identity);

	float last_y = getPlayerPosition().y;
	float max_fall_speed = 0.0f;

	for (int i = 0; i < 60; ++i) {
		box2dManager->execute();
		b2Vec2 velocity = getPlayerVelocity();
		if (velocity.y > max_fall_speed) {
			max_fall_speed = velocity.y;
		}

		float current_y = getPlayerPosition().y;
		std::cout << "Step " << i << ": y=" << current_y << ", vy=" << velocity.y << std::endl;

		if (current_y < 7.0f) {
			 ASSERT_GE(velocity.y, -0.1f);
			 if (i > 5) {
				 ASSERT_GT(velocity.y, 1.0f);
			 }
		}
		last_y = current_y;
	}

	ASSERT_GT(max_fall_speed, 5.0f);
}

#define TILESIZE 32
#define SLOPE_ANGLE_THRESHOLD 45.0f
#define MINIMAL_SPEED_FACTOR 0.2f

float calculate_slope_angle_test(float height_diff) {
	if (TILESIZE == 0) {
		return 0.0f;
	}
	float angle_radians = std::atan(std::abs(height_diff) / TILESIZE);
	float angle_degrees = angle_radians * 180.0f / static_cast<float>(M_PI);
	return angle_degrees;
}

float calculate_minimal_speed_test(float slope_angle, float base_speed) {
	if (slope_angle <= SLOPE_ANGLE_THRESHOLD) {
		return base_speed;
	}

	float angle_excess = slope_angle - SLOPE_ANGLE_THRESHOLD;
	float angle_ratio = angle_excess / (90.0f - SLOPE_ANGLE_THRESHOLD);
	angle_ratio = std::min(angle_ratio, 1.0f);

	float speed_reduction = angle_ratio * (1.0f - MINIMAL_SPEED_FACTOR);
	float minimal_speed = base_speed * (1.0f - speed_reduction);

	return std::max(minimal_speed, base_speed * MINIMAL_SPEED_FACTOR);
}

TEST(SlopeMovementTests, CalculateSlopeAngle45Degrees) {
	float height_diff = 32.0f;
	float expected_angle = 45.0f;
	float calculated_angle = calculate_slope_angle_test(height_diff);

	EXPECT_NEAR(calculated_angle, expected_angle, 0.5f);
}

TEST(SlopeMovementTests, CalculateSlopeAngle60Degrees) {
	float height_diff = 55.4f;
	float expected_angle = 60.0f;
	float calculated_angle = calculate_slope_angle_test(height_diff);

	EXPECT_NEAR(calculated_angle, expected_angle, 0.5f);
}

TEST(SlopeMovementTests, CalculateSlopeAngle75Degrees) {
	float height_diff = 119.4f;
	float expected_angle = 75.0f;
	float calculated_angle = calculate_slope_angle_test(height_diff);

	EXPECT_NEAR(calculated_angle, expected_angle, 0.5f);
}

TEST(SlopeMovementTests, MinimalSpeedOnFlatSlope) {
	float slope_angle = 30.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_NEAR(minimal_speed, base_speed, 0.01f);
}

TEST(SlopeMovementTests, MinimalSpeedOn45DegreeSlope) {
	float slope_angle = 45.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_NEAR(minimal_speed, base_speed, 0.01f);
}

TEST(SlopeMovementTests, MinimalSpeedOn60DegreeSlope) {
	float slope_angle = 60.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_GT(minimal_speed, 0.0f);
	EXPECT_LT(minimal_speed, base_speed);
	EXPECT_GE(minimal_speed, base_speed * 0.2f);
}

TEST(SlopeMovementTests, MinimalSpeedOn75DegreeSlope) {
	float slope_angle = 75.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_GT(minimal_speed, 0.0f);
	EXPECT_LT(minimal_speed, base_speed);
	EXPECT_GE(minimal_speed, base_speed * 0.2f);
}

TEST(SlopeMovementTests, MinimalSpeedAlwaysPositive) {
	float base_speed = 4.0f;

	for (float angle = 0.0f; angle <= 90.0f; angle += 5.0f) {
		float minimal_speed = calculate_minimal_speed_test(angle, base_speed);
		EXPECT_GT(minimal_speed, 0.0f) << "Minimal speed should be positive at angle " << angle;
	}
}

TEST(SlopeMovementTests, MinimalSpeedRespectBaseSpeed) {
	float base_speed = 4.0f;

	for (float angle = 0.0f; angle <= 90.0f; angle += 5.0f) {
		float minimal_speed = calculate_minimal_speed_test(angle, base_speed);
		EXPECT_LE(minimal_speed, base_speed) << "Minimal speed should not exceed base speed at angle " << angle;
	}
}

static float capsule_bottom_offset = 0.675f;
static float player_half_h = 2.0f;

TEST_F(Box2dManagerTest, NoInputDoesNotPushHorizontallyOn30DegSlope) {
	createSlope(0.0f, 400.0f, 692.8f, 0.0f);
	float y_surface = 10.0f - 5.0f * 0.57735f;
	setPlayerTransform({5.0f, y_surface - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position no_input(0.0f, 0.0f);
	box2dManager->change_player_position(no_input);

	b2Vec2 vel = getPlayerVelocity();
	EXPECT_NEAR(vel.x, 0.0f, 0.05f);
}

TEST_F(Box2dManagerTest, NoInputDoesNotPushHorizontallyOn45DegSlope) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);
	float y_surface = 10.0f - 2.5f * 1.0f;
	setPlayerTransform({2.5f, y_surface - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position no_input(0.0f, 0.0f);
	box2dManager->change_player_position(no_input);

	b2Vec2 vel = getPlayerVelocity();
	EXPECT_NEAR(vel.x, 0.0f, 0.05f);
}

TEST_F(Box2dManagerTest, NoInputDoesNotPushHorizontallyOn60DegSlope) {
	createSlope(0.0f, 400.0f, 230.8f, 0.0f);
	float y_surface = 10.0f - 2.0f * 1.732f;
	setPlayerTransform({2.0f, y_surface - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position no_input(0.0f, 0.0f);
	box2dManager->change_player_position(no_input);

	b2Vec2 vel = getPlayerVelocity();
	EXPECT_NEAR(vel.x, 0.0f, 0.05f);
}

TEST_F(Box2dManagerTest, NoInputNoDriftOverTimeOn30DegSlope) {
	createSlope(0.0f, 400.0f, 692.8f, 0.0f);
	float y_surface = 10.0f - 5.0f * 0.57735f;
	setPlayerTransform({5.0f, y_surface - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position no_input(0.0f, 0.0f);
	float initial_x = getPlayerPosition().x;

	for (int frame = 0; frame < 60; ++frame) {
		box2dManager->change_player_position(no_input);
		box2dManager->execute();
	}

	float final_x = getPlayerPosition().x;
	float drift = std::abs(final_x - initial_x);
	EXPECT_LT(drift, 0.1f);
}

TEST_F(Box2dManagerTest, NoInputNoDriftOverTimeOn45DegSlope) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);
	float y_surface = 10.0f - 2.5f * 1.0f;
	setPlayerTransform({2.5f, y_surface - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position no_input(0.0f, 0.0f);
	float initial_x = getPlayerPosition().x;

	for (int frame = 0; frame < 60; ++frame) {
		box2dManager->change_player_position(no_input);
		box2dManager->execute();
	}

	float final_x = getPlayerPosition().x;
	float drift = std::abs(final_x - initial_x);
	EXPECT_LT(drift, 0.15f);
}

TEST_F(Box2dManagerTest, NoInputNoDriftOverTimeOn60DegSlope) {
	createSlope(0.0f, 400.0f, 230.8f, 0.0f);
	float y_surface = 10.0f - 2.0f * 1.732f;
	setPlayerTransform({2.0f, y_surface - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position no_input(0.0f, 0.0f);
	float initial_x = getPlayerPosition().x;

	for (int frame = 0; frame < 60; ++frame) {
		box2dManager->change_player_position(no_input);
		box2dManager->execute();
	}

	float final_x = getPlayerPosition().x;
	float drift = std::abs(final_x - initial_x);
	EXPECT_LT(drift, 0.25f);
}

TEST_F(Box2dManagerTest, PlayerJumpsFromSlopeWhenNotMoving) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);
	float y_surface = 10.0f - 2.5f * 1.0f;
	setPlayerTransform({2.5f, y_surface - capsule_bottom_offset}, b2Rot_identity);

	st_float_position no_input(0.0f, 0.0f);
	for (int i = 0; i < 3; ++i) {
		box2dManager->change_player_position(no_input);
		box2dManager->execute();
	}

	b2Vec2 pos_before = getPlayerPosition();

	box2dManager->execute();
	box2dManager->player_jump();

	box2dManager->execute();

	b2Vec2 pos_after = getPlayerPosition();
	b2Vec2 vel_after = getPlayerVelocity();

	EXPECT_LT(pos_after.y, pos_before.y);
	EXPECT_LT(vel_after.y, -1.0f);
}

TEST_F(Box2dManagerTest, PlayerFallsOntoSlopeFromAbove) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);

	float start_x = 2.5f;
	float capsule_radius = capsule_bottom_offset;
	setPlayerTransform({start_x, 2.0f}, b2Rot_identity);

	st_float_position no_input(0.0f, 0.0f);
	float prev_dy = 999.0f;
	for (int i = 0; i < 240; ++i) {
		box2dManager->execute();
		box2dManager->change_player_position(no_input);

		b2Vec2 pos = getPlayerPosition();
		float dy = std::abs(pos.y - prev_dy);
		if (i > 10 && dy < 0.0001f) {
			break;
		}
		prev_dy = pos.y;
	}

	b2Vec2 final_pos = getPlayerPosition();
	EXPECT_GT(final_pos.y, 5.0f);
	EXPECT_LT(final_pos.y, 8.0f);
	float surface_at_right = 10.0f - (final_pos.x + capsule_bottom_offset * 2.0f);
	float capsule_bottom = final_pos.y + capsule_bottom_offset;
	bool right_edge_below_surface = capsule_bottom >= surface_at_right - 0.1f;
	EXPECT_TRUE(right_edge_below_surface);
}

TEST_F(Box2dManagerTest, PlayerMovesDown45DegreeSlope) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);

	setPlayerTransform({4.0f, 6.0f - capsule_bottom_offset}, b2Rot_identity);

	st_float_position move_left(-2.0f, 0.0f);

	float initial_x = getPlayerPosition().x;
	float initial_y = getPlayerPosition().y;

	for (int i = 0; i < 20; ++i) {
		box2dManager->change_player_position(move_left);
		box2dManager->execute();
	}

	float final_x = getPlayerPosition().x;
	float final_y = getPlayerPosition().y;

	ASSERT_LT(final_x, initial_x);
	ASSERT_GT(final_y, initial_y);
}

TEST_F(Box2dManagerTest, SlideDownSlope45Degrees) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);

	setPlayerTransform({3.0f, 7.0f - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position slide_down(0.0f, 1.0f);
	box2dManager->change_player_position(slide_down);

	b2Vec2 vel = getPlayerVelocity();
	float slide_speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
	float expected_speed = 10.0f * 2.0f;

	EXPECT_NEAR(slide_speed, expected_speed, 1.0f);
	EXPECT_LT(vel.x, 0.0f);
	EXPECT_GT(vel.y, 0.0f);
	EXPECT_NEAR(std::abs(vel.x), std::abs(vel.y), 1.0f);
}

TEST_F(Box2dManagerTest, SlideDownSlope60Degrees) {
	createSlope(0.0f, 400.0f, 230.8f, 0.0f);

	setPlayerTransform({2.0f, 10.0f - 2.0f * 1.732f - capsule_bottom_offset}, b2Rot_identity);

	for (int i = 0; i < 3; ++i) box2dManager->execute();

	b2Vec2 slopeCheck;
	EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

	st_float_position slide_down(0.0f, 1.0f);
	box2dManager->change_player_position(slide_down);

	b2Vec2 vel = getPlayerVelocity();
	float slide_speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
	float expected_speed = 10.0f * 2.0f;

	EXPECT_NEAR(slide_speed, expected_speed, 1.0f);
	EXPECT_LT(vel.x, 0.0f);
	EXPECT_GT(vel.y, 0.0f);
}

TEST_F(Box2dManagerTest, SlideDownFromFrozenState) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);

	setPlayerTransform({3.0f, 7.0f - capsule_bottom_offset}, b2Rot_identity);

	st_float_position no_input(0.0f, 0.0f);
	for (int i = 0; i < 10; ++i) {
		box2dManager->execute();
		box2dManager->change_player_position(no_input);
	}

	b2Vec2 pos_before_slide = getPlayerPosition();

	st_float_position slide_down(0.0f, 1.0f);
	for (int i = 0; i < 3; ++i) {
		box2dManager->execute();
		box2dManager->change_player_position(slide_down);
	}

	b2Vec2 pos_after_slide = getPlayerPosition();
	b2Vec2 vel = getPlayerVelocity();

	ASSERT_LT(pos_after_slide.x, pos_before_slide.x);
	ASSERT_GT(pos_after_slide.y, pos_before_slide.y);

	float slide_speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
	float expected_speed = 10.0f * 2.0f;
	EXPECT_NEAR(slide_speed, expected_speed, 1.0f);
}

TEST_F(Box2dManagerTest, SlideContinuesAfterReleasingDown) {
	createSlope(0.0f, 400.0f, 400.0f, 0.0f);

	setPlayerTransform({4.0f, 6.0f - capsule_bottom_offset}, b2Rot_identity);

	st_float_position no_input(0.0f, 0.0f);
	for (int i = 0; i < 5; ++i) {
		box2dManager->execute();
		box2dManager->change_player_position(no_input);
	}

	b2Vec2 pos_before = getPlayerPosition();

	st_float_position slide_down(0.0f, 1.0f);
	box2dManager->execute();
	box2dManager->change_player_position(slide_down);

	b2Vec2 pos_after_slide_start = getPlayerPosition();

	for (int i = 0; i < 5; ++i) {
		box2dManager->execute();
		box2dManager->change_player_position(no_input);
	}

	b2Vec2 pos_after_continuation = getPlayerPosition();
	b2Vec2 vel = getPlayerVelocity();

	ASSERT_LT(pos_after_continuation.x, pos_after_slide_start.x);
	ASSERT_GT(pos_after_continuation.y, pos_after_slide_start.y);

	float slide_speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
	float expected_speed = 10.0f * 2.0f;
	EXPECT_NEAR(slide_speed, expected_speed, 1.0f);
}

TEST_F(Box2dManagerTest, PlayerFallsAtCorrectSpeedWhenPressingAgainstWall) {
	std::vector<st_rectangle> walls;
	walls.push_back(st_rectangle(0, 400, 800, 40));
	box2dManager->add_static_body_rectangles(walls);

	walls.clear();
	walls.push_back(st_rectangle(140, 0, 8, 400));
	box2dManager->add_static_body_rectangles(walls);

	float player_w = 1.35f;
	float wall_x = 3.5f;
	float player_body_x = wall_x - player_w;
	setPlayerTransform({player_body_x, 2.0f}, b2Rot_identity);

	box2dManager->player_jump();

	b2Vec2 prev_vel = getPlayerVelocity();
	float gravity_step = 39.6f * (1.0f / 60.0f);

	st_float_position push_right(2.0f, 0.0f);
	b2Vec2 vel;
	for (int frame = 0; frame < 30; ++frame) {
		box2dManager->execute();
		box2dManager->change_player_position(push_right);
		vel = getPlayerVelocity();

		if (frame > 5 && frame > 0) {
			float vy_change = vel.y - prev_vel.y;
			EXPECT_NEAR(vy_change, gravity_step, 0.25f);
		}
		prev_vel = vel;
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
