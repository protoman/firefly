#ifndef OBJECT_DATA_H
#define OBJECT_DATA_H

#include <string>

class ObjectData {
public:
    ObjectData();
    virtual ~ObjectData() = default;

    // Getters
    std::string get_graphic_filename() const;
    int get_graphic_width() const;
    int get_graphic_height() const;
    long get_frame_duration() const;

    // Setters
    void set_graphic_filename(const std::string& filename);
    void set_graphic_width(int width);
    void set_graphic_height(int height);
    void set_frame_duration(long duration);

protected:
    std::string m_graphic_filename;
    int m_graphic_width;
    int m_graphic_height;
    long m_frame_duration;
};

#endif // OBJECT_DATA_H
