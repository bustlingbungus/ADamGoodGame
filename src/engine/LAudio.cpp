#include "LAudio.hpp"
#include <iostream>

int g_channel = 0;

LAudio::LAudio()
{
    // open that audio device for playback
    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
}

LAudio::~LAudio()
{
    free();
}

void LAudio::free()
{
    if (chunk != NULL) {
        Mix_FreeChunk(chunk);
        chunk = NULL;
    }
}

bool LAudio::loadFromFile(std::string path)
{
    // get rid of preexisting audio
    free();

    // load the audio from file
    Mix_Chunk *newChunk = Mix_LoadWAV(path.c_str());

    if (newChunk == NULL) 
    {
        printf("Unable to create audio from %s! SDL Error: %s\n", path.c_str(), Mix_GetError());
    }

    // return success
    chunk = newChunk;
    return chunk != NULL;
}

void LAudio::play( int Channel, int loops )
{
    if (Channel == -1) 
    {
        // channel 0 reserved for music
        if (g_channel == 0) g_channel++;

        // find a channel to play the audio on
        channel = g_channel;
        g_channel = (g_channel+1) % NUM_AVAILABLE_CHANNELS;
    }
    else
    {
        if (g_channel == Channel) g_channel = (g_channel+1) % NUM_AVAILABLE_CHANNELS;
        channel = Channel;
    }

    Mix_PlayChannel(channel, chunk, loops);
}

void LAudio::stop()
{
    if (channel != -1) Mix_HaltChannel(channel);
    channel = -1;
}