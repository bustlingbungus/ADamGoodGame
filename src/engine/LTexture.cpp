#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>

#include "LTexture.hpp"

LTexture::LTexture(std::shared_ptr<LWindow> gHolder) : gHolder(gHolder)
{
  // Initialize
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}
LTexture::LTexture() {
  mTexture = NULL;
  gHolder = NULL;
  mWidth = 0;
  mHeight = 0;
}

LTexture::~LTexture()
{
  // Deallocate
  free();
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText,
                                    SDL_Color textColor, TTF_Font *font)
{
  // Get rid of preexisting texture
  free();

  if (font == NULL) font = gHolder->gFont;

  // Render text surface
  SDL_Surface *textSurface =
      TTF_RenderText_Solid(font, textureText.c_str(), textColor);
  if (textSurface != NULL)
  {
    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(gHolder->gRenderer, textSurface);
    if (mTexture == NULL)
    {
      printf("Unable to create texture from rendered text! SDL Error: %s\n",
             SDL_GetError());
    }
    else
    {
      // Get image dimensions
      mWidth = textSurface->w;
      mHeight = textSurface->h;
    }

    // Get rid of old surface
    SDL_FreeSurface(textSurface);
  }
  else
  {
    printf("Unable to render text surface! SDL_ttf Error: %s\n",
           TTF_GetError());
  }

  // Return success
  return mTexture != NULL;
}
#endif

bool LTexture::loadFromFile(std::string path)
{
  // Get rid of preexisting texture
  free();

  // The final texture
  SDL_Texture *newTexture = NULL;

  // Load image at specified path
  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
           IMG_GetError());
  }
  else
  {
    // Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE,
                    SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

    // Create texture from surface pixels
    newTexture =
        SDL_CreateTextureFromSurface(gHolder->gRenderer, loadedSurface);
    if (newTexture == NULL)
    {
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(),
             SDL_GetError());
    }
    else
    {
      // Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }

    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  // Return success
  mTexture = newTexture;
  return mTexture != NULL;
}

void LTexture::free()
{
  // Free texture if it exists
  if (mTexture != NULL)
  {
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
  // Modulate texture rgb
  SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
  // Set blending function
  SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
  // Modulate texture alpha
  SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::renderAsBackground(SDL_Rect *clip, double angle,
                                  SDL_Point *center, SDL_RendererFlip flip)
{
  // Set rendering space and render to screen
  SDL_Rect renderQuad = {0, 0, gHolder->getWidth(), gHolder->getHeight()};

  // Render to screen
  SDL_RenderCopyEx(gHolder->gRenderer, mTexture, clip, &renderQuad, angle,
                   center, flip);
}

void LTexture::render(int x, int y, SDL_Rect *dest, SDL_Rect *clip, double angle,
                      SDL_Point *center, SDL_RendererFlip flip)
{
  // Set rendering space and render to screen
  SDL_Rect renderQuad = {x, y, mWidth, mHeight};

  // Set dest rendering dimensions
  if (dest != NULL)
  {
    renderQuad.w = dest->w;
    renderQuad.h = dest->h;
  }

  // Render to screen
  SDL_RenderCopyEx(gHolder->gRenderer, mTexture, clip, &renderQuad, angle,
                   center, flip);
}

void LTexture::renderGeometry(SDL_Vertex *vertices, int n)
{
  SDL_RenderGeometry(gHolder->gRenderer, mTexture, vertices, n, NULL, -1);
}

int LTexture::getWidth() { return mWidth; }

int LTexture::getHeight() { return mHeight; }


void renderText( std::string txt, int x, int y, std::shared_ptr<LWindow> window, SDL_Color colour, TTF_Font *font, TextOrientation orientation )
{
  // vector of strings to be rendered
  std::vector<std::string> strings;

  int n = txt.size(), last = 0, i;

  for (i = 0; i < n; i++) 
  {
    if (txt[i] == '\n') {
      strings.push_back(txt.substr(last, i-last));
      last = i+1;
    }
  }
  strings.push_back(txt.substr(last, i-last));

  n = strings.size();
  for (i = 0; i < n; i++) 
  {
    if (strings[i].size() == 0) continue;
    auto rend = std::make_unique<LTexture>(window);
    if (!rend->loadFromRenderedText(strings[i], colour, font)) {
      std::cerr << "failed to load rendered text!";
    }
    int X = (orientation == Centred)? x - (rend->getWidth()/2) : x;
    rend->render(X, y);
    y += rend->getHeight()+3;
    rend->free();
  }
}