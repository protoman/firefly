#ifndef QUEST_OBJECT_DATA_H
#define QUEST_OBJECT_DATA_H

#include "object_data.h"
#include "cereal/cereal.hpp"

class QuestObjectData : public ObjectData {
public:
    QuestObjectData() : ObjectData() {
        m_object_type = ObjectDataType::QUEST;
    }
    ~QuestObjectData() override = default;

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ObjectData>(this));
    }
};

#endif // QUEST_OBJECT_DATA_H
