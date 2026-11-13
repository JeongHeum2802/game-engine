#pragma once

#include <miniaudio.h>
#include <string>

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void Play(const std::string& filename);
    void SetVolume(float volume);

private:
    ma_engine engine{};
};
