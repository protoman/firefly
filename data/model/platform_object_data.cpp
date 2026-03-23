#include "platform_object_data.h"

PlatformObjectData::PlatformObjectData()
    : ObjectData(),
      m_type(PlatformObjectDataTypeEnum::Moving),
      m_moving_direction(PlatformObjectDataMovingDirectionEnum::Right),
      m_timer_limit(0),
      m_movement_speed(0.0f),
      m_movement_limit(0.0f),
      m_activate_only_when_over_it(false),
      m_animate_only_when_active(false),
      m_animation_loop(true),
      m_animation_loop_in_reverse(false),
      m_door_key(0) {
}

PlatformObjectDataTypeEnum PlatformObjectData::get_type() const {
    return m_type;
}

PlatformObjectDataMovingDirectionEnum PlatformObjectData::get_moving_direction() const {
    return m_moving_direction;
}

int PlatformObjectData::get_timer_limit() const {
    return m_timer_limit;
}

float PlatformObjectData::get_movement_speed() const {
    return m_movement_speed;
}

float PlatformObjectData::get_movement_limit() const {
    return m_movement_limit;
}

bool PlatformObjectData::get_activate_only_when_over_it() const {
    return m_activate_only_when_over_it;
}

bool PlatformObjectData::get_animate_only_when_active() const {
    return m_animate_only_when_active;
}

bool PlatformObjectData::get_animation_loop() const {
    return m_animation_loop;
}

bool PlatformObjectData::get_animation_loop_in_reverse() const {
    return m_animation_loop_in_reverse;
}

int PlatformObjectData::get_door_key() const {
    return m_door_key;
}

void PlatformObjectData::set_type(PlatformObjectDataTypeEnum type) {
    m_type = type;
}

void PlatformObjectData::set_moving_direction(PlatformObjectDataMovingDirectionEnum direction) {
    m_moving_direction = direction;
}

void PlatformObjectData::set_timer_limit(int limit) {
    m_timer_limit = limit;
}

void PlatformObjectData::set_movement_speed(float speed) {
    m_movement_speed = speed;
}

void PlatformObjectData::set_movement_limit(float limit) {
    m_movement_limit = limit;
}

void PlatformObjectData::set_activate_only_when_over_it(bool activate) {
    m_activate_only_when_over_it = activate;
}

void PlatformObjectData::set_animate_only_when_active(bool animate) {
    m_animate_only_when_active = animate;
}

void PlatformObjectData::set_animation_loop(bool loop) {
    m_animation_loop = loop;
}

void PlatformObjectData::set_animation_loop_in_reverse(bool reverse) {
    m_animation_loop_in_reverse = reverse;
}

void PlatformObjectData::set_door_key(int key) {
    m_door_key = key;
}
