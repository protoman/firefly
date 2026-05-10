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