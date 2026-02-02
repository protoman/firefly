#include "textview.h"
#include "data/shareddata.h"

TextView* TextView::_instance = nullptr;

TextView::TextView()
{
}

TextView *TextView::get_instance()
{
    if (!_instance) {
        _instance = new TextView();
    }
    return _instance;
}

void TextView::init()
{
    //Initialize SDL_ttf
    if (TTF_Init() == false) {
       printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", SDL_GetError() );
       exit(-1);
    }

    // load font
    font = TTF_OpenFont("./fonts/bpmonobold.ttf", FONT_SIZE);
    outline_font = TTF_OpenFont("./fonts/bpmonobold.ttf", FONT_SIZE);
}

void TextView::renderText(int x, int y, st_color color, bool centered, std::string text)
{
    SDL_FRect text_pos={(float)x, (float)y, 0.0f, 0.0f};
    SDL_Color font_color = SDL_Color();
    font_color.r = color.r;
    font_color.g = color.g;
    font_color.b = color.b;

    if (!font) {
        printf("graphicsLib::draw_text - TTF_OpenFont: %s\n", SDL_GetError());
        exit(-1);
    }

    if (outline_font) {
        SDL_Color black = {0, 0, 0, 255};
        if (font_color.r < 80 && font_color.g < 80 && font_color.b < 80) { // dark fonts needs white outline
            black.r = 255;
            black.g = 255;
            black.b = 255;
        }
        SDL_Surface* text_outlineSF = TTF_RenderText_Blended(outline_font, text.c_str(), 0, black);

        if (text_outlineSF) {
            if (centered == true && text.size() > 0) {
                text_pos.x = RES_W/2 - text_outlineSF->w/2;
            }
            text_pos.w = text_outlineSF->w;
            text_pos.h = text_outlineSF->h;
            SDL_Texture* outlineTexture = SDL_CreateTextureFromSurface(gRenderer, text_outlineSF);
            SDL_RenderTexture(gRenderer, outlineTexture, nullptr, &text_pos);
            SDL_DestroySurface(text_outlineSF);
            SDL_DestroyTexture(outlineTexture);
        }
    }
    SDL_Surface* textSF = TTF_RenderText_Blended(font, text.c_str(), 0, font_color);

    if (textSF) {
        if (centered == true && text.size() > 0) {
            text_pos.x = RES_W/2 - textSF->w/2;
        }
        text_pos.x -= 1;
        text_pos.y -= 1;
        text_pos.w = textSF->w;
        text_pos.h = textSF->h;
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSF);
        SDL_RenderTexture(gRenderer, textTexture, nullptr, &text_pos);
        SDL_DestroySurface(textSF);
        SDL_DestroyTexture(textTexture);
    }
}

void TextView::renderText(int x, int y, std::string text)
{
    renderText(x, y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), false, text);
}

void TextView::renderCenteredText(int y, std::string text)
{
    renderText(0, y, st_color(TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE, TEXT_DEFAUL_COLOR_VALUE), true, text);
}

void TextView::renderCenteredText(int y, st_color color, std::string text)
{
    renderText(0, y, st_color(color.r, color.g, color.b), true, text);
}

st_size TextView::get_text_size(std::string text)
{
    SDL_Color black = {0, 0, 0};
    //TTF_Font *font, const char *text, size_t length, SDL_Color fg
    SDL_Surface* text_outlineSF = TTF_RenderText_Blended(outline_font, text.c_str(), 0, black);
    return st_size(text_outlineSF->w, text_outlineSF->h);
}



