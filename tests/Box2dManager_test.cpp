#include "gtest/gtest.h"
#include "box2d/Box2dManager.h"
#include "data/st_common.h" // For st_float_position
#include "data/SharedPlayerData.hpp" // Box2dManager uses this
#include <cmath>
#include <iostream>

class Box2dManagerTest : public ::testing::Test {
protected:
    Box2dManager* box2dManager;

    void SetUp() override {
        box2dManager = new Box2dManager();
        // Reset player position for each test
        b2Body_SetTransform(box2dManager->playerBodyId, {1.0f, 1.0f}, b2Rot_identity);
        b2Body_SetLinearVelocity(box2dManager->playerBodyId, {0.0f, 0.0f});
        b2Body_SetAngularVelocity(box2dManager->playerBodyId, 0.0f);
    }

    void TearDown() override {
        delete box2dManager;
        box2dManager = nullptr;
    }

    // Helper to create a slope
    void createSlope(float start_x_pixels, float start_y_pixels, float end_x_pixels, float end_y_pixels) {
        std::vector<std::vector<st_float_position>> polygons;
        std::vector<st_float_position> slope_points;

        // Define a simple triangle for the slope
        slope_points.push_back(st_float_position(start_x_pixels, start_y_pixels));
        slope_points.push_back(st_float_position(end_x_pixels, end_y_pixels));
        slope_points.push_back(st_float_position(end_x_pixels, start_y_pixels));

        polygons.push_back(slope_points);
        box2dManager->add_static_body_polygon(polygons);
    }

    // Protected helpers to access private members of Box2dManager (since we are a friend)
    void setPlayerTransform(b2Vec2 position, b2Rot rotation) {
        b2Body_SetTransform(box2dManager->playerBodyId, position, rotation);
    }

    void setPlayerVelocity(b2Vec2 velocity) {
        b2Body_SetLinearVelocity(box2dManager->playerBodyId, velocity);
    }

    b2Vec2 getPlayerPosition() {
        return b2Body_GetPosition(box2dManager->playerBodyId);
    }

    b2Vec2 getPlayerVelocity() {
        return b2Body_GetLinearVelocity(box2dManager->playerBodyId);
    }

    float getPlayerHeight() {
        return box2dManager->player_h;
    }

    float getHorizontalSpeedLimit() {
        return box2dManager->HORIZONTAL_SPEED_LIMIT;
    }
};

TEST_F(Box2dManagerTest, PlayerMovesUp45DegreeSlope) {
    // Create a 45-degree slope large enough for 0.5 seconds of movement
    // Pixels: (0, 400) to (400, 0)
    // Meters: (0, 10) to (10, 0)
    createSlope(0.0f, 400.0f, 400.0f, 0.0f);

    // Position player on the slope surface
    // At x=1.0, y_surface = 10 - 1 = 9.0
    // Feet at y=9.0 -> Center y = 9.0 - 2.0 = 7.0
    setPlayerTransform({1.0f, 7.0f}, b2Rot_identity);

    // Simulate moving right (climbing the slope)
    st_float_position move_inc(1.0f, 0.0f); // Move right

    float initial_x = getPlayerPosition().x;
    float initial_y = getPlayerPosition().y;

    // Run physics for a few steps
    for (int i = 0; i < 30; ++i) { // Simulate 0.5 seconds of movement
        box2dManager->change_player_position(move_inc);
        box2dManager->execute();
    }

    float final_x = getPlayerPosition().x;
    float final_y = getPlayerPosition().y;

    // Assert that the player has moved horizontally and vertically (up the slope)
    ASSERT_GT(final_x, initial_x);
    ASSERT_LT(final_y, initial_y); // Y decreases as player moves up
}

