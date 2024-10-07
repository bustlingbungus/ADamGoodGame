#include "Menu.hpp"
#include "Button.hpp"

void GameMenu::load_data()
{
    // load highscores
    load_highscores();

    // load settings
    std::string filename = "../../saves/data/UserSettings.txt";
    std::ifstream file(filename);

    if (!file) std::cerr << "Couldn't open " << filename <<'\n';
    else 
    {
        showLoadMessage("loading user settings", 512, 512, window);
        settings.loadFromFile(&file);
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        iss >> mayContinue;
        file.close();
    }

    // commit loaded settings
    Mix_Volume(-1, settings.volume);
    Mix_Volume(0, ((float)settings.volume/MIX_MAX_VOLUME)*settings.musicVolume);

    if (settings.flags & FULLSCREEN) {
        fullscreen = window->toggleFullscreen();
        sizeChange++;
    }

    game->set_max_framerate(settings.max_framrate);
}

void GameMenu::load_highscores()
{
    std::string filename = "../../saves/data/High_Scores.txt";
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Couldn't open " << filename << std::endl;

    } else {
        std::string line;
        std::getline(file, line);

        std::istringstream iss(line);

        showLoadMessage("loading highscores (1/6)", 512, 512, window);
        PlayerData currGame;
        currGame.loadFromIss( &iss );
        highscores[0] = currGame;

        std::getline(file, line);
        iss = std::istringstream(line);
        iss >> num_highscores;

        for (int i = 1; i <= num_highscores; i++)
        {
            showLoadMessage("loading highscores ("+std::to_string(i+1)+"/6", 512, 512, window);
            std::getline(file, line);
            iss = std::istringstream(line);
            PlayerData data;
            data.loadFromIss(&iss);
            highscores[i] = data;
        }
    }
}

void GameMenu::save_highscores()
{
    std::fstream file;
    std::string filename = "../../saves/data/High_Scores.txt";
    file.open(filename, std::ios::out);

    if (!file) 
    {
        std::cerr << "Failed to save to " << filename << std::endl;
    } 
    else 
    {
        highscores[0].Save(&file);

        file <<std::dec<< num_highscores << '\n';

        for (int i = 1; i <= num_highscores; i++) {
            highscores[i].Save(&file);
        }

        file.close();
    }
}

void GameMenu::save_data()
{
    save_highscores();

    std::string filename = "../../saves/data/UserSettings.txt";
    std::fstream file;
    file.open(filename, std::ios::out);

    if (!file) {
        std::cerr << "Failed to save to " << filename <<'\n';
    } else {
        settings.Save(&file);
        file << mayContinue;
        file.close();
    }

}

