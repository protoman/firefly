#include "gtest/gtest.h"
#include "box2d/Box2dManager.h"
#include "data/st_common.h" // For st_float_position
#include "data/SharedPlayerData.hpp" // Box2dManager uses this

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
    // Create a 45-degree slope
    createSlope(0.0f, 100.0f, 100.0f, 0.0f);

    // Place player at the bottom-left of the slope
    setPlayerTransform({0.5f, 2.5f - (getPlayerHeight() / 2.0f) - 0.01f}, b2Rot_identity);

    // Simulate moving right (climbing the slope)
    st_float_position move_inc(1.0f, 0.0f); // Move right

    float initial_x = getPlayerPosition().x;
    float initial_y = getPlayerPosition().y;

    // Run physics for a few steps
    for (int i = 0; i < 60; ++i) { // Simulate 1 second of movement
        box2dManager->change_player_position(move_inc);
        box2dManager->execute();
    }

    float final_x = getPlayerPosition().x;
    float final_y = getPlayerPosition().y;

    // Assert that the player has moved horizontally and vertically (up the slope)
    ASSERT_GT(final_x, initial_x);
    ASSERT_LT(final_y, initial_y); // Y decreases as player moves up
    // Also check if the speed is reduced as expected.
    b2Vec2 final_velocity = getPlayerVelocity();
    float expected_max_speed = getHorizontalSpeedLimit() * SLOPE_CLIMB_SPEED_FACTOR;
    ASSERT_LT(final_velocity.x, expected_max_speed + 0.1f); // Allow for some floating point error
    ASSERT_GT(final_velocity.x, 0.0f); // Should be moving
}

TEST_F(Box2dManagerTest, PlayerMovesUp60DegreeSlope) {
    // Create a 60-degree slope
    createSlope(0.0f, 100.0f, 57.7f, 0.0f);

    // Place player at the bottom-left of the slope
    setPlayerTransform({0.5f, 2.5f - (getPlayerHeight() / 2.0f) - 0.01f}, b2Rot_identity);

    // Simulate moving right (climbing the slope)
    st_float_position move_inc(1.0f, 0.0f); // Move right

    float initial_x = getPlayerPosition().x;
    float initial_y = getPlayerPosition().y;

    // Run physics for a few steps
    for (int i = 0; i < 60; ++i) { // Simulate 1 second of movement
        box2dManager->change_player_position(move_inc);
        box2dManager->execute();
    }

    float final_x = getPlayerPosition().x;
    float final_y = getPlayerPosition().y;

    // Assert that the player has moved horizontally and vertically (up the slope)
    ASSERT_GT(final_x, initial_x);
    ASSERT_LT(final_y, initial_y); // Y decreases as player moves up
    // Also check if the speed is reduced as expected.
    b2Vec2 final_velocity = getPlayerVelocity();
    float expected_max_speed = getHorizontalSpeedLimit() * SLOPE_CLIMB_SPEED_FACTOR;
    ASSERT_LT(final_velocity.x, expected_max_speed + 0.1f); // Allow for some floating point error
    ASSERT_GT(final_velocity.x, 0.0f); // Should be moving
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
