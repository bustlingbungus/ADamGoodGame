// prevents the console from opening on application startup
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <SDL_mixer.h>

#include "engine/LTexture.hpp"
#include "engine/LTimer.hpp"
#include "engine/LWindow.hpp"
#include "engine/GameMath.hpp"
#include "engine/LAudio.hpp"

#include "game/Game.hpp"

#include "gameObject/GameObject.hpp"

#include "Menu/Menu.hpp"
#include "Menu/Button.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#define GAME_RESOLUTION 1024

int main(
    // int argc, char* args[]
)
{
  // Initialize SDL
  auto gHolder = std::make_shared<LWindow>(
    GAME_RESOLUTION, GAME_RESOLUTION, 
    "A Dam Good Game", true,
    "../../assets/Entities/Player/front/0.png"
  );

  // load the game itself
  Game game(gHolder, GAME_RESOLUTION, GAME_RESOLUTION);
  // load the menu
  GameMenu menu(gHolder, &game, GAME_RESOLUTION, GAME_RESOLUTION);

  // Event handler
  SDL_Event e;

  bool quit = false;
  bool inMenu = true;
  // While application is running
  while (!quit)
  {
    // Handle events on queue
    while (SDL_PollEvent(&e) != 0)
    {
      // User requests quit
      if (e.type == SDL_QUIT)
      {
        quit = true;
      }

      // Handle window events
      gHolder->handleEvent(e);
      if (!quit) quit = menu.handle_events(e, &inMenu);
      if (!inMenu) game.handle_events(e);
    }

    menu.update();
    game.update_deltaTime();
    game.set_paused(!inMenu);
    if (!inMenu) 
    {
      game.update_gameobjects();
      // call update_cells first
      game.update_cells();
      game.dayNightCycle();
      game.attempt_enemy_spawn();
    }

    // Only draw when not minimized
    if (!gHolder->isMinimized())
    {
      // Clear screen
      SDL_SetRenderDrawColor(gHolder->gRenderer, 0, 0, 0, 0xFF);
      SDL_RenderClear(gHolder->gRenderer);

      // Render background
      if (menu.is_inGame()) 
      {
        game.center_camera_on_player();
        game.render_background();
        game.render_gameobjects();
        game.render_overlay();
        game.render_gameobjects_under_trees();
        game.render_darkness();

        // UI rendering
        game.render_cell_health();
        game.render_dialogue();
        game.render_player_health();
        game.render_clock();
        if (!inMenu) game.render_controls();
      }

      if (inMenu) {
        menu.render_background();
        menu.render_highscores();
        menu.render_settings();
        menu.render_buttons();
        menu.render_confirmation();
        menu.render_credits();
      }
      menu.render_FPS();
      menu.render_CRT();
      menu.render_aspect_ratio();

      // Update screen
      SDL_RenderPresent(gHolder->gRenderer);
    }
  }

  return 0;
}