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
#include "GameManager.h"
#include "aux_tools/stringutils.h"

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

void detect_language() {
    std::cout << "CONFIG.LANGUAGE[" << (int)SharedData::get_instance()->current_language << "]" << std::endl;
    if (SharedData::get_instance()->current_language == LANGUAGE_AUTODETECT) {
        // try to get language from the env, if set
        if (const char* env_lang = std::getenv("LANGUAGE")) {
            std::string lang_str(env_lang);
            std::string language = "en";
            if (std::string::npos != lang_str.find(":")) {
                std::vector<std::string> lang_list = StringUtils::split(lang_str, ":");
                if (lang_list.size() > 0) {
                    language = lang_list.at(0);
                }
            } else {
                language = lang_str;
            }
            if (language == "pt_BR") {
                SharedData::get_instance()->current_language = LANGUAGE_PORTUGUESE;
            } else { // default fallback
                SharedData::get_instance()->current_language = LANGUAGE_ENGLISH;
            }
        } else { // default fallback
            SharedData::get_instance()->current_language = LANGUAGE_ENGLISH;
        }
    } else {
        SharedData::get_instance()->current_language = SharedData::get_instance()->current_language;
    }
}

int main()
{
    get_filepath();
    detect_language();
    GameManager::get_instance()->initHardwareLayer();
    GameManager::get_instance()->preloadGameData();
    //GameManager::get_instance()->introScreen();
    GameManager::get_instance()->initGame();

    while (SharedData::get_instance()->run_game) {
        GameManager::get_instance()->show_game(true, true);
        SDL_Delay(10);
    }

    SDL_Quit();
    return 0;
}