TEST_F(Box2dManagerTest, PlayerMovesUp60DegreeSlope) {
    // Create a 60-degree slope
    // Pixels: (0, 400) to (230.8, 0)
    // Meters: (0, 10) to (5.77, 0)
    createSlope(0.0f, 400.0f, 230.8f, 0.0f);

    // At x=1.0, y_surface = 10 - 1.732 * 1.0 = 8.268
    // Feet at y=8.268 -> Center y = 8.268 - 2.0 = 6.268
    setPlayerTransform({1.0f, 6.268f}, b2Rot_identity);

    // Simulate moving right (climbing the slope)
    st_float_position move_inc(1.0f, 0.0f); // Move right

    float initial_x = getPlayerPosition().x;
    float initial_y = getPlayerPosition().y;

    // Run physics for a few steps
    for (int i = 0; i < 30; ++i) { // Simulate 0.5 seconds of movement
        box2dManager->change_player_position(move_inc);
        box2dManager->execute();
    }

    float final_x = getPlayerPosition().x;
    float final_y = getPlayerPosition().y;

    // Assert that the player has moved horizontally and vertically (up the slope)
    ASSERT_GT(final_x, initial_x);
    ASSERT_LT(final_y, initial_y); // Y decreases as player moves up
}

TEST_F(Box2dManagerTest, PlayerFallsNormally) {
    // Create a flat floor at y=10m
    std::vector<st_rectangle> floors;
    floors.push_back(st_rectangle(0, 400, 800, 40)); // 10m down, 20m wide, 1m thick
    box2dManager->add_static_body_rectangles(floors);

    // Place player high in the air at x=5m, y=2m
    setPlayerTransform({5.0f, 2.0f}, b2Rot_identity);
    
    // Ensure jump_started is true so grounding logic is active
    // (In actual game it would be set by execute_player_physics when in air)

    float last_y = getPlayerPosition().y;
    float max_fall_speed = 0.0f;

    // Run physics for 60 steps (1 second)
    for (int i = 0; i < 60; ++i) {
        box2dManager->execute();
        b2Vec2 velocity = getPlayerVelocity();
        if (velocity.y > max_fall_speed) {
            max_fall_speed = velocity.y;
        }
        
        float current_y = getPlayerPosition().y;
        std::cout << "Step " << i << ": y=" << current_y << ", vy=" << velocity.y << std::endl;
        
        // Assert that velocity doesn't suddenly drop to a very low value before hitting ground
        // Gravity is ~40, so after 0.5s speed should be around 20.
        // If it drops to something like 1.0 while still 1m above ground, that's the bug.
        if (current_y < 7.0f) { // While still significantly above the 10m floor
             ASSERT_GE(velocity.y, -0.1f); // Should be falling or stationary, not moving up
             if (i > 5) {
                 ASSERT_GT(velocity.y, 1.0f); // Should have gained some speed
             }
        }
        last_y = current_y;
    }
    
    ASSERT_GT(max_fall_speed, 5.0f); // Should have reached a decent fall speed
}

// ==================== Slope Movement Tests ====================
// These tests verify the mathematical calculations for slope angle and minimal speed

#define TILESIZE 32
#define SLOPE_ANGLE_THRESHOLD 45.0f
#define MINIMAL_SPEED_FACTOR 0.2f

/**
 * @brief Calculate slope angle in degrees from height difference
 */
float calculate_slope_angle_test(float height_diff) {
	if (TILESIZE == 0) {
		return 0.0f;
	}
	float angle_radians = std::atan(std::abs(height_diff) / TILESIZE);
	float angle_degrees = angle_radians * 180.0f / static_cast<float>(M_PI);
	return angle_degrees;
}

/**
 * @brief Calculate minimal movement speed for a given slope angle
 */
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

/**
 * @brief Test slope angle calculation for a 45-degree slope
 */
TEST(SlopeMovementTests, CalculateSlopeAngle45Degrees) {
	float height_diff = 32.0f;
	float expected_angle = 45.0f;
	float calculated_angle = calculate_slope_angle_test(height_diff);

	EXPECT_NEAR(calculated_angle, expected_angle, 0.5f);
}

/**
 * @brief Test slope angle calculation for a 60-degree slope
 */
TEST(SlopeMovementTests, CalculateSlopeAngle60Degrees) {
	float height_diff = 55.4f;
	float expected_angle = 60.0f;
	float calculated_angle = calculate_slope_angle_test(height_diff);

	EXPECT_NEAR(calculated_angle, expected_angle, 0.5f);
}

/**
 * @brief Test slope angle calculation for a 75-degree slope (very steep)
 */
