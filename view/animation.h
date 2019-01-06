#ifndef ANIMATION_H
#define ANIMATION_H

#include "defines.h"
#include "data/st_common.h"
#include "view/timerview.h"

enum ANIMATION_TYPES {
    ANIMATION_STATIC,
    ANIMATION_DYNAMIC,
    ANIMATION_COUNT
};

class animation
{
public:
    animation(ANIMATION_TYPES pos_type, int frames_n, const st_float_position &pos, unsigned int frame_time, unsigned int repeat_times, int direction, st_size framesize);
    ~animation();

    st_float_position get_position() const;


    bool finished() const;
    void execute();
    void set_initial_delay(int delay);
    st_rectangle getDisplayImageRegion();

private:
    const st_float_position* _ref_pos; /**< holds a pointer to the position it must follow in dynamic animation type */
    ANIMATION_TYPES _pos_type;
    st_float_position _static_pos;
    Uint8 _repeat_times; /**< number of times the whole frames will be shown */
    Uint8 _executed_times; /**< holds the number of times the whole frames were shown. if greater or equals to _repeat_times, set is_finished to true */
    int _frame_time;
    bool _finished;
    Uint8 _frames_number;
    Uint8 _direction;
    st_size _framesize;

    Uint8 _current_frame;
    int _current_frame_timer;
    Uint8 _max_repeat;
    Uint8 _repeated_times;

    int initial_timer;

};

#endif // ANIMATION_H
