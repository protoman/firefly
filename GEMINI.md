# Project Context

This is a C++ game that aims to be a Metroidvania-like similar to Shantae Half-Genie Hero.

## Project Architecture
- The build system is cmake-based.
- It uses SDL library for graphics, sound, timers, threads and input.
- It does use Box2D for physics and collisions.
- For map data structure it uses Tiled Map editor.
- It also does have its own editor, located in data_editor folder, for enemies, NPCs, objects and more.

## General Instructions
- When generating new code, the target language is **C++23**.
- Prioritize using modern C++ features and standard library components (e.g., `std::vector`, `std::string`, `std::unique_ptr`).
- Avoid raw pointers and manual memory management where possible.
- Ensure all new functions and classes have Doxygen-style comments.
- Ensure all functions that handle player and non-playable characters physics and collision have unit tests.
- The code should compile with `clang++` and use `CMake` for the build system.
- Do not remove `std::` from the namespace prefix for types.
- Always build the project to check it is working before and after starting doing changes.
- When the changed code has unit-tests, run it to check logic is working.
- Always ask before removing existing functions.
- This is a multi-platform project, so avoid using methods that only work on one operating system or architecture.
- Do not change the size of arrays used as file.
- The file format for maps is TMX (Tiled Map XML).
- The file format for objects, enemies, NPCs, dialogs, quests and other is JSON.
- The JSON files are handled by a library called Cereal, located in the cereal folder in the project.
- The data-files should have serialization and deserialization methods for cereal.  
- The game uses singletons, called controllers, to handle specific tasks such as input handling or map loading and moving.
- Replace instances of non-safe or deprecated methods like `sprintf`, `vsprintf`, `snprintf`, `strcpy`, `strcat`, `strncpy`, `strncat`, `strtok`, `alloca`, and `realpath` with memory-safe modern C++ alternatives.
- Prefer using `std::format`, `std::print`, or `std::println` (C++23) over `sprintf`, `snprintf`, or `std::stringstream` for type-safe and efficient string formatting and output.
- The game uses singletons, called controllers, to handle specific tasks such as input handling or map loading and moving.
- The game uses singletons, called shared(something), to access data between classes.

## Coding Style
- Use tab for indentation.
- Class names should use `PascalCase`.
- Function and variable names should use `snake_case`.
- Use `const` and `&` for function parameters when appropriate to avoid unnecessary copies and enable passing temporary objects.
- Use `enum class` for enumerations.
- Use `override` and `final` specifiers where appropriate for virtual functions.


## Box2D Physics System

### Coordinate System
- Box2D uses a **Y-down** convention (gravity pulls toward positive Y, matching screen coordinates).
- All positions and velocities use meters. Convert to pixels via `PIXELS_PER_METER = 40`.
- The player body position (`b2Body_GetPosition`) represents the **center** of the player's visual rectangle:
  - Visual rectangle: `(pos.x - player_w/2, pos.y - player_h/2, player_w, player_h)` in meters
  - `get_player_box()` converts to pixels: `(pos.x - player_w/2) * 40` for x, `(pos.y - player_h/2) * 40` for y
- **Capsule shape**: `center1 = (player_w/2, -player_h/2)` (top center), `center2 = (player_w/2, 0)` (bottom center), `radius = player_w/2`.
- The capsule bottom-most point is at `body.y + radius = body.y + 0.675m` (27 pixels below center).

### Box2dManager Architecture (`box2d/Box2dManager.h` / `.cpp`)
- Owns the `b2WorldId` and manages all physics bodies.
- Player is a **dynamic capsule** (`b2Capsule`) with fixed rotation.
- Static geometry is added via two methods:
  - `add_static_body_rectangles()` — simple box shapes from `st_rectangle` data.
  - `add_static_body_polygon()` — polygon shapes from vertex lists.
- `execute()` advances the simulation: `b2World_Step(worldId, 1/60, 4)`.
  - Saves `_vel_before_step` and position **before** the step for use by wall-friction correction and position-freeze logic.
  - If `_freeze_position` is true (no-input on slope/ground), restores the pre-step position and zeros velocity — eliminates solver-introduced drift.
- `execute_player_physics()` is defined but **NOT called** in the main game loop (may cause jump-state issues).

