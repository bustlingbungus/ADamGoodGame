#include "Menu.hpp"
#include "Button.hpp"

GameMenu::GameMenu( std::shared_ptr<LWindow> Window, Game *game, int resolutionWidth, int resolutionHeight ) 
: window(Window), game(game)
{
    // show that the menu is loading
    showLoadMessage("loading menu", 512, 512, window);

    wRect = {0, 0, resolutionWidth, resolutionHeight};
    load_assets();
    load_data();
    // create buttons
    create_buttons();

    auto continueButton = menuButtons[0];
    if (mayContinue != continueButton->is_toggled()) continueButton->swap_textures();
    continueButton->apply_settings();
}

GameMenu::~GameMenu()
{
    if (BGTexture != nullptr) BGTexture->free();
    if (CRT_Tex != nullptr) CRT_Tex->free();
    if (CRT_Base != nullptr) CRT_Base->free();
    if (aspectRatio != nullptr) aspectRatio->free();
    if (gameOverTex != nullptr) gameOverTex->free();
    if (titleTex != nullptr) titleTex->free();
    
    if (buttonSound != nullptr) buttonSound->free();
    if (arcadeBonus != nullptr) arcadeBonus->free();
    if (arcadeButton99 != nullptr) arcadeButton99->free();

    TTF_CloseFont(sevenSegment48);
    TTF_CloseFont(sevenSegment36);
    TTF_CloseFont(sevenSegment24);
    TTF_CloseFont(arcadeClassic18);
    TTF_CloseFont(arcadeClassic24);
    TTF_CloseFont(arcadeClassic36);
    TTF_CloseFont(arcadeClassic48);

    activeSlider = nullptr;
    for (int i = 0; i < menuButtons.size(); i++) menuButtons[i]->free();
    for (int i = 0; i < pauseButtons.size(); i++) pauseButtons[i]->free();
    for (int i = 0; i < confirmationButtons.size(); i++) confirmationButtons[i]->free();
    for (int i = 0; i < gameOverButtons.size(); i++) gameOverButtons[i]->free();
    for (int i = 0; i < settingsButtons.size(); i++) settingsButtons[i]->free();
    menuButtons.clear();
    pauseButtons.clear();
    settingsButtons.clear();
    confirmationButtons.clear();
    gameOverButtons.clear();
}

void GameMenu::create_buttons()
{
    confirmationText = "";
    if (currButtons == &confirmationButtons) {
        if (state == in_game) currButtons = &pauseButtons;
        else currButtons = &menuButtons;
    }

    pauseButtons.clear(); menuButtons.clear();
    gameOverButtons.clear(); settingsButtons.clear();
    confirmationButtons.clear();

    create_mainMenu_buttons();
    create_pauseMenu_buttons();
    create_settings_buttons();
}

void GameMenu::render_background()
{
    if (isActive) 
    {
        bool showMenuBG = state == main_menu || state == settings_menu || state == credits;
        auto tex = (showMenuBG)? BGTexture : pauseTex;
        tex->render(wRect.x, wRect.y, &wRect);

        if (state == game_over) {
            SDL_Rect rect = {(wRect.w-GAMEOVER_TXT_WIDTH)/2 + wRect.x, 128, GAMEOVER_TXT_WIDTH, GAMEOVER_TEX_HEIGHT};
            gameOverTex->render(rect.x, rect.y, &rect);
        } 
        else if ((state == main_menu || state == credits) && confirmationText == "") {
            SDL_Rect rect = {64 + wRect.x, 32 + wRect.y, 896, 192};
            titleTex->render(rect.x, rect.y, &rect);
            if (state == credits) {
                // std::string credits = "A Game By\nBustling Bungus\n \nWith music by\nwiredbeyongbelief\n \n \n \nThanks to the play testers:\n \npdoge, 3rd grade dropout,\nWes :), Ozi, YesISaidGaming,\nchunky monkey, and Maddy";
                // renderText(credits, 512, 256, window, {255,255,255,255}, arcadeClassic36);

                int x = 512+wRect.x, y = 256;
                renderText("A game by", x, y, window, {255,255,255,255}, arcadeClassic36);
                y += 40;
                renderText("Bustling Bungus", x, y, window, {255,255,255,255}, arcadeClassic48);
                y += 120;
                renderText("With music by", x, y, window, {255,255,255,255}, arcadeClassic36);
                y += 40;
                renderText("wiredbeyondbelief", x, y, window, {255,255,255,255}, arcadeClassic48);
                y += 160;
                renderText("Thanks to the play testers...", x, y, window, {255,255,255,255}, arcadeClassic24);
                y += 80;
                renderText(
                    "pdoge, chunky monkey,\n3rd grade dropout, Wes :),\nYesISaidGaming, Ozi,\nand Maddy",
                    x, y, window, {255,255,255,255}, arcadeClassic36
                );
            }
        }
    }
}

