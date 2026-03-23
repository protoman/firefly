#ifndef RECOVERY_OBJECT_DATA_H
#define RECOVERY_OBJECT_DATA_H

#include "object_data.h"
#include "cereal/cereal.hpp"

enum class RecoveryObjectDataTypeEnum {
    E_TANK,
    W_TANK,
    HP_SMALL,
    HP_BIG,
    WPN_SMALL,
    WPN_BIG,
    SAVE_POINT
};

class RecoveryObjectData : public ObjectData {
public:
    RecoveryObjectData();
    ~RecoveryObjectData() override = default;

    // Getters
    RecoveryObjectDataTypeEnum get_type() const;

    // Setters
    void set_type(RecoveryObjectDataTypeEnum type);

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ObjectData>(this),
                CEREAL_NVP(m_type));
    }

private:
    RecoveryObjectDataTypeEnum m_type;
};

#endif // RECOVERY_OBJECT_DATA_H
