cmake_minimum_required(VERSION 3.16)

project(FireFlyGameTests)

# Enable CTest
enable_testing()

# Add the Box2dManager unit test executable
add_executable(Box2dManager_test Box2dManager_test.cpp)

# Link against GTest and the main game library (FireFlyGame)
target_link_libraries(Box2dManager_test
    GTest::gtest_main
    FireFlyGameLib # Link against the game library
)

# Add the test to CTest
add_test(NAME Box2dManager_test COMMAND Box2dManager_test)
