#include "soundview.h"

SoundView* SoundView::_instance = nullptr;

SoundView::SoundView() {
    music = nullptr;
    boss_music = nullptr;
    is_playing_boss_music = false;
}

SoundView *SoundView::get_instance()
{
    if (!_instance) {
        _instance = new SoundView();
    }
    return _instance;
}

SoundView::~SoundView()
{
    close_audio();
}

void SoundView::init()
{
    if (!MIX_Init()) {
        SDL_Log("MIX_Init failed: %s", SDL_GetError());
    } else {
        SDL_Log("SDL_mixer is ready!");
    }

    int bitrate = 44100;
    int channels = 2;
    //SDL_AudioDeviceID devid, const SDL_AudioSpec *spec
    SDL_AudioSpec* audioSpec = nullptr; // Use system defaults
    //audioSpec.channels = channels;
    //audioSpec.format = SDL_AUDIO_F32;
    //audioSpec.freq = bitrate;

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, audioSpec);
    if (!mixer) {
        std::cout << "Couldn't open audio. Error: " << SDL_GetError() << std::endl;
    }
    sfx_track = MIX_CreateTrack(mixer);
    repeat_track = MIX_CreateTrack(mixer);
    music_track = MIX_CreateTrack(mixer);
    boss_music_track = MIX_CreateTrack(mixer);
    shared_music_track = MIX_CreateTrack(mixer);
    load_all_sfx();
}


void SoundView::play_sfx(Uint8 sfx) {
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }

    if (sfx_list[sfx] != nullptr) {
        if (!MIX_SetTrackAudio(sfx_track, music)) {
            SDL_Log("Failed to set track audio! SDL_mixer Error: %s\n", SDL_GetError());
        }
        //MIX_SetTrackGain(sfx_track, SharedData::get_instance()->game_config.volume_sfx);
        MIX_PlayTrack(sfx_track, sfx_options);
    }
}

void SoundView::play_repeated_sfx(Uint8 sfx, Uint8 loops) {
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }

    if (sfx_list[sfx] != nullptr) {
        if (is_playing_repeated_sfx()) {
            stop_repeated_sfx();
        }

        if (!MIX_SetTrackAudio(repeat_track, sfx_list[sfx])) {
            SDL_Log("Failed to set track audio! SDL_mixer Error: %s\n", SDL_GetError());
            return;
        }
        //MIX_SetTrackGain(repeat_track, SharedData::get_instance()->game_config.volume_sfx);
        MIX_SetTrackLoops(repeat_track, loops);
        MIX_PlayTrack(repeat_track, loops);
        repeating_sfx_number = sfx;
        is_playing_repeat = true;
    } else {
        std::cout << "Error: soundLib::play_sfx - null sfx\n";
    }
}

void SoundView::stop_repeated_sfx()
{
    //std::cout << ">>>>>> soundLib::stop_repeated_sfx._repeated_sfx_channel: " << _repeated_sfx_channel << std::endl;
    if (!repeat_track) {
        return;
    }
    MIX_StopTrack(repeat_track, 0);
    is_playing_repeat = false;
    repeating_sfx_number = -1;
}

bool SoundView::is_playing_repeated_sfx() const
{
    return is_playing_repeat;
}

Uint8 SoundView::get_repeated_sfx_n() const
{
    return repeating_sfx_number;
}


void SoundView::play_timed_sfx(Uint8 sfx, int time) {
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }

    if (sfx_list[sfx] != nullptr) {
        // TODO - implement
        //MIX_SetTrackGain(sfx_list[sfx], SharedData::get_instance()->game_config.volume_sfx);
        //Mix_PlayChannelTimed(-1, sfx_list[sfx], -1 , time);
    }
}

// must follow the oders defined in enum SFX_LIST (types.h)
void SoundView::load_all_sfx() {
    int i = 0;
    std::string filename;

    // TODO - open directory and load all waves instead of making a list here
    filename = SharedData::get_instance()->FILEPATH + "sfx/npc_hit.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/npc_killed.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_hit.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_shot.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_jump.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/cursor.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/stage_selected.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/got_energy_pill.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/got_item.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/shot_reflected.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/door_open.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/got_weapon.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/teleport.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/implosion.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_death.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/destrin_ship.wav";
    sfx_list[i] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/charged_shot.wav";
    sfx_list[SFX_PLAYER_CHARGED_SHOT] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/charging1.wav";
    sfx_list[SFX_CHARGING1] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/charging2.wav";
    sfx_list[SFX_CHARGING2] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/big_explosion.wav";
    sfx_list[SFX_BIG_EXPLOSION] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/water_enter.wav";
    sfx_list[SFX_WATER_ENTER] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/water_leave.wav";
    sfx_list[SFX_WATER_LEAVE] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;


    filename = SharedData::get_instance()->FILEPATH + "sfx/disappearning_block.wav";
    sfx_list[SFX_DISAPPEARING_BLOCK] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/shoryuken_girl.wav";
    sfx_list[SFX_SHORYUKEN_GIRL] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/beam.wav";
    sfx_list[SFX_BEAM] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;


    filename = SharedData::get_instance()->FILEPATH + "sfx/recharge.wav";
    sfx_list[SFX_GOT_ENERGY_BIG] = MIX_LoadAudio(mixer, filename.c_str(), false);
    i++;
    /*
    for (int j=0; j<i; j++) {
        Mix_VolumeChunk(sfx_list[j], MIX_MAX_VOLUME);
    }
    */


    // preload boss music
}



