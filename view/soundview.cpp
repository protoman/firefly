#include "soundview.h"

SoundView* SoundView::_instance = nullptr;

SoundView::SoundView() : _repeated_sfx_channel(-1), _repeated_sfx(-1)
{
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
    int bitrate = MIX_DEFAULT_FREQUENCY;
    int channels = 2;
    if (Mix_OpenAudio(bitrate, MIX_DEFAULT_FORMAT, channels, 4096) < 0) {
        std::cout << "Couldn't open audio. Error: " << SDL_GetError() << std::endl;
    }
    load_all_sfx();
}


void SoundView::play_sfx(Uint8 sfx) {
    //std::cout << "soundLib::play_sfx::START::VOLUME: " << (int)SharedData::get_instance()->game_config.volume_sfx << std::endl;
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        //std::cout << "soundLib::play_sfx::SOUND_DISABLED" << std::endl;
        return;
    }

    if (sfx_list[sfx] != nullptr) {
        //std::cout << "soundLib::play_sfx::PLAY" << std::endl;
        Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);
        Mix_PlayChannel(-1, sfx_list[sfx], 0);
    //} else {
        //std::cout << "soundLib::play_sfx::nullptr_SFX" << std::endl;
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
        _repeated_sfx = sfx;
        Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);
        _repeated_sfx_channel = Mix_PlayChannel(-1, sfx_list[sfx], loops);
    } else {
        std::cout << "Error: soundLib::play_sfx - null sfx\n";
    }
}

void SoundView::stop_repeated_sfx()
{
    //std::cout << ">>>>>> soundLib::stop_repeated_sfx._repeated_sfx_channel: " << _repeated_sfx_channel << std::endl;
    if (_repeated_sfx_channel == -1) {
        return;
    }
    Mix_HaltChannel(_repeated_sfx_channel);
    _repeated_sfx = -1;
    _repeated_sfx_channel = -1;
}

bool SoundView::is_playing_repeated_sfx() const
{
    if (_repeated_sfx_channel == -1) {
        return false;
    }
    return true;
}

Uint8 SoundView::get_repeated_sfx_n() const
{
    return _repeated_sfx;
}


void SoundView::play_timed_sfx(Uint8 sfx, int time) {
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }

    if (sfx_list[sfx] != nullptr) {
        Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);
        Mix_PlayChannelTimed(-1, sfx_list[sfx], -1 , time);
    }
}

// must follow the oders defined in enum SFX_LIST (types.h)
void SoundView::load_all_sfx() {
    int i = 0;
    std::string filename;

    filename = SharedData::get_instance()->FILEPATH + "sfx/npc_hit.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/npc_killed.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_hit.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_shot.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_jump.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/cursor.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/stage_selected.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/got_energy_pill.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/got_item.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/shot_reflected.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/door_open.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/got_weapon.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/teleport.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/implosion.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/player_death.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/destrin_ship.wav";
    sfx_list[i] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/charged_shot.wav";
    sfx_list[SFX_PLAYER_CHARGED_SHOT] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/charging1.wav";
    sfx_list[SFX_CHARGING1] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/charging2.wav";
    sfx_list[SFX_CHARGING2] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/big_explosion.wav";
    sfx_list[SFX_BIG_EXPLOSION] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/water_enter.wav";
    sfx_list[SFX_WATER_ENTER] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/water_leave.wav";
    sfx_list[SFX_WATER_LEAVE] = Mix_LoadWAV(filename.c_str());
    i++;


    filename = SharedData::get_instance()->FILEPATH + "sfx/disappearning_block.wav";
    sfx_list[SFX_DISAPPEARING_BLOCK] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/shoryuken_girl.wav";
    sfx_list[SFX_SHORYUKEN_GIRL] = Mix_LoadWAV(filename.c_str());
    i++;

    filename = SharedData::get_instance()->FILEPATH + "sfx/beam.wav";
    sfx_list[SFX_BEAM] = Mix_LoadWAV(filename.c_str());
    i++;


    filename = SharedData::get_instance()->FILEPATH + "sfx/recharge.wav";
    sfx_list[SFX_GOT_ENERGY_BIG] = Mix_LoadWAV(filename.c_str());
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
    music = Mix_LoadMUS(filename.c_str());
    if (!music) {
        std::cout << "Error in soundLib::load_music::Mix_LoadMUS('" << filename << "': '" << Mix_GetError() << "'\n";
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::load_music - not found[%s] ###", music_file.c_str());
#endif
    }
}

void SoundView::load_shared_music(std::string music_file)
{
    std::string filename;

    unload_music();
    filename = SharedData::get_instance()->GAMEPATH + "/shared/music/" + music_file;
    music = Mix_LoadMUS(filename.c_str());
    if (!music) {
        std::cout << "Error in soundLib::load_music::Mix_LoadMUS('" << filename << "': '" << Mix_GetError() << "'\n";
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::load_music - not found[%s] ###", music_file.c_str());
#endif
    }
}

void SoundView::load_boss_music(std::string music_file) {
    std::string filename;

    if (boss_music != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(boss_music);
        boss_music = nullptr;
    }
    filename = SharedData::get_instance()->FILEPATH + "music/" + music_file;
    //std::cout << "soundLib::load_boss_music - filename: " << filename << std::endl;
    boss_music = Mix_LoadMUS(filename.c_str());
    if (!boss_music) {
        std::cout << "Error in soundLib::load_boss_music::Mix_LoadMUS('" << filename << "': '" << Mix_GetError() << "'\n";
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::load_boss_music - not found[%s] ###", music_file.c_str());
#endif
    }
}

