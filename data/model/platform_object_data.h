#ifndef PLATFORM_OBJECT_DATA_H
#define PLATFORM_OBJECT_DATA_H

#include "object_data.h"

enum class PlatformObjectDataTypeEnum {
    Moving,
    Disappearing_Block,
    Falling_Platform,
    Flying_Platform,
    Horizontal_Expanding_Ray,
    Vertical_Expanding_Ray,
    Horizontal_Expanding_Death_Ray,
    Vertical_Expanding_Death_Ray,
    Door,
    Track_Platform,
    Destructible_Wall,
    Destructible,
    Destructible_Jump,
    Timed_Bomb
};

enum class PlatformObjectDataMovingDirectionEnum {
    Left,
    Right,
    Up,
    Down,
    LeftUp,
    RightUp,
    LeftDown,
    RightDown
};

class PlatformObjectData : public ObjectData {
public:
    PlatformObjectData();
    ~PlatformObjectData() override = default;

    // Getters
    PlatformObjectDataTypeEnum get_type() const;
    PlatformObjectDataMovingDirectionEnum get_moving_direction() const;
    int get_timer_limit() const;
    float get_movement_speed() const;
    float get_movement_limit() const;
    bool get_activate_only_when_over_it() const;
    bool get_animate_only_when_active() const;
    bool get_animation_loop() const;
    bool get_animation_loop_in_reverse() const;
    int get_door_key() const;

    // Setters
    void set_type(PlatformObjectDataTypeEnum type);
    void set_moving_direction(PlatformObjectDataMovingDirectionEnum direction);
    void set_timer_limit(int limit);
    void set_movement_speed(float speed);
    void set_movement_limit(float limit);
    void set_activate_only_when_over_it(bool activate);
    void set_animate_only_when_active(bool animate);
    void set_animation_loop(bool loop);
    void set_animation_loop_in_reverse(bool reverse);
    void set_door_key(int key);

private:
    PlatformObjectDataTypeEnum m_type;
    PlatformObjectDataMovingDirectionEnum m_moving_direction;
    int m_timer_limit;
    float m_movement_speed;
    float m_movement_limit;
    bool m_activate_only_when_over_it;
    bool m_animate_only_when_active;
    bool m_animation_loop;
    bool m_animation_loop_in_reverse;
    int m_door_key;
};

#endif // PLATFORM_OBJECT_DATA_H
