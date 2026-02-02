#ifndef TIMERVIEW_H
#define TIMERVIEW_H

#include <SDL3/SDL.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

class TimerView
{
public:
    static TimerView* get_instance();

private:
    TimerView();
    TimerView(TimerView const&){};             // copy constructor is private
    TimerView& operator=(TimerView const&){ return *this; };  // assignment operator is private


public:
    void delay(int) const;
    void udelay(int useconds);
    unsigned long getTimer() const;
    void start_ticker(); // start counting ticks
    long get_ticks();    // used if we need a timer count
    void pause();
    void unpause();
    bool is_paused();

private:
    static TimerView* _instance;
    bool _paused;
    long _ticks;
    long _paused_ticks;
    long _paused_timer;
};

#endif // TIMERVIEW_H
