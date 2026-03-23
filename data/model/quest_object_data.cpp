#include "quest_object_data.h"

QuestObjectData::QuestObjectData()
    : ObjectData(),
      m_quest_id(0),
      m_requires_item(0),
      m_delivers_item(0) {
}

int QuestObjectData::get_quest_id() const {
    return m_quest_id;
}

int QuestObjectData::get_requires_item() const {
    return m_requires_item;
}

int QuestObjectData::get_delivers_item() const {
    return m_delivers_item;
}

void QuestObjectData::set_quest_id(int id) {
    m_quest_id = id;
}

void QuestObjectData::set_requires_item(int req_item) {
    m_requires_item = req_item;
}

void QuestObjectData::set_delivers_item(int deliv_item) {
    m_delivers_item = deliv_item;
}
