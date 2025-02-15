#ifndef GAMENPC_H
#define GAMENPC_H

#include "character/character.h"
#include "file/fio_strings.h"

class GameNPC : public character
{
public:
    GameNPC(int map_id, int main_id, int id); // load data from game_data and create a new npc
    bool have_fly_movement();
    void initFrames();
    void execute();
    void show();
    void npc_activate_request_item_tooltip();
    int get_id();
    std::string get_dialog(unsigned int language);

protected:
    void death();
    void build_basic_npc(int map_id, int main_id);


private:
    void addNpcSpriteFrame(int anim_type, int posX, st_imageData&spritesSurface, int delay);

private:
    st_position start_point;
    short status = 0;
    int walk_range = 120;
    st_size graphic_size;
    unsigned long npc_request_item_tooltip_timer = 0;
    std::vector<std::string> npc_dialogs;
    fio_strings f_strings;

};

#endif // GAMENPC_H