### Key Constants (`Box2dManager.h`)
| Constant | Value | Purpose |
|---|---|---|
| `PIXELS_PER_METER` | 40 | Conversion factor |
| `GRAVITY` | 39.6 | Downward acceleration (m/s²) |
| `HORIZONTAL_SPEED_LIMIT` | 10.0 | Max horizontal speed (m/s) |
| `HORIZONTAL_MOVE_FORCE` | 4.0 | Impulse applied per frame for horizontal movement |
| `SLOPE_CLIMB_SPEED_FACTOR` | 0.5 | Speed multiplier when climbing slopes (vx = 10 * 0.5 = 5 m/s) |
| `SLIDE_DOWN_SPEED_MULTIPLIER` | 2.0 | Speed multiplier for slide (vx = 10 * 2 = 20 m/s) |
| `PLAYER_FRICTION` | 1.0 | Player body friction |
| `SCENARIO_FRICTION` | 0.5 | Static geometry friction |
| `player_w` | 54/40 = 1.35m | Player width |
| `player_h` | 160/40 = 4.0m | Player height |

### Slope Detection (`is_on_slope()`)
Uses a **two-tier detection system**:
1. **Raycasts**: 3 rays cast downward from across the player's width (20%, 50%, 80% of capsule width). Ray length = 0.15m (~6 pixels). If any ray hits a surface with `|normal.x| > 0.1` and `normal.y < -0.1`, it's a slope.
2. **Contact normals fallback**: Reads `b2Body_GetContactData()` and checks contact manifold normals for significant X components — more reliable when the capsule is actively in contact with the slope but raycasts miss.

Both checks use the same normal condition: `|normal.x| > 0.1f && normal.y < -0.1f`.
The returned `slopeNormal` inverts the hit normal: `out_normal = {-hit_normal.x, -hit_normal.y}`.

### Slope Climbing (`change_player_position()`)
When the player has horizontal input AND is on a slope:
- `climbing_right = (inc.x > 0 && slopeNormal.x > 0.1f)` — moving right into a right-up slope.
- `climbing_left = (inc.x < 0 && slopeNormal.x < -0.1f)` — moving left into a left-up slope.
- Velocity is **set directly** (not impulse): projects `speed_limit` along the slope tangent:
  - `speed_limit = HORIZONTAL_SPEED_LIMIT * SLOPE_CLIMB_SPEED_FACTOR = 5.0 m/s`
  - `slope_multiplier = -slopeNormal.x / slopeNormal.y`
  - `target_vx = speed_limit` (in movement direction)
  - `target_vy = target_vx * slope_multiplier`
- This sets the player's velocity to move along the slope surface at reduced speed.

### No-Input Behavior
When `inc.x == 0.0f`:
- **On slope** (`onSlope && !jump_started`): Sets velocity to `{0.0f, currentVelocity.y}` — stops horizontal drift but preserves vertical velocity so the player stays in contact with the slope surface.
- **On flat ground** (`groundType == PLAYER_GROUND_LINEAR && !jump_started`): Sets velocity to `{0.0f, 0.0f}` — full stop.
- **In air** (neither): Returns without modifying velocity — gravity handles movement.
- In both ground cases, `_freeze_position = true` is set. On the next `execute()`, the pre-step position is restored and velocity zeroed, eliminating solver-introduced drift.
- When the player provides input again or jumps, `_freeze_position` is cleared.
- **Historical note**: The old code set `{-slopeNormal.x * 0.2f, currentVelocity.y}` on slopes, which caused the player to slide backward horizontally on slopes > 45°.

### Slope Descent (`change_player_position()`)
When moving with horizontal input down a slope (e.g., moving right on a left-up slope or left on a right-up slope):
- `descending_right = (inc.x > 0 && slopeNormal.x < -0.1f)` — moving right down a left-up slope.
- `descending_left = (inc.x < 0 && slopeNormal.x > 0.1f)` — moving left down a right-up slope.
- Velocity is **set directly** at full horizontal speed projected along the slope tangent:
  - `slope_multiplier = -slopeNormal.x / slopeNormal.y`
  - `target_vx = HORIZONTAL_SPEED_LIMIT * sign(inc.x)`
  - `target_vy = target_vx * slope_multiplier`
- This keeps the player on the slope surface without floating, unlike the old impulse-based approach.

### Slide State Machine
When `inc.y > 0` (DOWN pressed) on a slope:
- Enters `_is_sliding = true` state.
- Velocity is set along the **downhill tangent**: `(-slopeNormal.y, slopeNormal.x)` normalized, then scaled by `HORIZONTAL_SPEED_LIMIT * SLIDE_DOWN_SPEED_MULTIPLIER = 20 m/s`.
- Once sliding, the state persists until:
  - Horizontal velocity drops below 30% of the expected target (wall/obstruction), OR
  - The player leaves the slope onto flat ground (one frame of `_slide_coasting` before full stop).
- On slide exit, velocity is zeroed and `_freeze_position` takes over.

