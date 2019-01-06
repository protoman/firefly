#include <iostream>
#include <SDL2/SDL.h>

#if defined(LINUX) || defined(RASPBERRY)
    #include <errno.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <sys/param.h>
#elif defined(WIN32)
    #include <direct.h>
    #undef main // to build on win32
#endif

#include "data/shareddata.h"
#include "gameManager.h"


SDL_Renderer* gRenderer;

void get_filepath()
{
#ifdef WIN32
    char* buffer;
    if( (buffer = _getcwd( nullptr, 0 )) != nullptr ) {
        FILEPATH = std::string(buffer);
        FILEPATH += "/";
    }
    delete[] buffer;
#else
    char *buffer = new char[MAXPATHLEN];
    char *res = getcwd(buffer, MAXPATHLEN);

    if(buffer != nullptr){
        SharedData::get_instance()->FILEPATH = std::string(buffer);
    }
    SharedData::get_instance()->GAMEPATH = std::string("/") + SharedData::get_instance()->FILEPATH + std::string("/");
    SharedData::get_instance()->FILEPATH += "/games/FireFly/";

    delete[] buffer;
#endif

    std::cout << "get_filepath - FILEPATH:" << SharedData::get_instance()->FILEPATH << std::endl;

}

int main()
{
    get_filepath();
    gameManager::get_instance()->initHardwareLayer();

    gameManager::get_instance()->preloadGameData();
    gameManager::get_instance()->initGame();

    for (int i=0; i<100000; i++) {
        gameManager::get_instance()->show_game(true, true);
        SDL_RenderPresent(gRenderer);
        SDL_Delay(10);
    }

    std::cout << "Hello World!" << std::endl;
    return 0;
}