void GameMenu::render_settings()
{
    if (state == settings_menu)
    {
        int x = 64 + wRect.x, y = 64;

        renderText("Audio", x, y, window, {255,255,255,255}, arcadeClassic36, Left_aligned);
        y += 40;

        auto white = tEditor.createSolidColour(464, 3, 0xFFFFFFFF, window);
        SDL_Rect rect = {x, y, 464, 3};
        white->render(x, y, &rect);
        y += 8;

        renderText("Volume", x, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);
        y += 90;

        auto grey = tEditor.createSolidColour(432, 3, 0xFFFFFFB0, window);
        rect.w = 432;
        grey->render(x + 16, y, &rect);

        y += 48;
        renderText("Music volume", x, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);
        y += 90;
        grey->render(x + 16, y, &rect);

        y += 96; rect.w = 464;
        renderText("Graphics", x, y, window, {255,255,255,255}, arcadeClassic36, Left_aligned);
        y += 40;

        white->render(x, y, &rect);
        y += 48;
        renderText("Pixel Filter", x + CHECKBOX_SIDELENGTH + 16, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);

        y += CHECKBOX_SIDELENGTH + 16;
        renderText("Fullscreen", x + CHECKBOX_SIDELENGTH + 16, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);
        
        y += CHECKBOX_SIDELENGTH + 16;
        renderText("Show framerate", x + CHECKBOX_SIDELENGTH + 16, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);

        y += 96;
        renderText("Max framerate", x, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);
        
        x = 512 + wRect.x;
        renderText("Gameplay", x, y, window, {255,255,255,255}, arcadeClassic36, Left_aligned);
        y += 40;

        white->render(x, y, &rect);
        y += 48;
        renderText("Show tutorial", x + CHECKBOX_SIDELENGTH + 16, y, window, {255,255,255,255}, arcadeClassic24, Left_aligned);

        white->free();
        grey->free();
    }
}

void GameMenu::render_credits()
{
    if (state == main_menu && confirmationText == "")
    {
        std::string txt = "With music by wiredbeyondbelief!";
        int x = wRect.w/2 + wRect.x, y = wRect.h - 64;

        renderText(txt, x, y, window, {255,255,255,255}, arcadeClassic24);
    }
}

void GameMenu::render_highscores()
{
    if (state == main_menu && confirmationText == "") 
    {
        int x = 64 + wRect.x, y = 256;

        renderText("HIGHSCORES", HIGHSCORE_CENTREPOS+wRect.x, y, window, {255,0,0,255}, arcadeClassic48);
        y += 96; x += 148;

        renderText("SCORE", x, y, window, {255,0,0,255}, arcadeClassic24); x += 136;
        renderText("NIGHTS\nSURVIVED", x, y, window, {255,0,0,255}, arcadeClassic18); x += 128;
        renderText("ENEMIES\nKILLED", x, y, window, {255,0,0,255}, arcadeClassic18);

        x = 64 + wRect.x; y += 80;

        for (int i = 1; i <= num_highscores; i++)
        {
            if (set_score_name == i) 
            {
                highscores[i].render(x, y, window, {255,255,255,255}, arcadeClassic24, currChar, {255,0,0,255});
                x += 500;
                std::string str = "NEW HIGHSCORE!";
                if (currChar == 2) {
                    int i;
                    std::string name = highscores[set_score_name].name;
                    for (i = 0; i < 3; i++) {
                        if (name[i] == '_') break;
                    }
                    if (i == 3) str = "[ENTER] to confirm";
                }
                renderText(str, x, y, window, {255,0,0,255}, arcadeClassic24, Left_aligned);
                x -= 500;
            } 
            else 
            {
                highscores[i].render(x, y, window, {255,255,255,255}, arcadeClassic24);
            }
            y += 64;
        }
    }
}

