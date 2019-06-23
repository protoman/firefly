#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "defines.h"
#include "data/st_common.h"

extern SDL_Renderer* gRenderer;

class TextView
{
public:
    static TextView *get_instance();
    void init();
    void renderText(int x, int y, st_color color, bool centered, std::string text);
    void renderText(int x, int y, std::string text);
    void renderCenteredText(int y, std::string text);
    void renderCenteredText(int y, st_color color, std::string text);


private:
    TextView();
    TextView(TextView const&){};             // copy constructor is private
    TextView& operator=(TextView const&){ return *this; };  // assignment operator is private


private:
    static TextView* _instance;
    TTF_Font *font;
    TTF_Font *outline_font;
};

#endif // TEXTVIEW_H
