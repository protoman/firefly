#ifndef RECOVERY_OBJECT_DATA_H
#define RECOVERY_OBJECT_DATA_H

#include "object_data.h"
#include "cereal/cereal.hpp"

class RecoveryObjectData : public ObjectData {
public:
    RecoveryObjectData() : ObjectData() {
        m_object_type = ObjectDataType::RECOVERY;
    }
    ~RecoveryObjectData() override = default;

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ObjectData>(this));
    }
};

#endif // RECOVERY_OBJECT_DATA_H
