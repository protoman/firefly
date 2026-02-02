//
// Created by iuri on 01/02/2026.
//

#ifndef FIREFLYGAME_SHAREDGAMEDATA_HPP
#define FIREFLYGAME_SHAREDGAMEDATA_HPP

namespace game_data {

    class SharedGameData {

    public:
        static SharedGameData* get_instance();

    private:
        SharedGameData();
        SharedGameData(SharedGameData const&)= default;                       // copy constructor is private
        SharedGameData& operator=(SharedGameData const&){ return *this; };    // assignment operator is private


    private:
        static SharedGameData* instance;

    };
}

#endif //FIREFLYGAME_SHAREDGAMEDATA_HPP