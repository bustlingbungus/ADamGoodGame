#pragma once

#include "LWindow.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

enum TextOrientation
{
  Centred,
  Left_aligned,
};

// forward declare texture manipulator class
class TextureManipulator;

// Texture wrapper class
class LTexture
{
  // declare TextureManipulator as a friend class
  friend class TextureManipulator;

public:
  // Initializes variables
  LTexture(std::shared_ptr<LWindow> gHolder);
  LTexture();

  // Deallocates memory
  ~LTexture();

  // Loads image at specified path
  bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
  // Creates image from font string
  bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font *font = NULL);
#endif


  // Deallocates texture
  void free();

  // Set color modulation
  void setColor(Uint8 red, Uint8 green, Uint8 blue);

  // Set blending
  void setBlendMode(SDL_BlendMode blending);

  // Set alpha modulation
  void setAlpha(Uint8 alpha);

  // Renders to fit the whole background
  void LTexture::renderAsBackground(SDL_Rect *clip = NULL, double angle = 0.0,
                                    SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

  // Renders texture at given point
  void render(int x, int y, SDL_Rect *dest = NULL, SDL_Rect *clip = NULL, double angle = 0.0,
              SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

  // renders texture on provided vertices
  void renderGeometry(SDL_Vertex *vertices, int n);

  // Gets image dimensions
  int getWidth();
  int getHeight();

private:
  // The actual hardware texture
  SDL_Texture *mTexture;

  std::shared_ptr<LWindow> gHolder;

  // Image dimensions
  int mWidth;
  int mHeight;
};


// renders a desired string centred on x
void renderText( std::string txt, int x, int y, std::shared_ptr<LWindow> window, SDL_Color colour = {255,255,255,255}, TTF_Font *font = NULL, TextOrientation orientation = Centred );