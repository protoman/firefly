#include "sharedmapdata.h"

namespace map_data {

SharedMapData* SharedMapData::instance = nullptr; // Definition/Allocation

SharedMapData *SharedMapData::get_instance()
{
    if (!instance) {
        instance = new map_data::SharedMapData();
    }
    return instance;
}

SharedMapData::SharedMapData() {}
}
