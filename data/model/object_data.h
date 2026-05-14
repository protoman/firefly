#ifndef OBJECT_DATA_H
#define OBJECT_DATA_H

#include <string>
#include <vector>
#include "cereal/cereal.hpp"

enum class ObjectDataType {
    RECOVERY,
    QUEST,
    PLATFORM,
    UTIL
};

struct ObjectDataTypeInfo {
    ObjectDataType type;
    std::string name;
};

enum class RecoveryObjectDataTypeEnum {
    E_TANK,
    W_TANK,
    HP_SMALL,
    HP_BIG,
    WPN_SMALL,
    WPN_BIG,
    SAVE_POINT
};

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

enum class UtilObjectDataTypeEnum {
    Ability,
    Teleporter,
    Key
};

class ObjectData {
public:
    ObjectData();
    virtual ~ObjectData() = default;

    // Static Helpers
    [[nodiscard]] static ObjectDataTypeInfo get_type_info(ObjectDataType type);
    [[nodiscard]] static ObjectDataType get_type_from_index(int index);
    [[nodiscard]] static std::vector<ObjectDataType> get_all_types();

    // Getters
    [[nodiscard]] std::string get_name() const;
    [[nodiscard]] std::string get_graphic_filename() const;
    [[nodiscard]] int get_graphic_width() const;
    [[nodiscard]] int get_graphic_height() const;
    [[nodiscard]] long get_frame_duration() const;
    [[nodiscard]] ObjectDataType get_object_type() const;

    // Recovery Getters
    [[nodiscard]] RecoveryObjectDataTypeEnum get_recovery_type() const;

    // Platform Getters
    [[nodiscard]] PlatformObjectDataTypeEnum get_platform_type() const;
    [[nodiscard]] PlatformObjectDataMovingDirectionEnum get_moving_direction() const;
    [[nodiscard]] int get_timer_limit() const;
    [[nodiscard]] float get_movement_speed() const;
    [[nodiscard]] float get_movement_limit() const;
    [[nodiscard]] bool get_activate_only_when_over_it() const;
    [[nodiscard]] bool get_animate_only_when_active() const;
    [[nodiscard]] bool get_animation_loop() const;
    [[nodiscard]] bool get_animation_loop_in_reverse() const;
    [[nodiscard]] int get_door_key() const;

    // Quest Getters
    [[nodiscard]] int get_quest_id() const;
    [[nodiscard]] int get_requires_item() const;
    [[nodiscard]] int get_delivers_item() const;

    // Util Getters
    [[nodiscard]] UtilObjectDataTypeEnum get_util_type() const;
    [[nodiscard]] float get_teleporter_destiny_x() const;
    [[nodiscard]] float get_teleporter_destiny_y() const;

    // Setters
    void set_name(const std::string& name);
    void set_graphic_filename(const std::string& filename);
    void set_graphic_width(int width);
    void set_graphic_height(int height);
    void set_frame_duration(long duration);
    void set_object_type(ObjectDataType type);

    // Recovery Setters
    void set_recovery_type(RecoveryObjectDataTypeEnum type);

    // Platform Setters
    void set_platform_type(PlatformObjectDataTypeEnum type);
    void set_moving_direction(PlatformObjectDataMovingDirectionEnum direction);
    void set_timer_limit(int limit);
    void set_movement_speed(float speed);
    void set_movement_limit(float limit);
    void set_activate_only_when_over_it(bool activate);
    void set_animate_only_when_active(bool animate);
    void set_animation_loop(bool loop);
    void set_animation_loop_in_reverse(bool reverse);
    void set_door_key(int key);

    // Quest Setters
    void set_quest_id(int id);
    void set_requires_item(int req_item);
    void set_delivers_item(int deliv_item);

    // Util Setters
    void set_util_type(UtilObjectDataTypeEnum type);
    void set_teleporter_destiny_x(float x);
    void set_teleporter_destiny_y(float y);

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(CEREAL_NVP(m_name),
                CEREAL_NVP(m_graphic_filename),
                CEREAL_NVP(m_graphic_width),
                CEREAL_NVP(m_graphic_height),
                CEREAL_NVP(m_frame_duration),
                CEREAL_NVP(m_object_type),
                CEREAL_NVP(m_recovery_type),
                CEREAL_NVP(m_platform_type),
                CEREAL_NVP(m_moving_direction),
                CEREAL_NVP(m_timer_limit),
                CEREAL_NVP(m_movement_speed),
                CEREAL_NVP(m_movement_limit),
                CEREAL_NVP(m_activate_only_when_over_it),
                CEREAL_NVP(m_animate_only_when_active),
                CEREAL_NVP(m_animation_loop),
                CEREAL_NVP(m_animation_loop_in_reverse),
                CEREAL_NVP(m_door_key),
                CEREAL_NVP(m_quest_id),
                CEREAL_NVP(m_requires_item),
                CEREAL_NVP(m_delivers_item),
                CEREAL_NVP(m_util_type),
                CEREAL_NVP(m_teleporter_destiny_x),
                CEREAL_NVP(m_teleporter_destiny_y));
    }

protected:
    std::string m_name;
    std::string m_graphic_filename;
    int m_graphic_width;
    int m_graphic_height;
    long m_frame_duration;
    ObjectDataType m_object_type;

    // Recovery
    RecoveryObjectDataTypeEnum m_recovery_type;

    // Platform
    PlatformObjectDataTypeEnum m_platform_type;
    PlatformObjectDataMovingDirectionEnum m_moving_direction;
    int m_timer_limit;
    float m_movement_speed;
    float m_movement_limit;
    bool m_activate_only_when_over_it;
    bool m_animate_only_when_active;
    bool m_animation_loop;
    bool m_animation_loop_in_reverse;
    int m_door_key;

    // Quest
    int m_quest_id;
    int m_requires_item;
    int m_delivers_item;

    // Util
    UtilObjectDataTypeEnum m_util_type;
    float m_teleporter_destiny_x;
    float m_teleporter_destiny_y;
};

#endif // OBJECT_DATA_H