void SoundView::load_music(std::string music_file) {
    std::string filename;

    unload_music();
    filename = SharedData::get_instance()->FILEPATH + "music/" + music_file;

    if (music != nullptr) {
        unload_music();
    }

    music = MIX_LoadAudio(mixer, filename.c_str(), false);
    if (!music) {
        std::cout << "Error in soundLib::load_music::Mix_LoadMUS('" << filename << "': '" << SDL_GetError() << "'\n";
        return;
    }
    if (!MIX_SetTrackAudio(music_track, music)) {
        SDL_Log("Failed to set track audio! SDL_mixer Error: %s\n", SDL_GetError());
    }
}

void SoundView::load_shared_music(std::string music_file)
{
    std::string filename;

    unload_music();
    filename = SharedData::get_instance()->GAMEPATH + "/shared/music/" + music_file;
    music = MIX_LoadAudio(nullptr, filename.c_str(), false);
    if (!music) {
        std::cout << "Error in soundLib::load_music::Mix_LoadMUS('" << filename << "': '" << SDL_GetError() << "'\n";
    } else {
        if (!MIX_SetTrackAudio(shared_music_track, music)) {
            SDL_Log("Failed to set track audio! SDL_mixer Error: %s\n", SDL_GetError());
        }
    }
}

void SoundView::load_boss_music(std::string music_file) {
    std::string filename;

    if (boss_music != nullptr) {
        MIX_StopTrack(music_track, 0);
        MIX_DestroyAudio(boss_music);
        boss_music = nullptr;
    }
    filename = SharedData::get_instance()->FILEPATH + "music/" + music_file;
    //std::cout << "soundLib::load_boss_music - filename: " << filename << std::endl;
    boss_music = MIX_LoadAudio(mixer, filename.c_str(), false);
    if (!boss_music) {
        std::cout << "Error in soundLib::load_boss_music::Mix_LoadMUS('" << filename << "': '" << SDL_GetError() << "'\n";
    } else {
        if (!MIX_SetTrackAudio(boss_music_track, music)) {
            SDL_Log("Failed to set track audio! SDL_mixer Error: %s\n", SDL_GetError());
        }
    }
}

void SoundView::unload_music()
{
    if (music_track != nullptr) {
        MIX_StopTrack(music_track, 0);
        MIX_DestroyTrack(music_track);
        music_track = nullptr;
    }
    if (music != nullptr) {
        MIX_DestroyAudio(music);
        music = nullptr;
    }
    is_playing_boss_music = false;
}



void SoundView::play_music() {

    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }
    int res = -1;
    // toca a música
    if (music) {
        res = MIX_PlayTrack(music_track, music_properties);
        //std::cout << "<<<<<<<<<<<<< soundLib::play_music, res[" << res << "], error[" << SDL_GetError() << "]" << std::endl;
        if (res == -1) {
            std::cout << "<<<<<<<<<<<<< Mix_PlayMusic Error: " << SDL_GetError() << std::endl;
        }
        //std::cout << "SOUNDLIB::play_music" << std::endl;
        //MIX_SetTrackGain(music_track, SharedData::get_instance()->game_config.volume_music);
    } else {
        std::cout << ">> play_music ERROR: music is null" << std::endl;
    }
}

void SoundView::play_music_once()
{
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }
    int res = -1;
    if (music) {
        res = MIX_PlayTrack(music_track, music_properties);
        if (res == -1) {
            std::cout << "<<<<<<<<<<<<< soundLib::play_music_once: " << SDL_GetError() << std::endl;
        }
        //std::cout << "SOUNDLIB::play_music" << std::endl;
        //MIX_SetTrackGain(music_track, SharedData::get_instance()->game_config.volume_music);
    } else {
        std::cout << ">> soundLib::play_music_once: music is null" << std::endl;
    }
}

