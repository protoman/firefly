#include "i18ntext.h"

i18nText* i18nText::_instance = nullptr;

i18nText::i18nText()
{

}

i18nText *i18nText::get_instance()
{
    if (!_instance) {
        _instance = new i18nText();
    }
    return _instance;

}

std::vector<std::string> i18nText::get_dialog(e_GAME_DIALOG number)
{
    std::vector<std::string> res;
    std::vector<std::string> *game_dialogs = &game_dialog_en;


    // if we don't have the dialog, log an error and return empty
    if (number >= game_dialogs->size()) {
        std::cout << "ERROR: Invalid dialog number[" << (int)number << "]" << std::endl;
        return res;
    }
    // if dialog is one-shot and already used, return empty
    if (one_shot_game_dialogs.find(number) != one_shot_game_dialogs.end() && used_game_dialogs.find(number) != used_game_dialogs.end()) {
        std::cout << "WARNING: dialog number[" << (int)number << "] is one-shot and already used." << std::endl;
        return res;
    }

    used_game_dialogs.insert(number);
    // split the dialog by the separator
    std::cout << "Splitting dialog[" << game_dialogs->at(number) << "]" << std::endl;
    res = split(game_dialogs->at(number), GAME_TEXT_DIALOG_SEPARATOR_CHAR);
    return res;
}

std::vector<std::string> i18nText::split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}
