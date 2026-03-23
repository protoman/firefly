#ifndef UTIL_OBJECT_DATA_H
#define UTIL_OBJECT_DATA_H

#include "object_data.h"
#include "cereal/cereal.hpp"

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

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ObjectData>(this),
                CEREAL_NVP(m_type),
                CEREAL_NVP(m_teleporter_destiny_x),
                CEREAL_NVP(m_teleporter_destiny_y));
    }

private:
    UtilObjectDataTypeEnum m_type;
    float m_teleporter_destiny_x;
    float m_teleporter_destiny_y;
};

#endif // UTIL_OBJECT_DATA_H
