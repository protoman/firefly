#ifndef SOUNDVIEW_H
#define SOUNDVIEW_H

#include <iostream>
#include <SDL3_mixer/SDL_mixer.h>

#include "data/shareddata.h"

// sound-effect list


class SoundView
{
public:
    static SoundView* get_instance();
    ~SoundView();

    void init();

    void play_sfx(Uint8 sfx);
    void play_repeated_sfx(Uint8 sfx, Uint8 loops);
    void stop_repeated_sfx();
    bool is_playing_repeated_sfx() const;
    Uint8 get_repeated_sfx_n() const;
    void play_timed_sfx(Uint8 sfx, int time);
    void load_all_sfx();
    void load_music(std::string music_file);
    void load_shared_music(std::string music_file);
    void load_boss_music(std::string music_file);
    void unload_music();
    void play_music();
    void play_music_once();
    void play_boss_music();
    void load_stage_music(std::string filename);
    void restart_music();
    void stop_music() const;
    void close_audio();
    void sound_loop();
    void disable_sound() const;
    void enable_sound();
    void update_volumes();
    void play_sfx_from_file(std::string filename, int repeat_n);
    void play_shared_sfx(std::string filename);
    void play_sfx_from_chunk(MIX_Track *chunk, int repeat_n);

    MIX_Audio *sfx_from_file(std::string filename);
    bool get_is_playing_boss_music();

    MIX_Audio *get_sfx(std::string filename);

private:
    SoundView();
    SoundView(SoundView const&){};             // copy constructor is private
    SoundView& operator=(SoundView const&){ return *this; };  // assignment operator is private
    static SoundView* _instance;


private:
    MIX_Mixer *mixer = nullptr;
    MIX_Track *sfx_track = nullptr;
    MIX_Track *repeat_track = nullptr;
    MIX_Track *music_track = nullptr;
    MIX_Track *boss_music_track = nullptr;
    MIX_Track *shared_music_track = nullptr;

    MIX_Audio *sfx_list[SFX_COUNT];
    MIX_Audio *music = nullptr;
    MIX_Audio *boss_music = nullptr;

    bool is_playing_repeat = false;
    int repeating_sfx_number = -1;
    bool is_playing_boss_music;
    std::map<std::string, MIX_Audio*> sfx_map; // map of sfx files loaded, used in projectiles to avoid loading from disk every time
    SDL_PropertiesID music_properties = SDL_CreateProperties();
    SDL_PropertiesID sfx_options = SDL_CreateProperties();
};

#endif // SOUNDVIEW_H
