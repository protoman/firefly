#include "gamecontroller.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

extern SDL_Renderer* gRenderer;

gameController::gameController()
{
    initHardwareLayer();
    loadGameData();
    loadMapData();
    demo();
}

void gameController::initHardwareLayer()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error[" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }
    SharedData::get_instance()->window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, RES_W, RES_H, SDL_WINDOW_SHOWN );
    if (SharedData::get_instance()->window == nullptr) {
        std::cout << "Window could not be created! SDL_Error[" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }

    gRenderer = SDL_CreateRenderer(SharedData::get_instance()->window, -1, SDL_RENDERER_ACCELERATED );
    if (gRenderer == nullptr) {
        std::cout << "Renderer could not be created! SDL Error" << SDL_GetError() << "]" << std::endl;
        exit(EXIT_FAILURE);
    }


    ImageView::get_instance()->init();
    TextView::get_instance()->init();
    SoundView::get_instance()->init();

    //Get window surface
    SharedData::get_instance()->screenSurface = SDL_GetWindowSurface(SharedData::get_instance()->window);
    SDL_UpdateWindowSurface(SharedData::get_instance()->window);


}

void gameController::demo()
{
    SDL_SetRenderDrawColor(gRenderer, 0xBB, 0xBB, 0xBB, 0xFF );
    SDL_RenderClear(gRenderer );

    TextView::get_instance()->renderText(100, 100, st_color(120, 0, 0), false, std::string("Hello World!"));

    st_imageData imageData = ImageView::get_instance()->imageFromFile(std::string("games/FireFly/images/logo.png"));
    ImageView::get_instance()->renderTexturePortionAt(0, 0, imageData.surface->w, imageData.surface->h, 400, 400, imageData.texture);

    mapController.loadMap();

    for (int i=0; i<1600; i++) {
        mapController.show();
        SDL_RenderPresent(gRenderer);
        SDL_Delay(10);
    }

    //Update the surface

    SoundView::get_instance()->load_music(std::string("rockbot_chaos_city.mod"));
    SoundView::get_instance()->play_music();

    SDL_Delay(4000);
}


void gameController::loadGameData()
{
    fio.read_game(SharedData::get_instance()->game_data);
    SharedData::get_instance()->enemy_list = fio_cmm.load_from_disk<file_npc_v3_1_2>("game_enemy_list_3_1_2.dat");
    if (SharedData::get_instance()->enemy_list.size() == 0) {
        SharedData::get_instance()->enemy_list.push_back(file_npc_v3_1_2());
    }

    SharedData::get_instance()->object_list = fio_cmm.load_from_disk<file_object>("game_object_list.dat");
    if (SharedData::get_instance()->object_list.size() == 0) { // add one first item to avoid errors
        SharedData::get_instance()->object_list.push_back(file_object());
    }

    SharedData::get_instance()->ai_list = fio_cmm.load_from_disk<file_artificial_inteligence>("game_ai_list.dat");
    //std::cout << "MEDIATOR::load_game::ai_list.size(): " << ai_list.size() << std::endl;
    if (SharedData::get_instance()->ai_list.size() == 0) { // add one first item to avoid errors
        for (int i=0; i<SharedData::get_instance()->enemy_list.size(); i++) {
            SharedData::get_instance()->ai_list.push_back(file_artificial_inteligence());
        }
    }


    SharedData::get_instance()->projectile_list_v3 = fio_cmm.load_from_disk<file_projectilev3>(SharedData::get_instance()->FILEPATH+PROJECTILE_FILE_V3);
    if (SharedData::get_instance()->projectile_list_v3.size() == 0) {
        SharedData::get_instance()->projectile_list_v3.push_back(file_projectilev3());
    }
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@ projectile_list_v3.size[" << SharedData::get_instance()->projectile_list_v3.size() << "]" << std::endl;


}

void gameController::loadMapData()
{
    // FILE V5 //
    SharedData::get_instance()->file_v5_map_header_list = fio_cmm.load_from_disk<file_v5_map_header>(SharedData::get_instance()->FILEPATH+FILE_V5_MAP_HEADER_LIST);
    SharedData::get_instance()->file_v5_map_link_list = fio_cmm.load_from_disk<file_v5_map_link>(SharedData::get_instance()->FILEPATH+FILE_V5_MAP_LINK_LIST);

    for (int i=0; i<SharedData::get_instance()->file_v5_map_header_list.size(); i++) {
        SharedData::get_instance()->file_v5_map_tile_map.insert(std::pair<int, std::vector<file_v5_map_tile>>(i, std::vector<file_v5_map_tile>()));
        char mapName[FS_CHAR_FILENAME_SIZE];
        sprintf(mapName, "/data/v5_map_%d_tiles.dat", i);
        SharedData::get_instance()->file_v5_map_tile_map.at(i) = fio_cmm.load_from_disk<file_v5_map_tile>(SharedData::get_instance()->FILEPATH+std::string(mapName));
    }
}
