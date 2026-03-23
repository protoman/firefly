#include "util_object_data.h"

UtilObjectData::UtilObjectData()
    : ObjectData(),
      m_type(UtilObjectDataTypeEnum::Ability),
      m_teleporter_destiny_x(0.0f),
      m_teleporter_destiny_y(0.0f) {
}

UtilObjectDataTypeEnum UtilObjectData::get_type() const {
    return m_type;
}

float UtilObjectData::get_teleporter_destiny_x() const {
    return m_teleporter_destiny_x;
}

float UtilObjectData::get_teleporter_destiny_y() const {
    return m_teleporter_destiny_y;
}

void UtilObjectData::set_type(UtilObjectDataTypeEnum type) {
    m_type = type;
}

void UtilObjectData::set_teleporter_destiny_x(float x) {
    m_teleporter_destiny_x = x;
}

void UtilObjectData::set_teleporter_destiny_y(float y) {
    m_teleporter_destiny_y = y;
}