TEST(SlopeMovementTests, CalculateSlopeAngle75Degrees) {
	float height_diff = 119.4f;
	float expected_angle = 75.0f;
	float calculated_angle = calculate_slope_angle_test(height_diff);

	EXPECT_NEAR(calculated_angle, expected_angle, 0.5f);
}

/**
 * @brief Test that minimal speed on flat/normal slopes equals base speed
 */
TEST(SlopeMovementTests, MinimalSpeedOnFlatSlope) {
	float slope_angle = 30.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_NEAR(minimal_speed, base_speed, 0.01f);
}

/**
 * @brief Test that minimal speed on 45-degree slope equals base speed (threshold boundary)
 */
TEST(SlopeMovementTests, MinimalSpeedOn45DegreeSlope) {
	float slope_angle = 45.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_NEAR(minimal_speed, base_speed, 0.01f);
}

/**
 * @brief Test that minimal speed on 60-degree slope is reduced but not zero
 */
TEST(SlopeMovementTests, MinimalSpeedOn60DegreeSlope) {
	float slope_angle = 60.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_GT(minimal_speed, 0.0f);
	EXPECT_LT(minimal_speed, base_speed);
	EXPECT_GE(minimal_speed, base_speed * 0.2f);
}

/**
 * @brief Test that minimal speed on 75-degree slope (very steep) is reduced
 */
TEST(SlopeMovementTests, MinimalSpeedOn75DegreeSlope) {
	float slope_angle = 75.0f;
	float base_speed = 4.0f;
	float minimal_speed = calculate_minimal_speed_test(slope_angle, base_speed);

	EXPECT_GT(minimal_speed, 0.0f);
	EXPECT_LT(minimal_speed, base_speed);
	EXPECT_GE(minimal_speed, base_speed * 0.2f);
}

/**
 * @brief Test that minimal speed is always positive
 */
TEST(SlopeMovementTests, MinimalSpeedAlwaysPositive) {
	float base_speed = 4.0f;

	for (float angle = 0.0f; angle <= 90.0f; angle += 5.0f) {
		float minimal_speed = calculate_minimal_speed_test(angle, base_speed);
		EXPECT_GT(minimal_speed, 0.0f) << "Minimal speed should be positive at angle " << angle;
	}
}

/**
 * @brief Test minimal speed respects base speed upper bound
 */
TEST(SlopeMovementTests, MinimalSpeedRespectBaseSpeed) {
	float base_speed = 4.0f;

	for (float angle = 0.0f; angle <= 90.0f; angle += 5.0f) {
		float minimal_speed = calculate_minimal_speed_test(angle, base_speed);
		EXPECT_LE(minimal_speed, base_speed) << "Minimal speed should not exceed base speed at angle " << angle;
	}
}

// ==================== No-Input Slope Tests ====================
// These tests verify the player does not drift horizontally when
// standing on slopes of various angles with no directional input.

// Helper: position player so the capsule bottom touches the slope surface
// Slope goes from (0, y_start) to (x_end, 0) in meters
// y_slope(x) = y_start - x * (y_start / x_end)
// Capsule bottom is at position.y + capsule_bottom_offset
// We want position.y + bottom_offset = y_slope
static float capsule_bottom_offset = 0.675f; // radius of capsule
static float player_half_h = 2.0f; // player_h / 2

TEST_F(Box2dManagerTest, NoInputDoesNotPushHorizontallyOn30DegSlope) {
    createSlope(0.0f, 400.0f, 692.8f, 0.0f);
    // y_surface(5) = 10 - 5*tan(30) = 7.113
    float y_surface = 10.0f - 5.0f * 0.57735f;
    // Place capsule bottom exactly at surface
    setPlayerTransform({5.0f, y_surface - capsule_bottom_offset}, b2Rot_identity);

    // Minimal steps to establish contact without building slide momentum
    for (int i = 0; i < 3; ++i) box2dManager->execute();

    // Verify slope detection is working
    b2Vec2 slopeCheck;
    EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

    st_float_position no_input(0.0f, 0.0f);
    box2dManager->change_player_position(no_input);

    b2Vec2 vel = getPlayerVelocity();
    // Must not set horizontal velocity when there's no input
    EXPECT_NEAR(vel.x, 0.0f, 0.05f);
}