void SoundView::unload_music()
{
    if (music != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
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
        res = Mix_PlayMusic(music, -1);
        //std::cout << "<<<<<<<<<<<<< soundLib::play_music, res[" << res << "], error[" << Mix_GetError() << "]" << std::endl;
        if (res == -1) {
            std::cout << "<<<<<<<<<<<<< Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### Mix_PlayMusic Error[%s] ###", Mix_GetError());
#endif
        }
        //std::cout << "SOUNDLIB::play_music" << std::endl;
        Mix_VolumeMusic(SharedData::get_instance()->game_config.volume_music);
    } else {
        std::cout << ">> play_music ERROR: music is null" << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::play_music - music is nullptr ###");
#endif
    }
}

void SoundView::play_music_once()
{
    if (SharedData::get_instance()->game_config.sound_enabled == false) {
        return;
    }
    int res = -1;
    if (music) {
        res = Mix_PlayMusic(music, 1);
        if (res == -1) {
            std::cout << "<<<<<<<<<<<<< soundLib::play_music_once: " << Mix_GetError() << std::endl;
        }
        //std::cout << "SOUNDLIB::play_music" << std::endl;
        Mix_VolumeMusic(SharedData::get_instance()->game_config.volume_music);
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
        if (Mix_PlayMusic(boss_music, -1) == -1) {
            std::cout << "<<<<<<<<<<<<< Mix_PlayMusic, Error: " << Mix_GetError() << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::play_boss_music Error[%s] ###", Mix_GetError());
#endif
        }
        //std::cout << "SOUNDLIB::play_boss_music" << std::endl;
        Mix_VolumeMusic(SharedData::get_instance()->game_config.volume_music);
    } else {
        printf(">> play_boss_music ERROR: boss_music is null\n");
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::play_boss_music - music is nullptr ###");
#endif
    }
}

void SoundView::load_stage_music(std::string filename) {
    is_playing_boss_music = false;
    //std::cout << "soundLib::load_stage_music - filename: " << filename << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::load_stage_music[%s] ###", filename.c_str());
#endif
    Mix_HaltMusic();
    unload_music();
    if (filename.length() > 0) {
        load_music(filename);
    } else {
        std::cout << "soundLib::load_stage_music - music filename undefined." << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::load_stage_music - music is nullptr ###");
#endif
    }
}

void SoundView::restart_music()
{
    //std::cout << "SOUNDLIB::restart_music" << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::restart_music");
#endif
    Mix_HaltMusic();
    play_music();
}


void SoundView::stop_music() const {
    //std::cout << "SOUNDLIB::stop_music" << std::endl;
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::stop_music");
#endif
    Mix_HaltMusic();
}

void SoundView::close_audio() {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::close_audio");
#endif
    Mix_FreeMusic(music);
    Mix_CloseAudio();
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
    Mix_VolumeMusic(SharedData::get_instance()->game_config.volume_music);
    Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);
}

void SoundView::play_sfx_from_file(std::string filename, int repeat_n)
{
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::play_sfx_from_file[%s] ###", filename.c_str());
#endif
    filename = SharedData::get_instance()->FILEPATH + "sfx/" + filename;
    Mix_Chunk *sfx = Mix_LoadWAV(filename.c_str());

    if (!sfx) {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::play_sfx_from_file - error loading [%s] ###", filename.c_str());
#endif
        return;
    }

    Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);

    Mix_PlayChannel(-1, sfx, repeat_n-1);
}

void SoundView::play_shared_sfx(std::string filename)
{
    filename = SharedData::get_instance()->GAMEPATH + "shared/sfx/" + filename;
    Mix_Chunk *sfx = Mix_LoadWAV(filename.c_str());

    if (!sfx) {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::play_shared_sfx - error loading [%s] ###", filename.c_str());
#endif
        return;
    }

    Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);

    Mix_PlayChannel(-1, sfx, 0);
}

void SoundView::play_sfx_from_chunk(Mix_Chunk *chunk, int repeat_n)
{
    if (!chunk) {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::play_sfx_from_chunk - invalid chunk ###");
#endif
        return;
    }
    Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);
    Mix_PlayChannel(-1, chunk, repeat_n-1);
}

Mix_Chunk* SoundView::sfx_from_file(std::string filename)
{
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "###ROCKBOT2###", "### SOUNDLIB::sfx_from_file[%s] ###", filename.c_str());
#endif
    Mix_Volume(-1, SharedData::get_instance()->game_config.volume_sfx);
    filename = SharedData::get_instance()->FILEPATH + "sfx/" + filename;
    Mix_Chunk *sfx = Mix_LoadWAV(filename.c_str());
    return sfx;
}

bool SoundView::get_is_playing_boss_music()
{
    return is_playing_boss_music;
}

Mix_Chunk *SoundView::get_sfx(std::string filename)
{
    std::map<std::string, Mix_Chunk*>::iterator it = sfx_map.find(filename);
    if (it == sfx_map.end()) {
        Mix_Chunk* sfx = SoundView::get_instance()->sfx_from_file(filename);
        sfx_map.insert(std::pair<std::string, Mix_Chunk*>(filename, sfx));
        return sfx;
    } else {
        return it->second;
    }
}

