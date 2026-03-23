#ifndef UTIL_OBJECT_DATA_H
#define UTIL_OBJECT_DATA_H

#include "object_data.h"

enum class UtilObjectDataTypeEnum {
    Ability,
    Teleporter,
    Key
};

class UtilObjectData : public ObjectData {
public:
    UtilObjectData();
    ~UtilObjectData() override = default;

    // Getters
    UtilObjectDataTypeEnum get_type() const;
    float get_teleporter_destiny_x() const;
    float get_teleporter_destiny_y() const;

    // Setters
    void set_type(UtilObjectDataTypeEnum type);
    void set_teleporter_destiny_x(float x);
    void set_teleporter_destiny_y(float y);

private:
    UtilObjectDataTypeEnum m_type;
    float m_teleporter_destiny_x;
    float m_teleporter_destiny_y;
};

#endif // UTIL_OBJECT_DATA_H
