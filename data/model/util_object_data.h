#ifndef UTIL_OBJECT_DATA_H
#define UTIL_OBJECT_DATA_H

#include "object_data.h"
#include "cereal/cereal.hpp"

class UtilObjectData : public ObjectData {
public:
    UtilObjectData() : ObjectData() {
        m_object_type = ObjectDataType::UTIL;
    }
    ~UtilObjectData() override = default;

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ObjectData>(this));
    }
};

#endif // UTIL_OBJECT_DATA_H
