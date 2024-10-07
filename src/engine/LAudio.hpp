#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include <string>

#define NUM_AVAILABLE_CHANNELS 8

class LAudio
{
    public:
        // initialises variables
        LAudio();

        // deallocates memory
        ~LAudio();

        // loads audio from an mp3 file
        bool loadFromFile(std::string path);

        // deallocates audio
        void free();

        // plays the audio
        void play( int Channel = -1, int loops = 0 );

        // halts the audio
        void stop();

    private:

        // the actual audio
        Mix_Chunk *chunk = NULL;

        int channel = -1;
};