void SoundView::play_boss_music() {
    is_playing_boss_music = true;
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }
    // toca a música
    if (boss_music) {
        if (!MIX_PlayTrack(music_track, music_properties)) {
            std::cout << "<<<<<<<<<<<<< Mix_PlayMusic, Error: " << SDL_GetError() << std::endl;
        }
        //std::cout << "SOUNDLIB::play_boss_music" << std::endl;
        //MIX_SetTrackGain(music_track, SharedData::get_instance()->game_config.volume_music);
    } else {
        printf(">> play_boss_music ERROR: boss_music is null\n");
    }
}

void SoundView::load_stage_music(std::string filename) {
    is_playing_boss_music = false;
    //std::cout << "soundLib::load_stage_music - filename: " << filename << std::endl;
    MIX_StopTrack(music_track, 0);
    unload_music();
    if (filename.length() > 0) {
        load_music(filename);
    } else {
        std::cout << "soundLib::load_stage_music - music filename undefined." << std::endl;
    }
}

void SoundView::restart_music()
{
    //std::cout << "SOUNDLIB::restart_music" << std::endl;
    MIX_StopTrack(music_track, 0);
    play_music();
}


void SoundView::stop_music() const {
    //std::cout << "SOUNDLIB::stop_music" << std::endl;
    MIX_StopTrack(music_track, 0);
}

void SoundView::close_audio() {
    MIX_DestroyAudio(music);
    MIX_DestroyMixer(mixer);
}

void sound_loop() {}

void SoundView::disable_sound() const
{
    SharedData::get_instance()->game_config.sound_enabled = false;
    stop_music();
}

void SoundView::enable_sound()
{
    SharedData::get_instance()->game_config.sound_enabled = true;
    play_music();
}

void SoundView::update_volumes()
{
    //MIX_SetTrackGain(music_track, SharedData::get_instance()->game_config.volume_music);
    //MIX_SetTrackGain(sfx_track, SharedData::get_instance()->game_config.volume_sfx);
}

void SoundView::play_sfx_from_file(std::string filename, int repeat_n)
{
    filename = SharedData::get_instance()->FILEPATH + "sfx/" + filename;
    MIX_Audio *sfx = MIX_LoadAudio(mixer, filename.c_str(), false);

    if (!sfx) {
        return;
    }

    if (!MIX_SetTrackAudio(sfx_track, sfx)) {
        SDL_Log("Failed to set track audio! SDL_mixer Error: %s\n", SDL_GetError());
    }
    //MIX_SetTrackGain(sfx_track, SharedData::get_instance()->game_config.volume_sfx);
    MIX_PlayTrack(sfx_track, sfx_options);
}

void SoundView::play_shared_sfx(std::string filename)
{
    filename = SharedData::get_instance()->GAMEPATH + "shared/sfx/" + filename;
    MIX_Audio *sfx = MIX_LoadAudio(mixer, filename.c_str(), false);

    if (!sfx) {
        return;
    }

    if (!MIX_SetTrackAudio(sfx_track, sfx)) {
        SDL_Log("Failed to set track audio! SDL_mixer Error: %s\n", SDL_GetError());
    }
    //MIX_SetTrackGain(sfx_track, SharedData::get_instance()->game_config.volume_sfx);
    MIX_PlayTrack(sfx_track, sfx_options);
}

void SoundView::play_sfx_from_chunk(MIX_Track *chunk, int repeat_n)
{
    if (!chunk) {
        return;
    }
    //MIX_SetTrackGain(chunk, SharedData::get_instance()->game_config.volume_sfx);
    MIX_PlayTrack(chunk, sfx_options);
}

MIX_Audio *SoundView::sfx_from_file(std::string filename)
{
    filename = SharedData::get_instance()->FILEPATH + "sfx/" + filename;
    MIX_Audio *sfx = MIX_LoadAudio(mixer, filename.c_str(), false);
    //MIX_SetTrackGain(sfx, SharedData::get_instance()->game_config.volume_sfx);
    return sfx;
}

bool SoundView::get_is_playing_boss_music()
{
    return is_playing_boss_music;
}

MIX_Audio *SoundView::get_sfx(std::string filename)
{
    std::map<std::string, MIX_Audio*>::iterator it = sfx_map.find(filename);
    if (it == sfx_map.end()) {
        MIX_Audio* sfx = SoundView::get_instance()->sfx_from_file(filename);
        sfx_map.insert(std::pair<std::string, MIX_Audio*>(filename, sfx));
        return sfx;
    } else {
        return it->second;
    }
}

