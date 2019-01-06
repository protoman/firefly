#include "animation.h"

animation::animation(ANIMATION_TYPES pos_type, int frames_n, const st_float_position &pos, unsigned int frame_time, unsigned int repeat_times, int direction, st_size framesize) : _finished(false), _repeated_times(0)
{
    _ref_pos = nullptr;
    _pos_type = pos_type;
    if (_pos_type == ANIMATION_DYNAMIC) {
        _ref_pos = &pos;
        //std::cout << ">>>>> animation::animation [DYNAMIC] - x: " << (int)_ref_pos->x << ", y: " << (int)_ref_pos->y << std::endl;
    } else {
        _static_pos = pos;
        //std::cout << ">>>>> animation::animation [STATIC] - x: " << (int)_static_pos.x << ", y: " << (int)_static_pos.y << std::endl;
    }
    _repeat_times = repeat_times;
    _frame_time = frame_time;
    //std::cout << "[ANIMATION] - frame_time: " << frame_time << ", _frame_time: " << _frame_time << std::endl;
    _direction = direction;
    _framesize = framesize;
    _frames_number = frames_n;
    _max_repeat = repeat_times;
    _current_frame_timer = TimerView::get_instance()->getTimer() + _frame_time;
    //std::cout << ">> animation::animation - _frames_number: " << _frames_number << ", x: "<< pos.x << ", y: " << pos.y << ", surface.w: " << _surface->width << ", surface.h: " << _surface->height << std::endl;
    _current_frame = 0;
    _executed_times = 0;
    initial_timer = TimerView::get_instance()->getTimer();
}


animation::~animation()
{
}

st_float_position animation::get_position() const
{
    if (_pos_type == ANIMATION_DYNAMIC) {
        //std::cout << "*** ANIM::get_position - DYNAMIC.x: " << (int)_ref_pos->x << ", DYNAMIC.y: " << (int)_ref_pos->y << std::endl;
        return *_ref_pos;
    } else {
        //std::cout << "*** ANIM::get_position - STATIC.x: " << _static_pos.x << ", STATIC.y: " << _static_pos.y << std::endl;
        return _static_pos;
    }
}


bool animation::finished() const
{
    return _finished;
}

void animation::execute()
{
    if (TimerView::get_instance()->getTimer() < initial_timer) {
        return;
    }
    //std::cout << "simple_animation::execute - pos.x: " << (int)get_position().x << ", pos.y: " << (int)get_position().y << std::endl;
    int now_timer = TimerView::get_instance()->getTimer();
    //std::cout << ">> animation::execute - _current_frame_timer: " << (int)_current_frame_timer << ", now_timer: " << now_timer << std::endl;
    st_rectangle g_rect(_current_frame*_framesize.width, 0, _framesize.width, _framesize.height);


    if (_current_frame_timer < now_timer) {
        _current_frame++;
        _current_frame_timer = now_timer + _frame_time;
        //std::cout << "SET _current_frame_timer: " << _current_frame_timer << std::endl;
        if (_current_frame >= _frames_number) {
            _current_frame = 0;
            _repeated_times++;
            //std::cout << ">> animation::execute - _repeated_times: <<" << _repeated_times << std::endl;
            if (_repeated_times >= _max_repeat) {
                //std::cout << ">> animation::execute - FINISHED <<" << std::endl;
                _finished = true;
            }
        }
    }
}

void animation::set_initial_delay(int delay)
{
    initial_timer = TimerView::get_instance()->getTimer() + delay;
}

st_rectangle animation::getDisplayImageRegion()
{
    st_rectangle g_rect(_current_frame*_framesize.width, 0, _framesize.width, _framesize.height);
    return g_rect;
}