### Wall Friction Correction
During airborne jumps/falls (`jump_started && !onSlope && groundType == PLAYER_GROUND_NONE`):
- Box2D's solver can reduce vertical speed when the player is pressed against a wall.
- `execute()` saves `_vel_before_step` before `b2World_Step`.
- In `change_player_position()`, the post-step Y velocity is compared with the expected value: `_vel_before_step.y + GRAVITY * timeStep`.
- If `|actual_vy - expected_vy| > 0.1`, the velocity is corrected to the expected Y value (X is untouched).
- This prevents wall contact from slowing the player's jump or accelerating the fall.

### Settling Delay
When landing on a slope (ground type transitions to `PLAYER_GROUND_SLOPE`):
- `_settle_counter` increments each frame once `inc.x == 0` and the player is on a slope.
- After 5 frames of no input on the slope, `_freeze_position = true` is set.
- The 5-frame delay allows the body to settle to equilibrium after landing from a jump/fall.
- Moving, jumping, or sliding resets `_settle_counter` to 0.

### Slope Transition Boost
When moving with input and NOT currently on a slope, but `_last_slope_normal` (from the most recent frame where `is_on_slope` was true) indicates the player recently left a slope:
- If moving in the same direction as the slope normal and vertical velocity is near-zero, a small vertical impulse is applied to help clear the transition edge.
- `vy_boost = -last_normal.x / last_normal.y * 0.3f`

### Two Parallel Movement Systems
The game has **two independent movement systems** that run concurrently:
1. **Tile-based system** (`character::charMove()`): Modifies `character::position` using tile-map collision checks (`map_collision()`), slope detection via `moveSlopes` class, and `TILESIZE=32` grid logic.
2. **Box2D system** (`Box2dManager`): Controls the Box2D body position through impulses and velocity.

In `GameManager::show_game()`:
```
box2d_manager.execute();                    // Step Box2D physics
player1.execute();                           // Calls move() + charMove() (tile-based)
box2d_manager.change_player_position(inc);   // Apply Box2D movement based on input
```
The Box2D body position is used to compute the visual rectangle via `get_player_box()`.

### Movement Flow (Box2D path)
1. `GameManager` reads `player1.getMoveCommands()` to determine input.
2. `box2d_manager.execute()` — step physics.
3. `box2d_manager.change_player_position(inc)` — apply Box2D movement based on input, where:
   - `inc.x` comes from `moveCommands.right - moveCommands.left` (typically ±2.0f).
   - `inc.y` comes from `moveCommands.down` (2.0f when DOWN is pressed, triggering slide).
4. `Box2dManager::change_player_position()` handles all slope/climb/descent/slide/wall-friction logic.

### Unit Tests (`tests/Box2dManager_test.cpp`)
- Uses Google Test framework.
- `Box2dManagerTest` fixture creates a fresh `Box2dManager` per test and provides helpers: `createSlope()`, `setPlayerTransform()`, `getPlayerPosition()`, `getPlayerVelocity()`.
- `friend class Box2dManagerTest` in `Box2dManager.h` grants test access to private members.
- Slopes are created as right-triangle polygons: `createSlope(start_x_px, start_y_px, end_x_px, end_y_px)`.
- Slope surface Y at a given X: `y_surface = Y_START - X * tan(angle)`.
- Player capsule bottom offset from body position: `capsule_bottom_offset = radius = player_w/2 = 0.675m`.
- To position player on slope surface: `body.y = y_surface - capsule_bottom_offset`.
- Existing tests: 45° climbing, 60° climbing, falling, no-input drift at 30°/45°/60°, jump-from-slope, fall-onto-slope, 45° descent, 45°/60° slide, slide-from-frozen, slide-continuation, wall-friction.

### Future Work / Known Issues
- `execute_player_physics()` is defined but never called in the game loop — the jump state (`jump_started`) is only managed inside `get_player_box()` as a side effect.
- The two parallel movement systems (tile-based and Box2D) can conflict — the tile-based `charMove()` modifies `character::position` while Box2D independently controls the body position. There's no synchronization between them.
- `add_static_body_polygon()` has a bug: it uses `groundBodyDef` (default position at origin) instead of the computed position when creating the body on line 137.
- The `SLOPE_REDUCED_SPEED_FACTOR` define is no longer used after the no-input fix.

## Example C++ Code Snippet (for context)
```cpp
// include/utils.h
#pragma once

#include <string>

/**
 * @brief Converts a string to uppercase.
 * @param input The input string.
 * @return The uppercase string.
 */
std::string to_uppercase(const std::string& input);