void GameMenu::render_confirmation() {
    if (confirmationText != "") 
    {
        std::string txt = "Are you sure?\n" + confirmationText;
        int x = (wRect.w/2) + wRect.x, y = 65 + wRect.y;
        
        renderText(txt, x, y, window, {255,255,255,255}, arcadeClassic24);
    }
}

void GameMenu::render_buttons()
{
    // find the coordinates of the mouse
    int x, y;
    get_mousePos(&x, &y);

    if (isActive && !set_score_name) 
    {
        int n = currButtons->size();
        for (int i = 0; i < n; i++) {
            //render the button
            auto button = (*currButtons)[i]; 
            button->render( wRect.x, wRect.y );

            // indicade which button (if any) is being hovered over
            if (button->isPressed(x, y)) 
            {
                int w = button->get_width(), h = button->get_height();
                bool inactiveContinue = button == menuButtons[0] && !button->is_toggled();
                if (w == BUTTON_WIDTH && h == BUTTON_HEIGHT && !inactiveContinue)
                {
                    std::string txt = ">";
                    auto tex = std::make_unique<LTexture>(window);
                    tex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic48);
                    int X, Y;
                    button->get_pos(&X, &Y);
                    
                    int rendx = X - tex->getWidth(), rendy = Y + (h-tex->getHeight())/2;
                    tex->render(rendx + wRect.x, rendy + wRect.y);

                    txt = "<";
                    tex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic48);
                    rendx = X + w;
                    tex->render(rendx + wRect.x, rendy + wRect.y);

                    tex->free();
                }
            }
        }
    }
}

void GameMenu::render_FPS()
{
    if (settings.flags&SHOW_FPS) game->render_framerate();
}

void GameMenu::render_CRT()
{
    if (settings.flags&CRT_FILTER) CRT_Tex->render(wRect.x, wRect.y, &wRect);
}

void GameMenu::enter_game_over()
{
    mayContinue = false;
    auto continueButton = menuButtons[0];
    if (mayContinue == continueButton->is_toggled()) continueButton->swap_textures();
    isActive = true;
    currButtons = &gameOverButtons;
    state = game_over;
    confirmationText = "";
}

void GameMenu::render_aspect_ratio()
{
    int w = wRect.x, h = wRect.h;
    if (w) 
    {
        SDL_Rect rect = {0, 0, w, h};
        
        aspectRatio->render(rect.x, rect.y, &rect);
        rect.x += rect.w + wRect.w; rect.w += 32;
        aspectRatio->render(rect.x, rect.y, &rect);
    }
}

bool GameMenu::handle_events( SDL_Event &e, bool *menuActive )
{
    switch (e.type)
    {
        case SDL_KEYDOWN:

            switch (e.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    if (state == in_game) {
                        if (currButtons == &pauseButtons) isActive = !isActive;
                        confirmationText = "";
                        currButtons = &pauseButtons;
                        game->clear_input();
                    }
                    break;

                case SDLK_F11:
                {
                    // WARNING!! this will cause the game to crash if you literally
                    // hold down F11 to spam fullscreen toggle, so don't do that!
                    fullscreen = window->toggleFullscreen();

                    sizeChange++;

                    if (state == in_game || state == game_over) {
                        game->initialise_BGTexture();
                    }
                    settings.flags &= ~FULLSCREEN;
                    settings.flags |= fullscreen << 4;

                    int idx = settingsButtons.size() - 4;
                    auto checkbox = settingsButtons[idx];
                    if (fullscreen != checkbox->is_toggled()) checkbox->swap_textures();

                    break;
                }

                default:
                    if (set_score_name != 0) rename_highscore(e.key.keysym.sym);
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) activeSlider = nullptr;
            break;
        
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT && input_cooldown <= 0.0f) leftClickFunc();
        
        default:
            if (activeSlider != nullptr && state == settings_menu) {
                activeSlider->execute_function();
            }
            break;
    }

    *menuActive = isActive;

    return state == Quit;
}

