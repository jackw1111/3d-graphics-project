#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <map>
#include <thread>

#include <AL/al.h>
#include <AL/alc.h>

#include <audio/wave.h>


void list_audio_devices(const ALCchar *devices);

ALenum to_al_format(short channels, short samples);

/// @brief An AudioWindow class to play audio files
class AudioWindow {
    ALboolean enumeration;
    const ALCchar *devices;
    const ALCchar *defaultDeviceName;
    int ret;
    WaveInfo *wave;

    char *bufferData;
    ALCdevice *device;
    ALvoid *data;
    ALCcontext *context;
    ALsizei size, freq;
    ALenum format;
    ALuint buffer, source;
    ALfloat listenerOri[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    ALboolean loop = AL_FALSE;
    ALCenum error;
    ALint source_state;
  	std::map<std::string, std::pair<ALuint, ALuint>> audioCache;
    float volume = 1.0;
private:
  	static int _playAudioThread(ALuint source, int source_state);
public:
    /// @note Create a AudioWindow instance
	AudioWindow();
    /// @note Loads audio into a buffer and initialize source
    ///       provided type.
	int loadAudio();
    /// @note Plays audio buffer
    /// @param filePath the path to the .wav file
	int playAudio(std::string filePath);

    /// @note Stops audio buffer
    /// @param filePath the path to the .wav file
    int stopAudio(std::string filePath);

    /// @note change volume of audio cli
    int setVolume(std::string filePath, float value);

    /// @note Clean-up AudioWindow memory
	~AudioWindow();
};


#endif