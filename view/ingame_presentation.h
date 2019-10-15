#ifndef INGAMEPRESENTATION_H
#define INGAMEPRESENTATION_H

#define DEFAULT_SPEED 14.0

#include <vector>
#include <string>

#include "data/st_common.h"
#include "text/i18ntext.h"

class InGamePresentation
{
public:
    static InGamePresentation* get_instance();
    void execute_ingame_presentation();
    void start_show_ready();
    void show_ready_presentation();
    bool is_showing_ready();
    std::vector<st_dialog> get_game_dialog(int number);

private:
    InGamePresentation();
    InGamePresentation(InGamePresentation const&){};             // copy constructor is private
    InGamePresentation& operator=(InGamePresentation const&){ return *this; };  // assignment operator is private

    static InGamePresentation* _instance;
    bool must_show_ready = false;
    int show_ready_state = 0;
    float pos_x = 0;
    float pos_y = 0;
    st_size ready_text_stage_number_size;
    st_size ready_text_stage_name_size;
    float speed_x = DEFAULT_SPEED;
    float speed_y = 1.0;
    float limit_x = RES_W/2;
    float limit_y = AREA_H/2;
};

#endif // INGAMEPRESENTATION_H
