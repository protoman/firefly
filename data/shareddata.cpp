#include "shareddata.h"

SharedData* SharedData::_instance = nullptr;

SharedData::SharedData()
{

}

void SharedData::add_missing_area_rooms(int stage_n, int area_n)
{
    // generate data if none exists yet
    for (std::map<int, std::vector<file_v6_level_point>>::iterator it = v6_level_map.begin(); it != v6_level_map.end(); ++it) {
        if (it->first == stage_n) {
            for (int i=0; i<it->second.size(); i++) {
                file_v6_room new_room;
                new_room.stage_n = stage_n;
                new_room.area_n = area_n;
                new_room.position = st_position(it->second.at(i).x, it->second.at(i).y);
                if (v6_area_room_list.find(new_room.position) == v6_area_room_list.end()) {
                    v6_area_room_list.insert(std::pair<st_position, file_v6_room>(new_room.position, new_room));
                }

            }
        }
    }
}

void SharedData::add_missing_areas()
{
    for (unsigned int i=0; i<SharedData::get_instance()->v6_stage_list.size(); i++) {
        std::vector<file_v6_area> new_area_list;
        new_area_list.push_back(file_v6_area());

        if (SharedData::get_instance()->v6_area_map.find(i) == SharedData::get_instance()->v6_area_map.end()) {
            SharedData::get_instance()->v6_area_map.insert(std::pair<int, std::vector<file_v6_area>>(i, new_area_list));
        } else if (SharedData::get_instance()->v6_area_map.find(i)->second.size() == 0) {
            SharedData::get_instance()->v6_area_map.find(i)->second.push_back(file_v6_area());
        }
    }
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




