#ifndef NPC_DIALOG_MANAGER_H
#define NPC_DIALOG_MANAGER_H

#include "data/st_common.h"

class npcDialogManager
{
public:
    npcDialogManager();

    int item_request_state(int npc_id, int obj_id);
    void inc_request_state(int npc_id);
    int request_item_ud(int npc_id);
    int given_item_ud(int npc_id);



};

#endif // NPC_DIALOG_MANAGER_H
