#include "object_data.h"

ObjectData::ObjectData()
    : m_graphic_filename(""),
      m_graphic_width(0),
      m_graphic_height(0),
      m_frame_duration(0) {
}

std::string ObjectData::get_graphic_filename() const {
    return m_graphic_filename;
}

int ObjectData::get_graphic_width() const {
    return m_graphic_width;
}

int ObjectData::get_graphic_height() const {
    return m_graphic_height;
}

long ObjectData::get_frame_duration() const {
    return m_frame_duration;
}

void ObjectData::set_graphic_filename(const std::string& filename) {
    m_graphic_filename = filename;
}

void ObjectData::set_graphic_width(int width) {
    m_graphic_width = width;
}

void ObjectData::set_graphic_height(int height) {
    m_graphic_height = height;
}

void ObjectData::set_frame_duration(long duration) {
    m_frame_duration = duration;
}
