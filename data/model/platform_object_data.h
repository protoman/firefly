#ifndef PLATFORM_OBJECT_DATA_H
#define PLATFORM_OBJECT_DATA_H

#include "object_data.h"
#include "cereal/cereal.hpp"

class PlatformObjectData : public ObjectData {
public:
    PlatformObjectData() : ObjectData() {
        m_object_type = ObjectDataType::PLATFORM;
    }
    ~PlatformObjectData() override = default;

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ObjectData>(this));
    }
};

#endif // PLATFORM_OBJECT_DATA_H
