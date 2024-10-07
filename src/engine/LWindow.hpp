#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>

// SDL Init Object
class LWindow
{
public:
  // The window renderer
  SDL_Renderer *gRenderer = NULL;

  // Globally used font
  TTF_Font *gFont = NULL;

  // Handles window events
  void handleEvent(SDL_Event &e);

  // Window dimensions
  int getWidth();
  int getHeight();

  // monitor dimensions
  int get_mWidth();
  int get_mHeight();

  float getScaleX();
  float getScaleY();

  // Window focii
  bool hasMouseFocus();
  bool hasKeyboardFocus();
  bool isMinimized();

  bool toggleFullscreen();

  // Initializes variables
  LWindow(int width = 1280, int height = 720, std::string name = "Window", bool shrinkWindowToMonitorSize = false, std::string iconFilename = "");

  // Deallocates memory
  ~LWindow();

private:
  // The window we'll be rendering to
  SDL_Window *gWindow = NULL;

  // Window dimensions
  int wWidth;
  int wHeight;
  // size of the monitor (not the window)
  int mWidth;
  int mHeight;

  // scaling factor
  float scaleX = 1.0f;
  float scaleY = 1.0f;

  // Window focus
  bool wMouseFocus;
  bool wKeyboardFocus;
  bool wFullScreen;
  bool wMinimized;


  std::string windowName= "";
};