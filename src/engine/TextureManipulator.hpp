#pragma once

#include <SDL.h>
#include <iostream>

#include "LTexture.hpp"



// a class that is able to manipulate textures
class TextureManipulator
{

    public:

        TextureManipulator();

        // renders one LTexture onto the desired region of another
        void renderTextureToTexture( std::shared_ptr<LTexture> target, std::shared_ptr<LTexture> source, SDL_Rect *targetRect );
        void renderTextureToTexture( std::shared_ptr<LTexture> target, std::shared_ptr<LTexture> source, SDL_Rect *targetRect, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE );
        void renderTextureToTexture( std::shared_ptr<LTexture> target, SDL_Texture *source, SDL_Rect *targetRect );

        // creates a greyscaled texture
        std::shared_ptr<LTexture> greyscaleTexture( std::string filename, std::shared_ptr<LWindow> window );

        // creates an empty LTexture of the specified dimensions with the desired LWindow
        std::shared_ptr<LTexture> createEmptyTexture( int width, int height, std::shared_ptr<LWindow> gHolder, SDL_TextureAccess access = SDL_TEXTUREACCESS_TARGET );
        
        std::shared_ptr<LTexture> createSolidColour( int width, int height, Uint32 colour, std::shared_ptr<LWindow> gHolder );
        std::shared_ptr<LTexture> createSolidColour( int width, int height, SDL_Color colour, std::shared_ptr<LWindow> gHolder );

        // creates some of the main menu buttons
        std::shared_ptr<LTexture> createMenuButton(std::string txt, int width, int height, std::shared_ptr<LWindow> window, TTF_Font *font, SDL_Color colour = {255,255,255,255});
        std::shared_ptr<LTexture> createSliderTexture(int width, int height, std::shared_ptr<LWindow> window, SDL_Color colour = {255,255,255,255});
        std::shared_ptr<LTexture> createFPSButton(int fps, int width, int height, std::shared_ptr<LWindow> window, TTF_Font *font, SDL_Color colour = {255,255,255,255});
};


// renders white text on a black screen to show what is being loaded
void showLoadMessage(std::string txt, int x, int y, std::shared_ptr<LWindow> window, SDL_Color bkgColour = {0,0,0,255}, SDL_Color txtColour = {255,255,255,255});


// creates an empty surface object with desired dimensions
SDL_Surface* createEmptySurface( int width, int height );
