#include "object_data.h"

ObjectData::ObjectData()
    : m_name("Object"),
      m_graphic_filename(""),
      m_graphic_width(0),
      m_graphic_height(0),
      m_frame_duration(0),
      m_object_type(ObjectDataType::RECOVERY),
      m_recovery_type(RecoveryObjectDataTypeEnum::E_TANK),
      m_platform_type(PlatformObjectDataTypeEnum::Moving),
      m_moving_direction(PlatformObjectDataMovingDirectionEnum::Right),
      m_timer_limit(0),
      m_movement_speed(0.0f),
      m_movement_limit(0.0f),
      m_activate_only_when_over_it(false),
      m_animate_only_when_active(false),
      m_animation_loop(true),
      m_animation_loop_in_reverse(false),
      m_door_key(0),
      m_quest_id(0),
      m_requires_item(0),
      m_delivers_item(0),
      m_util_type(UtilObjectDataTypeEnum::Ability),
      m_teleporter_destiny_x(0.0f),
      m_teleporter_destiny_y(0.0f) {
}

std::string ObjectData::get_name() const {
    return m_name;
}

std::string ObjectData::get_graphic_filename() const {
    return m_graphic_filename;
}

int ObjectData::get_graphic_width() const {
    return m_graphic_width;
}

int ObjectData::get_graphic_height() const {
    return m_graphic_height;
}

long ObjectData::get_frame_duration() const {
    return m_frame_duration;
}

ObjectDataType ObjectData::get_object_type() const {
    return m_object_type;
}

// Recovery Getters
RecoveryObjectDataTypeEnum ObjectData::get_recovery_type() const {
    return m_recovery_type;
}

// Platform Getters
PlatformObjectDataTypeEnum ObjectData::get_platform_type() const {
    return m_platform_type;
}

PlatformObjectDataMovingDirectionEnum ObjectData::get_moving_direction() const {
    return m_moving_direction;
}

int ObjectData::get_timer_limit() const {
    return m_timer_limit;
}

float ObjectData::get_movement_speed() const {
    return m_movement_speed;
}

float ObjectData::get_movement_limit() const {
    return m_movement_limit;
}

bool ObjectData::get_activate_only_when_over_it() const {
    return m_activate_only_when_over_it;
}

bool ObjectData::get_animate_only_when_active() const {
    return m_animate_only_when_active;
}

bool ObjectData::get_animation_loop() const {
    return m_animation_loop;
}

bool ObjectData::get_animation_loop_in_reverse() const {
    return m_animation_loop_in_reverse;
}

int ObjectData::get_door_key() const {
    return m_door_key;
}

// Quest Getters
int ObjectData::get_quest_id() const {
    return m_quest_id;
}

int ObjectData::get_requires_item() const {
    return m_requires_item;
}

int ObjectData::get_delivers_item() const {
    return m_delivers_item;
}

// Util Getters
UtilObjectDataTypeEnum ObjectData::get_util_type() const {
    return m_util_type;
}

float ObjectData::get_teleporter_destiny_x() const {
    return m_teleporter_destiny_x;
}

float ObjectData::get_teleporter_destiny_y() const {
    return m_teleporter_destiny_y;
}


// Setters
void ObjectData::set_name(const std::string& name) {
    m_name = name;
}

void ObjectData::set_graphic_filename(const std::string& filename) {
    m_graphic_filename = filename;
}

void ObjectData::set_graphic_width(int width) {
    m_graphic_width = width;
}

void ObjectData::set_graphic_height(int height) {
    m_graphic_height = height;
}

void ObjectData::set_frame_duration(long duration) {
    m_frame_duration = duration;
}

void ObjectData::set_object_type(ObjectDataType type) {
    m_object_type = type;
}

// Recovery Setters
void ObjectData::set_recovery_type(RecoveryObjectDataTypeEnum type) {
    m_recovery_type = type;
}

// Platform Setters
void ObjectData::set_platform_type(PlatformObjectDataTypeEnum type) {
    m_platform_type = type;
}

void ObjectData::set_moving_direction(PlatformObjectDataMovingDirectionEnum direction) {
    m_moving_direction = direction;
}

void ObjectData::set_timer_limit(int limit) {
    m_timer_limit = limit;
}

void ObjectData::set_movement_speed(float speed) {
    m_movement_speed = speed;
}

void ObjectData::set_movement_limit(float limit) {
    m_movement_limit = limit;
}

void ObjectData::set_activate_only_when_over_it(bool activate) {
    m_activate_only_when_over_it = activate;
}

void ObjectData::set_animate_only_when_active(bool animate) {
    m_animate_only_when_active = animate;
}

void ObjectData::set_animation_loop(bool loop) {
    m_animation_loop = loop;
}

void ObjectData::set_animation_loop_in_reverse(bool reverse) {
    m_animation_loop_in_reverse = reverse;
}

void ObjectData::set_door_key(int key) {
    m_door_key = key;
}

// Quest Setters
void ObjectData::set_quest_id(int id) {
    m_quest_id = id;
}

void ObjectData::set_requires_item(int req_item) {
    m_requires_item = req_item;
}

void ObjectData::set_delivers_item(int deliv_item) {
    m_delivers_item = deliv_item;
}

// Util Setters
void ObjectData::set_util_type(UtilObjectDataTypeEnum type) {
    m_util_type = type;
}

void ObjectData::set_teleporter_destiny_x(float x) {
    m_teleporter_destiny_x = x;
}

void ObjectData::set_teleporter_destiny_y(float y) {
    m_teleporter_destiny_y = y;
}
