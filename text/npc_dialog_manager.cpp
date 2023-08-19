#include "npc_dialog_manager.h"
#include "GameManager.h"
#include "data/shareddata.h"

npcDialogManager::npcDialogManager()
{

}

int npcDialogManager::item_request_state(int npc_id, int obj_id)
{
    return GameData::get_instance()->npc_state_map.at(npc_id);
}

void npcDialogManager::inc_request_state(int npc_id)
{
    GameData::get_instance()->npc_state_map.at(npc_id)++;
}

int npcDialogManager::request_item_ud(int npc_id)
{
    return GameData::get_instance()->get_enemy(npc_id)->npc_requested_item_id;
}

int npcDialogManager::given_item_ud(int npc_id)
{
    return GameData::get_instance()->get_enemy(npc_id)->npc_given_item_id;
}
