#ifndef QUEST_OBJECT_DATA_H
#define QUEST_OBJECT_DATA_H

#include "object_data.h"

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

private:
    int m_quest_id;
    int m_requires_item;
    int m_delivers_item;
};

#endif // QUEST_OBJECT_DATA_H
