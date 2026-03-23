#ifndef QUEST_OBJECT_DATA_H
#define QUEST_OBJECT_DATA_H

#include "object_data.h"
#include "cereal/cereal.hpp"

class QuestObjectData : public ObjectData {
public:
    QuestObjectData();
    ~QuestObjectData() override = default;

    // Getters
    int get_quest_id() const;
    int get_requires_item() const;
    int get_delivers_item() const;

    // Setters
    void set_quest_id(int id);
    void set_requires_item(int req_item);
    void set_delivers_item(int deliv_item);

    // Cereal serialization
    template<class Archive>
    void serialize(Archive & archive) {
        archive(cereal::base_class<ObjectData>(this),
                CEREAL_NVP(m_quest_id),
                CEREAL_NVP(m_requires_item),
                CEREAL_NVP(m_delivers_item));
    }

private:
    int m_quest_id;
    int m_requires_item;
    int m_delivers_item;
};

#endif // QUEST_OBJECT_DATA_H
