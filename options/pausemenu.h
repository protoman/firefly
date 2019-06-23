#ifndef PAUSEMENU_H
#define PAUSEMENU_H


class PauseMenu
{
public:
    static PauseMenu* get_instance();
    bool execute_pause_menu();

private:
    PauseMenu();
    PauseMenu(PauseMenu const&){};             // copy constructor is private
    PauseMenu& operator=(PauseMenu const&){ return *this; };  // assignment operator is private


private:
    bool is_paused = false;
    static PauseMenu* _instance;
    int screen = 0;                             // current screen (change with L/R buttons)
};

#endif // PAUSEMENU_H
