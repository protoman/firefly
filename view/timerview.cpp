#include "timerview.h"

TimerView* TimerView::_instance = nullptr;

TimerView *TimerView::get_instance()
{
    if (!_instance) {
        _instance = new TimerView();
    }
    return _instance;
}

TimerView::TimerView()
{
    _paused_timer = 0;
    _ticks = 0;
    _paused_ticks = 0;
    _paused = false;
}

void TimerView::delay(int waitMS) const
{
    SDL_Delay(waitMS);
}

void TimerView::udelay(int useconds)
{
#ifndef PC
    SDL_Delay(useconds);
#else
    usleep(useconds);
#endif
}

unsigned long TimerView::getTimer() const
{
    return SDL_GetTicks() - _paused_ticks;
}

void TimerView::start_ticker()
{
    _ticks = getTimer();
}

long TimerView::get_ticks()
{
    return getTimer() - _ticks;
}

void TimerView::pause()
{
    _paused = true;
    _paused_timer = getTimer();
    //std::cout << "PAUSE - SDL_GetTicks(): " << _paused_timer << std::endl;
}

void TimerView::unpause()
{
    // unpausing without pause being called
    if (_paused == false) {
        return;
    }
    _paused = false;
    long paused_ticks = getTimer() - _paused_timer;
    //std::cout << "UNPAUSE - _paused_ticks: " << _paused_ticks << ", paused_ticks: " << paused_ticks;
    _paused_ticks += paused_ticks;
    //std::cout << ", SDL_GetTicks(): " << SDL_GetTicks() << ", timer: " << getTimer() << std::endl;
}

bool TimerView::is_paused()
{
    return _paused;
}