void GameMenu::rename_highscore( SDL_Keycode sym )
{
    char *ch = &highscores[set_score_name].name[currChar];
    switch (sym)
    {
        case SDLK_UP:
            if (*ch == '_' || *ch == 'Z') *ch = 'A';
            else (*ch)++;
            break;

        case SDLK_DOWN:
            if (*ch == '_' || *ch == 'A') *ch = 'Z';
            else (*ch)--;
            break;

        case SDLK_BACKSPACE:
            *ch = '_';
        case SDLK_LEFT:
            currChar = max(0, currChar-1);
            break;

        case SDLK_RIGHT:
            currChar = min(2, currChar+1);
            break;

        case SDLK_RETURN:
            if (currChar < 2) currChar++;
            else {
                int i;
                std::string name = highscores[set_score_name].name;
                for (i = 0; i < 3; i++) {
                    if (name[i] == '_') break;
                }
                if (i == 3) set_score_name = currChar = 0;
            }
            break;

        default:
            if (sym >= SDLK_a && sym <= SDLK_z) {
                *ch = (int)sym - 32; 
                currChar = min(2, currChar+1);
            }
    }
}

void GameMenu::get_mousePos( int *x, int *y)
{
    SDL_GetMouseState(x, y);
    *x /= window->getScaleX(); *y /= window->getScaleY();
    *x -= wRect.x; *y -= wRect.y;
}

void GameMenu::leftClickFunc()
{
    input_cooldown = inputCooldownTime;
    if (isActive && !set_score_name) 
    {
        // find the coordinates of the mouse click
        int x, y;
        get_mousePos(&x, &y);

        if (state == settings_menu)
        {
            int idx = settingsButtons.size()-1;
            if (settingsButtons[idx]->isPressed(x, y)) activeSlider = settingsButtons[idx];
            else if (settingsButtons[idx-1]->isPressed(x, y)) activeSlider = settingsButtons[idx-1];
        }

        // check all the buttons to see if they were clicked
        for (int i = 0; i < currButtons->size(); i++) {
            std::shared_ptr<Button> b = (*currButtons)[i];
            if (b->isPressed( x, y )) {
                // when pressed, execute the button's function
                b->execute_function();
                b->play_sound();
                break;
            }
        }
    }
}

bool GameMenu::is_active() { return isActive; }

void GameMenu::update()
{
    if (game->game_over() && state == in_game) enter_game_over();

    if (state == Quit) {
        save_data();
    }

    if (sizeChange) 
    {
        if (sizeChange == 2) 
        {
            create_buttons();

            create_CRT_Texture();
            create_pause_texture();
            game->createDarknessTex();

            if (aspectRatio != nullptr) aspectRatio->free();

            if (fullscreen) 
            {
                int w = window->get_mWidth(), h = window->get_mHeight(),
                    s = wRect.w * window->getScaleX();


                    wRect.x = (w - s)/(2*window->getScaleX()); wRect.y = 0;

                    aspectRatio = tEditor.createSolidColour(w, h, 0x000000FF, window);
            }
            else
            {
                wRect.x = wRect.y = 0;
            }

            game->renderOffset = Vector2Int(wRect.x, wRect.y);
            sizeChange = 0;
        } else sizeChange++;
    }
    input_cooldown -= game->get_deltaTime();
}


int GameMenu::new_highscore()
{
    int newScore = highscores[0].highscore,
        newEnemyCount = highscores[0].mostEnemiesKilled,
        newNightCount = highscores[0].mostNightsSurvived;

    int i;
    for (i = num_highscores; i > 0; i--) 
    {
        PlayerData curr = highscores[i];

        // this used to be a huge if-else chain but i did some proofs with
        // propositional logic and made it far more elegant lol

        bool p = newScore < curr.highscore,
             q = newScore == curr.highscore,
             a = newNightCount < curr.mostNightsSurvived,
             b = newNightCount == curr.mostNightsSurvived,
             c = newEnemyCount <= curr.mostEnemiesKilled;

        bool eval = p || (q && (a || ( b && c)));

        if (eval) break;
    }
    return (i + 1)%6;
}


bool GameMenu::is_inGame() { return state == in_game || state == game_over; }