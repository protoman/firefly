#ifndef FILE_V6_QUEST_H
#define FILE_V6_QUEST_H

#include "defines.h"

#define FILE_V6_QUEST_NAME_SIZE 256
#define FILE_V6_QUEST_TEXT_SIZE 1024

// TODO - utf8 support //
// -1 due to autoselect option in enum
struct file_v6_quest {
    char name[LANGUAGE_COUNT-1][FILE_V6_QUEST_NAME_SIZE];
    char short_description[LANGUAGE_COUNT-1][FILE_V6_QUEST_NAME_SIZE];
    char received_item_text[LANGUAGE_COUNT-1][FILE_V6_QUEST_TEXT_SIZE];
    char give_item_text[LANGUAGE_COUNT-1][FILE_V6_QUEST_TEXT_SIZE];
    int requested_item_id;
    int given_item_id;

    file_v6_quest() {
        requested_item_id = -1;
        given_item_id = -1;
    }
};

#endif // FILE_V6_QUEST_H
