#include "shareddata.h"

SharedData* SharedData::_instance = nullptr;

SharedData::SharedData()
{

}

SharedData *SharedData::get_instance()
{
    if (!_instance) {
        _instance = new SharedData();
    }
    return _instance;
}

std::string SharedData::get_player_face_file()
{
    char filechr[255];
    sprintf(filechr, "player%d.png", current_player);
    return std::string(filechr);
}