void GameMenu::create_mainMenu_buttons()
{
    showLoadMessage("creating buttons (1/6)", 512, 512, window);
    auto texture = tEditor.createMenuButton("CONTINUE", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    auto greyContinue = tEditor.createMenuButton("CONTINUE", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48, {100,100,100,255});
    
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 256;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    auto button = std::make_shared<Button>(this, rect, greyContinue, &Button::continue_game, arcadeBonus, texture);
    menuButtons.push_back(button);

    showLoadMessage("creating buttons (2/6)", 512, 512, window);
    auto newGameTexture = tEditor.createMenuButton("NEW GAME", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::new_game_confirmation, arcadeButton99);
    menuButtons.push_back(button);

    showLoadMessage("creating buttons (3/6)", 512, 512, window);
    texture = tEditor.createMenuButton("SETTINGS", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_settings, arcadeButton99);
    menuButtons.push_back(button);

    showLoadMessage("creating buttons (4/6)", 512, 512, window);
    texture = tEditor.createMenuButton("EXIT", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, texture, &Button::exit_to_desktop, arcadeButton99);
    menuButtons.push_back(button);


    showLoadMessage("creating buttons (5/6)", 512, 512, window);
    rect.x = HIGHSCORE_CENTREPOS - (BUTTON_WIDTH/2);
    texture = tEditor.createMenuButton("RESET", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_highscores_confirmation, arcadeButton99);
    menuButtons.push_back(button);

    showLoadMessage("creating buttons (6/6)", 512, 512, window);
    rect = {(wRect.w-BUTTON_WIDTH)/2, wRect.h/2, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, newGameTexture, &Button::load_new_game, arcadeBonus);
    gameOverButtons.push_back(button);
}

void GameMenu::create_pauseMenu_buttons()
{
    SDL_Rect rect = {(wRect.w-BUTTON_WIDTH)/2, 128, BUTTON_WIDTH, BUTTON_HEIGHT};

    showLoadMessage("creating buttons (1/6)", 512, 512, window);
    auto texture = tEditor.createMenuButton("RESUME", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    auto button = std::make_shared<Button>(this, rect, texture, &Button::close_pause_menu, arcadeBonus);
    pauseButtons.push_back(button);

    showLoadMessage("creating buttons (2/6)", 512, 512, window);
    rect.y += 175;
    texture = tEditor.createMenuButton("SETTINGS", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_settings_from_pause, arcadeButton99);
    pauseButtons.push_back(button);

    showLoadMessage("creating buttons (3/6)", 512, 512, window);
    auto MenuTexture = tEditor.createMenuButton("MAIN MENU", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic36);
    rect.y += 175;
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::exit_to_menu_confirmation, arcadeBonus);
    pauseButtons.push_back(button);


    showLoadMessage("creating buttons (4/6)", 512, 512, window);
    rect.y = 256;
    texture = tEditor.createMenuButton("YES", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::doNothing, arcadeBonus);
    confirmationButtons.push_back(button);

    showLoadMessage("creating buttons (5/6)", 512, 512, window);
    rect.y += 175;
    texture = tEditor.createMenuButton("NO", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::doNothing, arcadeButton99);
    confirmationButtons.push_back(button);

    showLoadMessage("creating buttons (6/6)", 512, 512, window);
    rect = {(wRect.w-BUTTON_WIDTH)/2, (wRect.h/2)+175, BUTTON_WIDTH, BUTTON_HEIGHT};
    button = std::make_shared<Button>(this, rect, MenuTexture, &Button::go_to_main_menu_from_gameover, arcadeButton99);
    gameOverButtons.push_back(button);
}

void GameMenu::create_settings_buttons()
{
    int x = wRect.w - (9 * BUTTON_WIDTH/8), y = 64;
    SDL_Rect rect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};

    showLoadMessage("creating buttons (1/15)", 512, 512, window);
    auto texture = tEditor.createMenuButton("CREDITS", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    auto button = std::make_shared<Button>(this, rect, texture, &Button::go_to_credits, arcadeBonus);
    settingsButtons.push_back(button);
    rect.y += 175;

    showLoadMessage("creating buttons (2/15)", 512, 512, window);
    texture = tEditor.createMenuButton("REVERT", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::revert_settings, arcadeButton99);
    settingsButtons.push_back(button);
    rect.y += 175;

    showLoadMessage("creating buttons (3/15)", 512, 512, window);
    texture = tEditor.createMenuButton("RESET", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::reset_settings, arcadeButton99);
    settingsButtons.push_back(button);
    rect.y += 175;

    showLoadMessage("creating buttons (4/15)", 512, 512, window);
    texture = tEditor.createMenuButton("RETURN", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_mainMenu, arcadeBonus);
    settingsButtons.push_back(button);


    showLoadMessage("creating buttons (5/15)", 512, 512, window);
    rect.x = 512-(BUTTON_WIDTH/2);
    rect.y = 1008 - BUTTON_HEIGHT;
    texture = tEditor.createMenuButton("RETURN", BUTTON_WIDTH, BUTTON_HEIGHT, window, arcadeClassic48);
    button = std::make_shared<Button>(this, rect, texture, &Button::go_to_settings_from_credits, arcadeButton99);
    creditsButtons.push_back(button);


    rect = {64, 508, CHECKBOX_SIDELENGTH, CHECKBOX_SIDELENGTH};
    
    showLoadMessage("creating buttons (6/15)", 512, 512, window);
    auto unselected = std::make_shared<LTexture>(window),
         selected   = std::make_shared<LTexture>(window);
    if (!unselected->loadFromFile("../../assets/Menu/Buttons/UnselectedCheckbox.png")) {
        std::cerr << "Failed to load unselected checkbox!" << std::endl;
    }
    if (!selected->loadFromFile("../../assets/Menu/Buttons/SelectedCheckbox.png")) {
        std::cerr << "Failed to load selected checkbox!" << std::endl;
    }
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_CRT, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&CRT_FILTER) button->swap_textures();

    showLoadMessage("creating buttons (7/15)", 512, 512, window);
    rect.y += CHECKBOX_SIDELENGTH + 16;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_fullscreen, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&FULLSCREEN) button->swap_textures();
    
    showLoadMessage("creating buttons (8/15)", 512, 512, window);
    rect.y += CHECKBOX_SIDELENGTH + 16;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_FPS, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&SHOW_FPS) button->swap_textures();
    
    showLoadMessage("creating buttons (9/15)", 512, 512, window);
    rect.y = 852; rect.x = 512;
    button = std::make_shared<Button>(this, rect, unselected, &Button::toggle_instructions, arcadeButton99, selected);
    settingsButtons.push_back(button);
    if (settings.flags&SHOW_INSTRUCTIONS) button->swap_textures();

    showLoadMessage("creating buttons (10/15)", 512, 512, window);
    rect = {64, 820, BUTTON_WIDTH/2, BUTTON_HEIGHT/2};
    auto fpsbutton = std::make_shared<FPSButton>(-1, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);

    showLoadMessage("creating buttons (11/15)", 512, 512, window);
    rect.y += rect.h + 16;
    fpsbutton = std::make_shared<FPSButton>(120, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);

    showLoadMessage("creating buttons (12/15)", 512, 512, window);
    rect.x += rect.w + 16;
    fpsbutton = std::make_shared<FPSButton>(144, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);

    showLoadMessage("creating buttons (13/15)", 512, 512, window);
    rect.y -= rect.h + 16;
    fpsbutton = std::make_shared<FPSButton>(60, this, rect, &Button::doNothing, arcadeButton99);
    settingsButtons.push_back(fpsbutton);
    

    int minX = 72, maxX = 504;
    float t = (float)settings.volume / MIX_MAX_VOLUME;
    x = (minX * (1.0f - t)) + (maxX * t);

    showLoadMessage("creating buttons (14/15)", 512, 512, window);
    rect = {x, 169, SLIDER_WIDTH, SLIDER_HEIGHT};
    texture = tEditor.createSliderTexture(SLIDER_WIDTH, SLIDER_HEIGHT, window, {255,255,255,255});
    button = std::make_shared<Button>(this, rect, texture, &Button::volume_slider);
    settingsButtons.push_back(button);

    t = (float)settings.musicVolume / MIX_MAX_VOLUME;
    rect.x = (minX * (1.0f - t)) + (maxX * t);

    showLoadMessage("creating buttons (15/15)", 512, 512, window);
    rect.y = 341 - SLIDER_HEIGHT/2;
    button = std::make_shared<Button>(this, rect, texture, &Button::music_volume_slider);
    settingsButtons.push_back(button);
}

