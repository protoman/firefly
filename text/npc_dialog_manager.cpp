#include "npc_dialog_manager.h"
#include "GameManager.h"
#include "data/shareddata.h"

npcDialogManager::npcDialogManager()
{

}

int npcDialogManager::item_request_state(int npc_id, int obj_id)
{
    return SharedData::get_instance()->enemy_state_map.at(npc_id);
}

void npcDialogManager::inc_request_state(int npc_id)
{
    SharedData::get_instance()->enemy_state_map.at(npc_id)++;
}

int npcDialogManager::request_item_ud(int npc_id)
{
    return SharedData::get_instance()->enemy_list.at(npc_id).npc_requested_item_id;
}

int npcDialogManager::given_item_ud(int npc_id)
{
    return SharedData::get_instance()->enemy_list.at(npc_id).npc_given_item_id;
}
