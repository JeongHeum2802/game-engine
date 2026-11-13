#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>

AudioManager::AudioManager()
{
    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to initialize audio engine"
        );
    }

    ma_engine_set_volume(&engine, 0.7f);
}

AudioManager::~AudioManager()
{
    ma_engine_uninit(&engine);
}

void AudioManager::Play(const std::string& filename)
{
    const std::string path =
        "assets/sounds/" + filename;

    if (!std::filesystem::exists(path))
    {
        std::cerr
            << "[Audio] File not found: "
            << path << '\n';
        return;
    }

    ma_result result = ma_engine_play_sound(
        &engine,
        path.c_str(),
        nullptr
    );

    if (result != MA_SUCCESS)
    {
        std::cerr
            << "[Audio] Failed to play: "
            << path << '\n';
    }
}

void AudioManager::SetVolume(float volume)
{
    ma_engine_set_volume(&engine, volume);
}
