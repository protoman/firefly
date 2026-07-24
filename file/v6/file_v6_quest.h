#ifndef FILE_V6_QUEST_H
#define FILE_V6_QUEST_H

#include "defines.h"

#define FILE_V6_QUEST_NAME_SIZE 256
#define FILE_V6_QUEST_TEXT_SIZE 1024

// TODO - utf8 support //
// -1 due to autoselect option in enum
#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

struct file_v6_quest {
    char name[LANGUAGE_COUNT-1][FILE_V6_QUEST_NAME_SIZE];
    char short_description[LANGUAGE_COUNT-1][FILE_V6_QUEST_NAME_SIZE];
    char beforehand_text[LANGUAGE_COUNT-1][FILE_V6_QUEST_TEXT_SIZE];
    char received_item_text[LANGUAGE_COUNT-1][FILE_V6_QUEST_TEXT_SIZE];
    char give_item_text[LANGUAGE_COUNT-1][FILE_V6_QUEST_TEXT_SIZE];
    char afterwards_text[LANGUAGE_COUNT-1][FILE_V6_QUEST_TEXT_SIZE];
    int requested_item_id;
    int given_item_id;

    file_v6_quest() {
        requested_item_id = -1;
        given_item_id = -1;
    }

    template <class Archive>
    void save(Archive & ar) const {
        // convert fixed arrays to vector<string> for serialization
        std::vector<std::string> name_v, short_v, beforehand_v, received_v, give_v, afterwards_v;
        for (int i=0;i<LANGUAGE_COUNT-1;i++) { name_v.push_back(std::string(name[i])); short_v.push_back(std::string(short_description[i])); beforehand_v.push_back(std::string(beforehand_text[i])); received_v.push_back(std::string(received_item_text[i])); give_v.push_back(std::string(give_item_text[i])); afterwards_v.push_back(std::string(afterwards_text[i])); }
        ar(CEREAL_NVP(name_v), CEREAL_NVP(short_v), CEREAL_NVP(beforehand_v), CEREAL_NVP(received_v), CEREAL_NVP(give_v), CEREAL_NVP(afterwards_v), CEREAL_NVP(requested_item_id), CEREAL_NVP(given_item_id));
    }

    template <class Archive>
    void load(Archive & ar) {
        std::vector<std::string> name_v, short_v, beforehand_v, received_v, give_v, afterwards_v;
        ar(name_v, short_v, beforehand_v, received_v, give_v, afterwards_v, requested_item_id, given_item_id);
        for (int i=0;i<LANGUAGE_COUNT-1;i++) {
            if (i < (int)name_v.size()) strncpy(name[i], name_v[i].c_str(), FILE_V6_QUEST_NAME_SIZE);
            else name[i][0] = '\0';
            if (i < (int)short_v.size()) strncpy(short_description[i], short_v[i].c_str(), FILE_V6_QUEST_NAME_SIZE);
            else short_description[i][0] = '\0';
            if (i < (int)beforehand_v.size()) strncpy(beforehand_text[i], beforehand_v[i].c_str(), FILE_V6_QUEST_TEXT_SIZE);
            else beforehand_text[i][0] = '\0';
            if (i < (int)received_v.size()) strncpy(received_item_text[i], received_v[i].c_str(), FILE_V6_QUEST_TEXT_SIZE);
            else received_item_text[i][0] = '\0';
            if (i < (int)give_v.size()) strncpy(give_item_text[i], give_v[i].c_str(), FILE_V6_QUEST_TEXT_SIZE);
            else give_item_text[i][0] = '\0';
            if (i < (int)afterwards_v.size()) strncpy(afterwards_text[i], afterwards_v[i].c_str(), FILE_V6_QUEST_TEXT_SIZE);
            else afterwards_text[i][0] = '\0';
        }
    }
};

#endif // FILE_V6_QUEST_H
