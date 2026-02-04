//
// Created by iuri on 03/02/2026.
//

#ifndef FIREFLYGAME_TILEDMAPOBJECTLAYER_HPP
#define FIREFLYGAME_TILEDMAPOBJECTLAYER_HPP
#include <tmxlite/Map.hpp>

#include "data/st_common.h"


class TiledMapObjectLayer {
public:
    std::vector<st_float_position> init(const tmx::Map &map, unsigned int layerIndex);
};


#endif //FIREFLYGAME_TILEDMAPOBJECTLAYER_HPP