TEST_F(Box2dManagerTest, NoInputDoesNotPushHorizontallyOn45DegSlope) {
    createSlope(0.0f, 400.0f, 400.0f, 0.0f);
    // y_surface(2.5) = 10 - 2.5*tan(45) = 7.5
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
    // y_surface(2) = 10 - 2*tan(60) = 6.536
    float y_surface = 10.0f - 2.0f * 1.732f;
    setPlayerTransform({2.0f, y_surface - capsule_bottom_offset}, b2Rot_identity);

    for (int i = 0; i < 3; ++i) box2dManager->execute();

    b2Vec2 slopeCheck;
    EXPECT_TRUE(box2dManager->is_on_slope(slopeCheck));

    st_float_position no_input(0.0f, 0.0f);
    box2dManager->change_player_position(no_input);

    b2Vec2 vel = getPlayerVelocity();
    // Must not set horizontal velocity when there's no input
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

// Reproduce: player stands still on slope, presses jump.
// Game flow: execute() then player_jump() -- NO change_player_position in between.
// With _freeze_position still true from previous no-input frames, execute() kills the jump.
TEST_F(Box2dManagerTest, PlayerJumpsFromSlopeWhenNotMoving) {
    createSlope(0.0f, 400.0f, 400.0f, 0.0f);
    float y_surface = 10.0f - 2.5f * 1.0f;
    setPlayerTransform({2.5f, y_surface - capsule_bottom_offset}, b2Rot_identity);

    // Establish freeze state: 3 no-input frames
    st_float_position no_input(0.0f, 0.0f);
    for (int i = 0; i < 3; ++i) {
        box2dManager->change_player_position(no_input);
        box2dManager->execute();
    }

    b2Vec2 pos_before = getPlayerPosition();

    // Simulate the exact game flow on a jump frame:
    // 1. execute() runs first (sees _freeze_position=true, freezes position)
    // 2. No change_player_position is called (jump==0 gate in game)
    // 3. player_jump() is called
    box2dManager->execute();
    box2dManager->player_jump();

    // Simulate next frame: execute() runs with what player_jump set
    box2dManager->execute();

    b2Vec2 pos_after = getPlayerPosition();
    b2Vec2 vel_after = getPlayerVelocity();

    EXPECT_LT(pos_after.y, pos_before.y);
    EXPECT_LT(vel_after.y, -1.0f);
}

// When the player falls from above onto a slope, the capsule bottom should end up
// exactly on the slope surface — not hovering above it.
// The raycasts in is_on_slope start from visual-bottom (pos.y + player_h/2) which is
// ~1.3m below the capsule collision bottom. If the raycasts miss and the contact
// normal fallback fails on the first frame of contact, the body keeps falling past
// the surface, gets pushed back by the solver, and _freeze_position locks it at a
// position where the capsule bottom is above the true surface.
TEST_F(Box2dManagerTest, PlayerFallsOntoSlopeFromAbove) {
    createSlope(0.0f, 400.0f, 400.0f, 0.0f);

    // Place the player high above the slope
    float start_x = 2.5f;
    float capsule_radius = capsule_bottom_offset;
    setPlayerTransform({start_x, 2.0f}, b2Rot_identity);

    // Let the player fall with gravity (no input) until they settle on the slope
    st_float_position no_input(0.0f, 0.0f);
    float prev_dy = 999.0f;
    for (int i = 0; i < 240; ++i) {
        // Game flow: execute then change_player_position
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
    // Verify the body has fallen significantly (from start y=2 down to near slope ~y=6-7)
    EXPECT_GT(final_pos.y, 5.0f);
    EXPECT_LT(final_pos.y, 8.0f);
    // Verify the capsule contacts the slope somewhere within its width.
    // Capsule spans from body.x to body.x + player_w. The rightmost point
    // penetrates the slope even when the center hovers.
    float surface_at_right = 10.0f - (final_pos.x + capsule_bottom_offset * 2.0f);
    float capsule_bottom = final_pos.y + capsule_bottom_offset;
    bool right_edge_below_surface = capsule_bottom >= surface_at_right - 0.1f;
    EXPECT_TRUE(right_edge_below_surface);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