void GameMenu::load_assets()
{
    std::string filename = "../../assets/Menu/MainMenuBG.png";
    showLoadMessage("loading menu assets (1/14)\n"+filename, 512, 512, window);
    BGTexture = std::make_shared<LTexture>(window);
    if (!BGTexture->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Menu/GameOver.png";
    showLoadMessage("loading menu assets (2/14)\n"+filename, 512, 512, window);
    gameOverTex = std::make_shared<LTexture>(window);
    if (!gameOverTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Audio/ThinkFastChucklenuts.wav";
    showLoadMessage("loading menu assets (3/14)\n"+filename, 512, 512, window);
    buttonSound = std::make_shared<LAudio>();
    if (!buttonSound->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Menu/arcade-bonus.wav";
    showLoadMessage("loading menu assets (4/14)\n"+filename, 512, 512, window);
    arcadeBonus = std::make_shared<LAudio>();
    if (!arcadeBonus->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Menu/arcadebutton99.wav";
    showLoadMessage("loading menu assets (5/14)\n"+filename, 512, 512, window);
    arcadeButton99 = std::make_shared<LAudio>();
    if (!arcadeButton99->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Fonts/Seven_Segment.ttf";
    showLoadMessage("loading menu assets (6/14)\n"+filename, 512, 512, window);
    sevenSegment48 = TTF_OpenFont(filename.c_str(), 48);
    if (sevenSegment48 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Fonts/Seven_Segment.ttf";
    showLoadMessage("loading menu assets (7/14)\n"+filename, 512, 512, window);
    sevenSegment24 = TTF_OpenFont(filename.c_str(), 24);
    if (sevenSegment24 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    
    filename = "../../assets/Fonts/Seven_Segment.ttf";
    showLoadMessage("loading menu assets (8/14)\n"+filename, 512, 512, window);
    sevenSegment36 = TTF_OpenFont(filename.c_str(), 36);
    if (sevenSegment36 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    
    filename = "../../assets/Fonts/ARCADECLASSIC.TTF";
    showLoadMessage("loading menu assets (9/14)\n"+filename, 512, 512, window);
    arcadeClassic48 = TTF_OpenFont(filename.c_str(), 48);
    if (arcadeClassic48 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Fonts/ARCADECLASSIC.TTF";
    showLoadMessage("loading menu assets (10/14)\n"+filename, 512, 512, window);
    arcadeClassic24 = TTF_OpenFont(filename.c_str(), 24);
    if (arcadeClassic24 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    
    filename = "../../assets/Fonts/ARCADECLASSIC.TTF";
    showLoadMessage("loading menu assets (11/14)\n"+filename, 512, 512, window);
    arcadeClassic36 = TTF_OpenFont(filename.c_str(), 36);
    if (arcadeClassic36 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Fonts/ARCADECLASSIC.TTF";
    showLoadMessage("loading menu assets (12/14)\n"+filename, 512, 512, window);
    arcadeClassic18 = TTF_OpenFont(filename.c_str(), 18);
    if (arcadeClassic18 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Menu/Title.png";
    showLoadMessage("loading menu assets (13/14)\n"+filename, 512, 512, window);
    titleTex = std::make_shared<LTexture>(window);
    if (!titleTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }


    filename = "../../assets/CRT_Base_Texture.png";
    showLoadMessage("loading menu assets (14/14)\n"+filename, 512, 512, window);
    CRT_Base = std::make_shared<LTexture>(window);
    if (!CRT_Base->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    showLoadMessage("creating pixel filter", 512, 512, window);
    create_CRT_Texture();
    create_pause_texture();

    int w = wRect.x, h = wRect.h;
    aspectRatio = tEditor.createSolidColour(w, h, 0x000000FF, window);
}

void GameMenu::create_CRT_Texture()
{
    CRT_Tex = tEditor.createEmptyTexture(wRect.w, wRect.h, window);
    SDL_Rect rect = { 0, 0, int(CRT_Base->getWidth()*PIXEL_RENDER_SCALE), int(CRT_Base->getHeight()*PIXEL_RENDER_SCALE) };
    int nx = CRT_Tex->getWidth() / rect.w, ny = CRT_Tex->getHeight() / rect.h;

    for (int i = 0; i <= nx; i++) {
        int x = i * rect.w;
        for (int j = 0; j <= ny; j++) {
            int y = j * rect.h;
            rect.x = x; rect.y = y;
            tEditor.renderTextureToTexture(CRT_Tex, CRT_Base, &rect);
        }
    }
    CRT_Tex->setAlpha(PIXEL_FILTER_ALPHA);
}

void GameMenu::create_pause_texture() {
    pauseTex = tEditor.createSolidColour(wRect.w, wRect.h, 180, window);
}