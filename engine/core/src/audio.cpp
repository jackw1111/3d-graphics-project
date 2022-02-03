#include "audio.h"



void list_audio_devices(const ALCchar *devices)
{
    const ALCchar *device = devices, *next = devices + 1;
    size_t len = 0;

    fprintf(stdout, "Devices list:\n");
    fprintf(stdout, "----------\n");
    while (device && *device != '\0' && next && *next != '\0') {
        fprintf(stdout, "%s\n", device);
        len = strlen(device);
        device += (len + 1);
        next += (len + 2);
    }
    fprintf(stdout, "----------\n");
}
ALCenum error;
int ret;
#define TEST_ERROR(_msg)        \
    error = alGetError();       \
    if (error != AL_NO_ERROR) { \
        fprintf(stderr, _msg "\n"); \
    }

ALenum to_al_format(short channels, short samples)
{
    bool stereo = (channels > 1);

    switch (samples) {
    case 16:
        if (stereo)
            return AL_FORMAT_STEREO16;
        else
            return AL_FORMAT_MONO16;
    case 8:
        if (stereo)
            return AL_FORMAT_STEREO8;
        else
            return AL_FORMAT_MONO8;
    default:
        return -1;
    }
}



AudioWindow::AudioWindow() {


    fprintf(stdout, "Using LibAudio as audio backend\n");

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE)
        fprintf(stderr, "enumeration extension not available\n");

    list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

    defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    device = alcOpenDevice(defaultDeviceName);
    if (!device) {
        fprintf(stderr, "unable to open default device\n");
        return;
    }

    fprintf(stdout, "Device: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));

    alGetError();

    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context)) {
        fprintf(stderr, "failed to make default context\n");
        return;
    }
    TEST_ERROR("make default context");

    /* set orientation */
    alListener3f(AL_POSITION, 0, 0, 1.0f);
    TEST_ERROR("listener position");
    alListener3f(AL_VELOCITY, 0, 0, 0);
    TEST_ERROR("listener velocity");
    alListenerfv(AL_ORIENTATION, listenerOri);
    TEST_ERROR("listener orientation");

}

// Needs testing / refactoring
int AudioWindow::setVolume(std::string filePath, float value) {
    volume = value;
    std::pair<ALuint, ALuint> soundMapping = audioCache.at(filePath);

    // bind audio
    ALuint source = soundMapping.first;
    ALuint buffer = soundMapping.second;

    // play audio
    alSourcei(source, AL_BUFFER, buffer);
    TEST_ERROR("buffer binding");
    alSourcef(source, AL_GAIN, volume);

    return 0;
}

int AudioWindow::loadAudio() {
	return 0;
}

int AudioWindow::playAudio(std::string filePath) {

	// check if audio file has been cached; if not create buffer and add to cache
	if (audioCache.count(filePath) == 0) {


	    alGenSources((ALuint)1, &source);
	    TEST_ERROR("source generation");

	    alSourcef(source, AL_PITCH, 1);
	    TEST_ERROR("source pitch");
	    alSourcef(source, AL_GAIN, 0.1);
	    TEST_ERROR("source gain");
	    alSource3f(source, AL_POSITION, 0, 0, 0);
	    TEST_ERROR("source position");
	    alSource3f(source, AL_VELOCITY, 0, 0, 0);
	    TEST_ERROR("source velocity");
	    alSourcei(source, AL_LOOPING, AL_FALSE);
	    TEST_ERROR("source looping");

	    alGenBuffers(1, &buffer);
	    TEST_ERROR("buffer generation");

	    /* load data */
	    wave = WaveOpenFileForReading(filePath.c_str());
	    if (!wave) {
	        fprintf(stderr, "failed to read wave file\n");
	        return -1;
	    }

	    ret = WaveSeekFile(0, wave);
	    if (ret) {
	        fprintf(stderr, "failed to seek wave file\n");
	        return -1;
	    }

	    bufferData = (char*)malloc(wave->dataSize);
	    if (!bufferData) {
	        perror("malloc");
	        return -1;
	    }

	    ret = WaveReadFile(bufferData, wave->dataSize, wave);
	    if ((unsigned int)ret != (unsigned int)wave->dataSize) {
	        fprintf(stderr, "short read: %d, want: %d\n", ret, wave->dataSize);
	        return -1;
	    }

	    alBufferData(buffer, to_al_format(wave->channels, wave->bitsPerSample),
	            bufferData, wave->dataSize, wave->sampleRate);
	    TEST_ERROR("failed to load buffer data");

	    audioCache.insert(std::make_pair(filePath, std::make_pair(source, buffer)));
	    std::cout << "adding to cache..." << std::endl;
	    
	} else {

	}
	// retrieve cached audio
	std::pair<ALuint, ALuint> soundMapping = audioCache.at(filePath);

	// bind audio
	ALuint source = soundMapping.first;
	ALuint buffer = soundMapping.second;

	// play audio
    alSourcei(source, AL_BUFFER, buffer);
    TEST_ERROR("buffer binding");
    alSourcef(source, AL_GAIN, volume);
    alSourcePlay(source);
    TEST_ERROR("source playing");

    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    TEST_ERROR("source state get");
    //std::thread audioThread(AudioWindow::_playAudioThread, source, source_state);
    //audioThread.join();
    return 0;
}

int AudioWindow::_playAudioThread(ALuint source, int source_state) {
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        //TEST_ERROR("source state get");
    }
    return 0;
}

int AudioWindow::stopAudio(std::string filePath) {
    std::pair<ALuint, ALuint> soundMapping = audioCache.at(filePath);

    ALuint source = soundMapping.first;
    ALuint buffer = soundMapping.second;
    alSourcei(source, AL_BUFFER, buffer);
    TEST_ERROR("buffer binding");
    alSourcef(source, AL_GAIN, volume);
    alSourceStop(source);

    return 0;
}

AudioWindow::~AudioWindow() {
    /* exit context */
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}


