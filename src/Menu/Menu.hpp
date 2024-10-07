#pragma once

#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/TextureManipulator.hpp"
#include "../engine/LAudio.hpp"
#include "../game/Game.hpp"

#include <iostream>
#include <vector>
#include <memory>

// forwards declaration
class Button;

// different menu states
enum State
{
    main_menu,
    in_game,
    Quit,
    game_over,
    settings_menu,
    credits
};


class GameMenu
{
    friend class Button;
    friend class FPSButton;

    public:

        // initialise menu
        GameMenu( std::shared_ptr<LWindow> Window, Game *game, int resolutionWidth, int resolutionHeight );

        ~GameMenu();

        // renders the background texture
        void render_background();
        // shows all the buttons
        void render_buttons();
        void render_confirmation();
        // shows the highscores on the main menu
        void render_highscores();
        // renders the CRT overlay effect
        void render_CRT();
        // renders black bars on either side of the game
        void render_aspect_ratio();
        // renders the settings menu
        void render_settings();
        // calls the games render framerate function
        void render_FPS();
        // renders a thanks to dylan for making music :3
        void render_credits();


        // handle user input, returns true when quit is requested
        bool handle_events( SDL_Event &e, bool *menuActive );

        bool is_active();

        bool is_inGame();

        void create_buttons();
        // creates all the button objects for the main menu
        void create_mainMenu_buttons();
        // creates all the button objects for the in-game pause menu
        void create_pauseMenu_buttons();
        // creates all the button objects for the settings menu
        void create_settings_buttons();

        // updates some of the internal variables
        void update();

        void load_assets();

        // loads highscores, settings, and misc data
        void load_data();
        void save_data();

        void load_highscores();
        void save_highscores();

        void create_CRT_Texture();

        void get_mousePos( int *x, int *y );

    private:

        /* -------- CONSTANTS -------- */
        int BUTTON_WIDTH = 384, BUTTON_HEIGHT = BUTTON_WIDTH/3;
        int SLIDER_HEIGHT = 66, SLIDER_WIDTH = 16;
        int CHECKBOX_SIDELENGTH = 64;
        int GAMEOVER_TXT_WIDTH = 600, GAMEOVER_TEX_HEIGHT = GAMEOVER_TXT_WIDTH*0.55f;
        int HIGHSCORE_CENTREPOS = 300;
        float PIXEL_RENDER_SCALE = 0.68f;
        Uint8 PIXEL_FILTER_ALPHA = 30;


        std::shared_ptr<LWindow> window;
        SDL_Rect wRect;

        std::shared_ptr<LTexture> BGTexture = nullptr, gameOverTex = nullptr, 
                                  CRT_Tex = nullptr, CRT_Base = nullptr, aspectRatio = nullptr,
                                  titleTex = nullptr, pauseTex = nullptr;

        // the sound made when you click a button
        std::shared_ptr<LAudio> buttonSound = nullptr, arcadeBonus = nullptr, arcadeButton99 = nullptr;

        // fonts
        TTF_Font *sevenSegment48, *sevenSegment24, *sevenSegment36,
                 *arcadeClassic48, *arcadeClassic24, *arcadeClassic36, *arcadeClassic18;


        std::string confirmationText = "";

        std::vector<std::shared_ptr<Button>> *currButtons = &menuButtons;
        std::vector<std::shared_ptr<Button>> menuButtons, // buttons in the main menu
                                             pauseButtons, // pause menu
                                             confirmationButtons, // yes/no
                                             gameOverButtons, // play again/main menu
                                             settingsButtons, // edit game settings
                                             creditsButtons;

        bool isActive = true;
        int sizeChange = 0;
        bool fullscreen = false;
        bool mayContinue = false;
        float input_cooldown = 0.0f, inputCooldownTime = 0.1f;

        // which volume slider is currently being interacted with
        std::shared_ptr<Button> activeSlider = nullptr;


        State state = main_menu;

        Game *game = nullptr;

        TextureManipulator tEditor;

        int num_highscores;
        int set_score_name = 0;
        int currChar = 0;
        // [0] = the data struct for the current game, all other are highscores
        PlayerData highscores[6];

        // user settings
        Settings settings, prevSettings;

        // checks to see if the player clicks any buttons
        void leftClickFunc();

        void enter_game_over();

        // returns the index of the highscore the latest game was better than
        int new_highscore();

        void rename_highscore( SDL_Keycode sym );

        void create_pause_texture();
};