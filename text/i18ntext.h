#ifndef I18NTEXT_H
#define I18NTEXT_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include "defines.h"

enum e_IN_GAME_TEXT {

};



class i18nText
{
public:
    static i18nText* get_instance();

    std::vector<std::string> get_dialog(e_GAME_DIALOG number);
    std::vector<std::string> split(const std::string &s, char delim);
    void consume_dialog(e_GAME_DIALOG number);                          // some dialogs needs to be shown only once

private:
    i18nText();
    i18nText(i18nText const&){};             // copy constructor is private
    i18nText& operator=(i18nText const&){ return *this; };  // assignment operator is private


private:
    static i18nText* _instance;
    std::set<e_GAME_DIALOG> one_shot_game_dialogs = {                // these dialogs will be shown just once
        GAME_DIALOG_INTRO_STAGE_SWAMP_ARRIVAL
    };






    std::vector<std::string> game_dialog_en = {
        "Here we are... somewhere in the universe.#Huh.. guys?#Where are you?#Could be that they were sent so some other place?#Looks like first mission is to find my friends!",
        "This is a test"
    };

    std::vector<std::string> game_dialog_ptBR = {
        "Aqui estamos.. em algum lugar do universo.#Hã.. Pessoal?#Onde estão vocês?#Será que eles foram parar em outro lugar?#Parece que minha primeira missão é;encontrar meus amigos!",
        "Isto é um teste"
    };


};

#endif // I18NTEXT_H
