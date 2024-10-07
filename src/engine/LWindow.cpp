#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <iostream>

#include "LWindow.hpp"

LWindow::LWindow(int width, int height, std::string name, bool shrinkWindowToMonitorSize, std::string iconFilename)
{
  // Initialize non-existant window
  gWindow = NULL;
  wMouseFocus = false;
  wKeyboardFocus = false;
  wFullScreen = false;
  wMinimized = false;
  windowName = name;


  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
  {
    printf("SDL could not initialize! %s\n", SDL_GetError());
    exit(0);
  }
  
  // now that SDL is initialised, get the monitor dimensions
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  mWidth = DM.w; mHeight = DM.h;

  if (shrinkWindowToMonitorSize)
  {
    int dh = height-128, dw = width-128;
    if (mHeight < dh) {
      scaleY = (float)mHeight / dh;
    } 
    if (mWidth < dw) {
      scaleX = (float)mWidth / dw;
    }

    if (scaleY < scaleX) {
      scaleX = scaleY;
    } else scaleY = scaleX;

    wWidth = width * scaleX;
    wHeight = width * scaleY;
  }
  else {
    wWidth = width;
    wHeight = height;
  }

  // audio setup
  int result = 0, flags = MIX_INIT_MP3;
  if (flags != (result = Mix_Init(flags))) {
    printf("could not initialise mixer (result: %d)\n", result);
    printf("Mix_Init: %s\n", Mix_GetError());
    exit(0);
  }

  // Set texture filtering to linear
  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
  {
    printf("Warning: Linear texture filtering not enabled!");
  }

  // Create window
  gWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, wWidth, wHeight,
                             SDL_WINDOW_SHOWN);
  if (gWindow == NULL)
  {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    exit(0);
  }
  else
  {
    wMouseFocus = true;
    wKeyboardFocus = true;
  }

  // Create renderer for window
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  if (gRenderer == NULL)
  {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    exit(0);
  }

  // Initialize renderer color
  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
  {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
    exit(0);
  }

  // Initialize SDL_ttf
  if (TTF_Init() == -1)
  {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    exit(0);
  }

  // Open the default font
  gFont = TTF_OpenFont("../../assets/Fonts/ARCADECLASSIC.TTF", 18);
  if (gFont == NULL)
  {
    printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    exit(0);
  }

  // apply the render scale
  SDL_RenderSetScale(gRenderer, scaleX, scaleY);
  SDL_RenderPresent(gRenderer);

  // set the window icon
  if (iconFilename != "") {
    SDL_Surface *icon = IMG_Load(iconFilename.c_str());
    if (icon != NULL) {
      SDL_SetWindowIcon(gWindow, icon);
    } else {
      printf("Failed to load window icon! SDL Error: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(icon);
  }
}

void LWindow::handleEvent(SDL_Event &e)
{
  // Window event occured
  if (e.type == SDL_WINDOWEVENT)
  {
    switch (e.window.event)
    {
    // Get new dimensions and repaint on window size change
    case SDL_WINDOWEVENT_SIZE_CHANGED:
    {
      wWidth = e.window.data1;
      wHeight = e.window.data2;
      // change scaling factor
      scaleX = (float)wWidth / 1024.0f; scaleY = (float)wHeight / 1024.0f;
      scaleX = scaleY = (scaleX < scaleY)? scaleX : scaleY;
      SDL_RenderSetScale(gRenderer, scaleX, scaleY);
      SDL_RenderPresent(gRenderer);
      break;
    }

    // Repaint on exposure
    case SDL_WINDOWEVENT_EXPOSED:
      SDL_RenderPresent(gRenderer);
      break;

    // Mouse entered window
    case SDL_WINDOWEVENT_ENTER:
      wMouseFocus = true;
      break;

    // Mouse left window
    case SDL_WINDOWEVENT_LEAVE:
      wMouseFocus = false;
      break;

    // Window has keyboard focus
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      wKeyboardFocus = true;
      break;

    // Window lost keyboard focus
    case SDL_WINDOWEVENT_FOCUS_LOST:
      wKeyboardFocus = false;
      break;

    // Window minimized
    case SDL_WINDOWEVENT_MINIMIZED:
      wMinimized = true;
      break;

    // Window maximized
    case SDL_WINDOWEVENT_MAXIMIZED:
      wMinimized = false;
      break;

    // Window restored
    case SDL_WINDOWEVENT_RESTORED:
      wMinimized = false;
      break;
    }
  }
}

bool LWindow::toggleFullscreen()
{
  if (wFullScreen)
    {
      SDL_SetWindowFullscreen(gWindow, 0);
      wFullScreen = false;
    }
    else
    {
      SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
      wFullScreen = true;
      wMinimized = false;
    }
    return wFullScreen;
}

int LWindow::getWidth()
{
  return wWidth;
}

int LWindow::getHeight()
{
  return wHeight;
}

int LWindow::get_mWidth() { return mWidth; }
int LWindow::get_mHeight() { return mHeight; }

float LWindow::getScaleX() { return scaleX; }
float LWindow::getScaleY() { return scaleY; }

bool LWindow::hasMouseFocus()
{
  return wMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
  return wKeyboardFocus;
}

bool LWindow::isMinimized()
{
  return wMinimized;
}

LWindow::~LWindow()
{
  // Destroy window
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  wMouseFocus = false;
  wKeyboardFocus = false;
  wWidth = 0;
  wHeight = 0;

  // Quit SDL subsystems
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}