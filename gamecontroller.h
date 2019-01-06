#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <stdio.h>
#include <iostream>

#include "defines.h"
#include "data/shareddata.h"
#include "view/textview.h"
#include "view/imageview.h"
#include "view/soundview.h"
#include "data/st_common.h"
#include "controller/mapcontroller.h"

#include "file/file_io.h"
#include "file/fio_common.h"
#include "file/fio_scenes.h"
#include "file/fio_strings.h"

class gameController
{
public:
    gameController();
    void initHardwareLayer();
    void demo();
    void loadGameData();
    void loadMapData();

private:

    // FILE-UTILS //
    file_io fio;
    fio_scenes fio_scenes;
    fio_common fio_cmm;
    MapController mapController;
};

#endif // GAMECONTROLLER_H
