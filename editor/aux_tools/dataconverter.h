#ifndef DATACONVERTER_H
#define DATACONVERTER_H

#include "file/v0/file_game_v0.h"
#include "file/v4/file_game_v4.h"

class DataConverter
{
public:
    DataConverter();

    file_projectile_v0 projectile_v4_to_v0(file_projectilev3 data);
    file_player_v0 player_v3_1_to_v0(file_player_v3_1_1 data);
};

#endif // DATACONVERTER_H
