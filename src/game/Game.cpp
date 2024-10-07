#include "Game.hpp"
#include "../gameObject/AStarPathfinding.hpp"

// constructor
Game::Game( std::shared_ptr<LWindow> Window, int resolutionWidth, int resolutionHeight ) 
: window(Window)
{
    // show that the game is loading
    showLoadMessage("loading game", 512, 512, window);

    // do this first!!!
    load_textures();
    load_animations();
    load_audio();
    load_fonts();
    create_textures();

    // camera dimensions should be the same as window size
    camera = { 0, 0, resolutionWidth, resolutionHeight };

    currSong = menuMusic;
    play_current();
}

// deallocate resources
Game::~Game()
{
    Base.free(); Woods.free(); Town.free();

    currNPC = nullptr;

    while (!secondRenders.empty()) secondRenders.pop();
    while (!dialogueRenders.empty()) dialogueRenders.pop();

    if (BGTexture != nullptr) BGTexture->free();
    if (overlayTexture != nullptr) overlayTexture->free();
    if (logTex != nullptr) logTex->free();
    if (damTex != nullptr) damTex->free();
    if (waterTex != nullptr) waterTex->free();
    if (grassTex != nullptr) grassTex->free();
    if (treeTex != nullptr) treeTex->free();
    if (stumpTex != nullptr) stumpTex->free();
    if (saplingTex != nullptr) saplingTex->free();
    if (playerTex != nullptr) playerTex->free();
    if (wolfTex != nullptr) wolfTex->free();
    if (falling_treeTex != nullptr) falling_treeTex->free();
    if (pine_coneTex != nullptr) pine_coneTex->free();
    if (plankTex != nullptr) plankTex->free();
    if (foxTex != nullptr) foxTex->free();
    if (berryTex != nullptr) berryTex->free();
    if (shoreline0Tex != nullptr) shoreline0Tex->free();
    if (shoreline1Tex != nullptr) shoreline1Tex->free();
    if (shoreline2Tex != nullptr) shoreline2Tex->free();
    if (shoreline3Tex != nullptr) shoreline3Tex->free();
    if (shoreline4Tex != nullptr) shoreline4Tex->free();
    if (closed_doorTex != nullptr) closed_doorTex->free();
    if (open_doorTex != nullptr) open_doorTex->free();
    if (dirtTex != nullptr) dirtTex->free();
    if (berry_bushTex != nullptr) berry_bushTex->free();
    if (empty_bushTex != nullptr) empty_bushTex->free();
    if (heartTex != nullptr) heartTex->free();
    if (stoneTex != nullptr) stoneTex->free();
    if (LMBTex != nullptr) LMBTex->free();
    if (MMBTex != nullptr) MMBTex->free();
    if (RMBTex != nullptr) RMBTex->free();
    if (BearTex != nullptr) BearTex->free();
    if (BirdTex != nullptr) BirdTex->free();
    if (BombTex != nullptr) BombTex->free();
    if (TargetTex != nullptr) TargetTex->free();
    if (dashed_circleTex != nullptr) dashed_circleTex->free();
    if (rabbitTex != nullptr) rabbitTex->free();

    if (playerAnimations != nullptr) playerAnimations->free();
    if (wolfWalkingAnimation != nullptr) wolfWalkingAnimation->free();
    if (explosionAnimation != nullptr) explosionAnimation->free();
    if (splashAnimation != nullptr) splashAnimation->free();

    if (logDestruction != nullptr) logDestruction->free();
    if (treeFalling != nullptr) treeFalling->free();
    if (doorToggle != nullptr) doorToggle->free();
    if (leaves != nullptr) leaves->free();
    if (pop != nullptr) pop->free();
    if (bonk != nullptr) bonk->free();
    if (birdSpawn != nullptr) birdSpawn->free();
    if (playerDamage != nullptr) playerDamage->free();
    if (healSound != nullptr) healSound->free();
    if (gameOverSound != nullptr) gameOverSound->free();
    if (explosionSound != nullptr) explosionSound->free();
    if (repairSound != nullptr) repairSound->free();

    if (vocalDeep0 != nullptr) vocalDeep0->free();
    if (vocalDeep1 != nullptr) vocalDeep1->free();
    if (vocalHigh0 != nullptr) vocalHigh0->free();
    if (vocalHigh1 != nullptr) vocalHigh1->free();
    if (vocalMed0 != nullptr) vocalMed0->free();
    if (vocalMed1 != nullptr) vocalMed1->free();

    for (int i = 0; i < 3; i++) {
        auto audio = wolfSpawnSounds[i];
        if (audio != nullptr) audio->free();
    }
    for (int i = 0; i < 3; i++) {
        auto audio = splashSounds[i];
        if (audio != nullptr) audio->free();
    }

    if (nightMusic != nullptr) nightMusic->free();
    if (dayMusic != nullptr) dayMusic->free();
    if (deathMusic != nullptr) deathMusic->free();
    if (menuMusic != nullptr) menuMusic->free();

    TTF_CloseFont(sevenSegment);
    TTF_CloseFont(arcadeClassic24);
    TTF_CloseFont(arcadeClassic36);

    if (fpsTex != nullptr) fpsTex->free();
    if (controlsTex != nullptr) controlsTex->free();
    if (clockTex != nullptr) clockTex->free();

    if (clockBackBar != nullptr) clockBackBar->free();
    if (redBar != nullptr) redBar->free();
    if (whiteBar != nullptr) whiteBar->free();
    if (darknessTex != nullptr) darknessTex->free();
}


float Game::get_time() { return g_time; }
float Game::get_deltaTime() { return deltaTime; }
bool Game::game_over() { return gameOver; }

void Game::new_game(bool show_tutorial) {
    gameOver = false;
    firstDay = true;
    showTutorial = show_tutorial && scores.mostNightsSurvived == 0;
}

void Game::clear_input() { inputKeys = 0; }

void Game::set_max_framerate( int maxFPS ) {
    MIN_DELTATIME = (maxFPS <= 0)? -1.0f : 1.0f / (float)maxFPS;
}

// handle events like player input
void Game::handle_events( SDL_Event& e )
{
    switch (e.type)
    {
        case SDL_KEYDOWN:
            if (e.key.repeat == 0) {
                switch (e.key.keysym.sym)
                {
                    case SDLK_w:
                        inputKeys |= 8; break;
                    case SDLK_a:
                        inputKeys |= 4; break;
                    case SDLK_s:
                        inputKeys |= 2; break;
                    case SDLK_d:
                        inputKeys |= 1; break;
                    case SDLK_LSHIFT:
                        inputKeys |= 16; break;
                    default:
                        handle_dialogue(e.key.keysym.sym); break;
                }
            }
            break;


        case SDL_KEYUP:
            if (e.key.repeat == 0) {
                switch (e.key.keysym.sym)
                {
                    case SDLK_w:
                        inputKeys &= ~8; break;
                    case SDLK_a:
                        inputKeys &= ~4; break;
                    case SDLK_s:
                        inputKeys &= ~2; break;
                    case SDLK_d:
                        inputKeys &= ~1; break;
                    case SDLK_LSHIFT:
                        inputKeys &= ~16; break;
                }
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            // left click
            switch (e.button.button)
            {
                case SDL_BUTTON_LEFT:
                    leftClickFunc(); break;
                case SDL_BUTTON_RIGHT:
                    rightClickFunc(); break;
                case SDL_BUTTON_MIDDLE:
                    if (currLevel->held != nullptr) throwSingleItem(); break;
            }
            break;
    }
}


// updates all the game objects
void Game::update_gameobjects()
{
    auto vec = &currLevel->gameObjects;
    for (int i = 0; i < vec->size(); i++) 
    {
        // std::cout << "Updating entity: "<< i <<'\t'<< "Number of objects: "<< vec->size() <<'\n';
        auto obj = (*vec)[i];
        if (obj != nullptr) obj->update();
        // if there is a secene switch, exit the loop
        if (switching_scenes) {
            switching_scenes = false; return;
        }
    }
}


// finds the time elapsed between frames
void Game::update_deltaTime() 
{
    clock_t t = clock(); // current time
    deltaTime = float(t - begin_time) / CLOCKS_PER_SEC;

    // NOTE: it seems that this method of finding deltaTime, that is, using the clock_t type, is 
    // only accurate to 1ms. therefore, it is impossible to cap at an exact framerate unless said
    // framerate can be represented as an integer number of milliseconds

    // for instance, it is possible to cap the game at 125fps, since 1/125 = 0.008 = 8 ms
    // it is impossible to cap the game at 120 fps, since 1/120 = 0.008333 = 8.3333 ms, which is not
    // an integer. rounding up to the next integer, 9ms, attempting to cap the framerate
    // at 120 will actually cap at 111fps (1/0.009 = 111.111) 
    while (deltaTime < MIN_DELTATIME)
    {   
        t = clock();
        deltaTime = float(t - begin_time) / CLOCKS_PER_SEC;
    }

    begin_time = t;
}


Vector2 Game::find_mouse_pos()
{
    int x, y;
    // get the coordinates of the mouse on the screen
    SDL_GetMouseState(&x, &y);
    x /= window->getScaleX(); y /= window->getScaleY();
    x -= renderOffset.x; y -= renderOffset.y;

    // convert that to coordinates in the game
    return Vector2(x+camera.x, y+camera.y);
}


void Game::attempt_enemy_spawn()
{
    // nothing will happen if the spawning attempt is invalid
    bool validAttempt = false;

    if (isNight && g_time >= DAY_TRANSITION_TIME) 
    {
        float rate = enemySpawnRate;
        if (currLevel != &Base) rate /= 2.0f;
        if (g_time - lastSpawn >= rate) {
            lastSpawn = g_time;
            validAttempt = true;
        }
    }

    if (validAttempt) 
    {
        float r = (float)rand() / RAND_MAX;
        if (r <= BIRD_SPAWN_CHANCE) spawnBird();
        else spawnWolf();
    }
}


std::shared_ptr<GameObject> Game::spawnWolf()
{
    // choose a random world edge to spawn on
    int edge; 
    int sideLen = currLevel->cell_sideLen, xRange = currLevel->gridDimensions.x*sideLen;

    Vector2 pPos = get_playerPos()/sideLen;
    Vector2Int pCell(pPos.x, pPos.y);
    int pNum = currLevel->grid[pCell.x][pCell.y];
    if (!(pNum&WATER)) {
        if (pCell.y > currLevel->gridDimensions.y/2) edge = 1;
        else edge = 0;
    } else edge = rand() % 2;

    float x = rand()%xRange, y = (edge == 0)? -50.0f : (currLevel->gridDimensions.y*sideLen)+50.0f;

    // spawn a wolf at the chosen location
    int idx = rand() % 3;
    wolfSpawnSounds[idx]->play();
    return Instantiate(wolf, Vector2(x, y), -1);
}

std::shared_ptr<GameObject> Game::spawnBird()
{
    // play a sound to indicate the bird spawning
    birdSpawn->play();
    return Instantiate(bird, get_playerPos(), 1);
}

// adds a game object and returns a pointer to it
std::shared_ptr<GameObject> Game::Instantiate( EntityType type, Vector2 pos, int hp, Scene *level )
{
    if (level == NULL) level = currLevel;

    int idx = level->gameObjects.size(), sideLen = level->cell_sideLen;
    std::shared_ptr<GameObject> obj = nullptr;

    switch (type)
    {
        case player:
            obj = std::make_shared<Player>(pos, idx, hp, this, sideLen);
            break;
        case wolf:
            obj = std::make_shared<Wolf>(pos, idx, hp, this, sideLen, nullptr);
            break;
        case bird:
            obj = std::make_shared<Bird>(pos, idx, this, sideLen);
            break;
        case bomb:
            obj = std::make_shared<Bomb>(pos, idx, this, sideLen);
            break;
        case target:
            std::cerr << "ERROR: Target may not be spawned using Instantiate!\n";
            break;
        case bombExplosionIndicator:
            std::cerr <<"ERROR: explosion indicator may not be spawned using Instantiate\n";
            break;
        case fallingTree:
            obj = std::make_shared<FallingTree>(pos, idx, this, sideLen);
            break;
        case ghostBuilding:
            obj = std::make_shared<GhostBuilding>(Vector2Int(pos.x, pos.y), currLevel->held->get_type(), idx, this, sideLen);
            break;
        case tutorialText:
            obj = std::make_shared<TutorialText>(pos, idx, this, sideLen);
            break;
        case explosion:
            obj = std::make_shared<Explosion>(pos, idx, this, sideLen);
            break;
        case splash:
            obj = std::make_shared<Splash>(pos, idx, this, sideLen);
            break;
        // NPCs
        case foxNPC:
        case bearNPC:
        case rabbitNPC:
            obj = std::make_shared<NPC>(type, pos, idx, this, sideLen);
            break;
        // items
        case logItem:
        case pineConeItem:
        case plankItem:
        case damItem:    
        case doorItem:      
        case stoneItem:     
        case berryItem:
            obj = std::make_shared<Item>(type, pos, idx, hp, this, sideLen);
            break;
        default:
            std::cerr << "Invalid Entity type used!\n";
            break;
    }
    if (obj != nullptr) level->gameObjects.push_back(obj);
    return obj;
}


void Game::Destroy( std::shared_ptr<GameObject> obj, std::vector<std::shared_ptr<GameObject>> *vec )
{
    if (vec == NULL) vec = &currLevel->gameObjects;

    if (obj == nullptr) return;

    if (obj->is_item()) {
        auto objItem = std::dynamic_pointer_cast<Item>(obj);
        if (objItem != nullptr) if (objItem->is_held()) currLevel->held = nullptr;
    }

    int n = obj->get_idx();
    // decrement the indices of all following game objects,
    // as the size of the vector will decrease
    for (int i = n+1; i < vec->size(); i++) (*vec)[i]->decrement_idx();

    // erase the object from the vector
    vec->erase( vec->begin() + n );
}


std::shared_ptr<Item> Game::spawnItemStack( EntityType type, Vector2 pos, int count )
{
    // only items will be spawned this way
    if (!is_item(type)) return nullptr;

    int num = RAND_MAX/2;
    Vector2 dir(rand()-num, rand()-num);

    // normalise dir to get a unit vector direction
    dir.normalise();

    auto obj = Instantiate(type, pos, count);

    obj->set_vel( dir * 35.0f );
    obj->set_accel( dir * -35.0f );

    return std::dynamic_pointer_cast<Item>(obj);
}


std::shared_ptr<Item> Game::craftTwoItems( std::shared_ptr<Item> item1, std::shared_ptr<Item> item2 )
{
    // validation
    if (item1 == nullptr || item2 == nullptr) return nullptr;
    if (item1->is_held() || item2->is_held()) return nullptr;
    if (item1->get_craftTimer() > 0.0f || item2->get_craftTimer() > 0.0f) return nullptr;
    
    std::shared_ptr<Item> res = nullptr;
    int hp1 = item1->get_HP(), hp2 = item2->get_HP();

    switch (item1->get_type())
    {
        case logItem:
            switch (item2->get_type())
            {
                case plankItem:
                {
                    // 1 log and 4 planks craft one DAM
                    // make sure there are adequate resources
                    int num = min(hp1, hp2/4);
                    if (num > 0)
                    {
                        res = spawnItemStack(damItem, item1->get_pos(), num);
                        item1->add_HP(-num); item2->add_HP(-4 * num);
                    }
                    break;
                }
            }
            break;

        case plankItem:
            switch (item2->get_type())
            {
                case logItem:
                {
                    // 1 logs and 4 planks craft one DAM
                    // make sure there are adequate resources
                    int num = min(hp1/4, hp2);
                    if (num > 0)
                    {
                        res = spawnItemStack(damItem, item1->get_pos(), num);
                        item1->add_HP(-4 * num); item2->add_HP(-num);
                    }
                    break;
                }
            }
            break;
    }
    if (res != nullptr) pop->play();
    return res;
}


std::shared_ptr<Item> Game::craftItem( std::shared_ptr<Item> item )
{
    std::shared_ptr<Item> res = nullptr;

    // which type of item will be created
    switch (item->get_type())
    {
        case logItem: // one log may be crafted into two planks
            // spawn a stack of two planks on the log stack
            res = spawnItemStack(plankItem, item->get_pos(), 2);
            res->set_craftTimer();
            item->add_HP(-1); // lose one log in crafting
            break;

        case damItem: {
            // DAMs may be deconstructed back into 4 planks and 1 logs
            Vector2 p = item->get_pos();
            res = spawnItemStack(logItem, p, 1);
            res->set_craftTimer();
            res = spawnItemStack(plankItem, p, 4);
            res->set_craftTimer();
            item->add_HP(-1);
            break;
        }

        case plankItem: { // four planks may be crafted into one door
            int hp = item->get_HP();
            if (hp >= 4) {
                // spawn a door on the plank stack
                res = spawnItemStack(doorItem, item->get_pos(), 1);
                res->set_craftTimer();
                item->add_HP(-4);
            }
            break;
        }

        case doorItem: // doors may be deconstructed into four planks
            res = spawnItemStack(plankItem, item->get_pos(), 4);
            res->set_craftTimer();
            item->add_HP(-1);
            break;
    }

    if (res != nullptr) {
        pop->play();
        // when crafting the held item, if you used all of the items in the held stack, 
        // make the product held
        if (item->is_held() && item->get_HP() <= 0) {
            currLevel->held = res;
            res->make_held();
        }
    }
    return res;
}


void Game::movePlayerToLevel( Scene *level, Vector2 newPlayerPos )
{
    // if the player is holding something, it should be removed from the current scene
    // and added to the new scene, as the held object
    if (currLevel->held != nullptr) {
        // revise using the same position as the player
        auto newHeld = Instantiate(currLevel->held->get_type(), newPlayerPos, currLevel->held->get_HP(), level);
        level->held = std::dynamic_pointer_cast<Item>(newHeld);
        Destroy(currLevel->held);
    }

    // std::cout << "creating a new player!\n";
    auto newPlayer = Instantiate(player, newPlayerPos, currLevel->player->get_HP(), level);
    level->player = std::dynamic_pointer_cast<Player>(newPlayer);
    Destroy(currLevel->player);
    currLevel->player = nullptr;
    
    currLevel = level;
    if (level->held != nullptr) level->held->make_held();
    interactRange = 3.0f * currLevel->cell_sideLen;

    // update pathfinding variables
    AStar::Open(&currLevel->grid, &barrier);

    enter_dialogue(None);

    initialise_BGTexture();

    switching_scenes = true;

    while(secondRenders.size()) secondRenders.pop();
    while(dialogueRenders.size()) dialogueRenders.pop();
    // std::cout << "finished switching scenes!\n";
}

void Game::set_paused(bool is_paused) { isPaused = is_paused; }

void Game::dayNightCycle()
{
    // after x seconds
    if (g_time >= DAY_LENGTH) 
    {
        stop_music();

        // toggle night, and make sure all scene objects get updated
        isNight = !isNight;

        if (isNight) nightMusic->play(0);

        // autosave the game
        save_game();
        // reset g_time to 0
        g_time = lastSpawn = 0.0f;
        return;

    } 
    else if (g_time == 0.0f) 
    {
        stop_music();

        if (!isNight)
        {
            if (!firstDay) {
                scores.mostNightsSurvived++;
                scores.calculate_score();
                // increase the spawn rate of enemies
                enemySpawnRate = clamp(MAX_SPAWN_RATE, START_SPAWN_RATE, enemySpawnRate-DIFF_SCALING);
            }
            else firstDay = false;
            // spawn npcs
            spawnNPCs();

            currSong = dayMusic;
        }
        else
        {
            currSong = nightMusic;
            firstDay = false;

            if (showTutorial) {
                Instantiate(tutorialText, Vector2(-50.0f, -50.0f), 1);
                showTutorial = false;
            }
        }
        play_current();
    }

    // update time
    g_time += deltaTime;
}

void Game::play_current( bool enteringMenu )
{
    if (enteringMenu) currSong = menuMusic;
    currSong->play(0, -1);
}

void Game::stop_music()
{
    if (currSong != nullptr) currSong->stop();
}

Vector2 Game::get_playerPos() { return currLevel->player->get_pos(); }

void Game::enter_dialogue( Dialogue newDialogue ) 
{ 
    currDialogue = newDialogue;
}


void Game::save_game()
{
    Base.Save("../../saves/curr/");
    Woods.Save("../../saves/curr/");
    Town.Save("../../saves/curr/");

    save_gameData("../../saves/curr/gameData.txt");
}



bool Game::is_item( int type ) { return type > ITEM_MIN && type < ITEM_MAX; }


bool Game::isInRegion( Vector2 p, SDL_Rect rect )
{
    float r = rect.x+rect.w, b = rect.y+rect.h;
    return p.x>=rect.x && p.x<=r && p.y>=rect.y && p.y<=b;
}

int Game::get_building( EntityType type )
{
    switch (type)
    {
        case logItem: return LOG;
        case pineConeItem: return SAPLING;
        case damItem: return DAM;
        case doorItem: return CLOSED_DOOR;
        default: return EMPTY;
    }
}


void Game::leftClickFunc()
{
    Vector2 mPos = find_mouse_pos();
    float r = (mPos - currLevel->player->get_pos()).length();

    // the cell the player clicked on
    float sideLen = currLevel->cell_sideLen;
    Vector2Int cell(mPos.x/sideLen, mPos.y/sideLen);
    // do something depending on the type of building in the clicked cell
    int val = currLevel->grid[cell.x][cell.y];

    switch (val&255)
    {
        case 7: // clicked a closed door, open it
            if (r <= interactRange) PlaceObjectInCell(cell, OPEN_DOOR, true);
            break;

        case 8: // clicked an open door, close it
            if (r <= interactRange) PlaceObjectInCell(cell, CLOSED_DOOR, true);
            break;
        
        default: // didn't click anything important, interact with items
            // if an item is NOT held, the player may pick up an item stack
            if (currLevel->held == nullptr && r <= interactRange)
            {
                // check all the game objects, and see if any of them ARE items AND were clicked
                auto vec = &currLevel->gameObjects;
                for (int i = 0; i < vec->size(); i++) {
                    auto obj = (*vec)[i];

                    if (isInRegion(mPos, obj->get_hitbox())) {
                        if (obj->is_item() && isInRegion(mPos, obj->get_hitbox()))
                        {
                            // make the player pick up the object and exit the function
                            setHeldObject(std::dynamic_pointer_cast<Item>(obj));
                            return;
                        } else if (obj->is_NPC()) {
                            auto npc = std::dynamic_pointer_cast<NPC>(obj);
                            npc->add_HP(1);
                            npc->set_dialogueTimer(npc->get_dialogueInterval());
                            currNPC = npc;
                            return;
                        }
                    }
                }
                // repair whatever cell that was clicked, if possible
                int health = val&HEALTH, maxHealth = val&MAX_HEALTH;
                if (health < maxHealth) damageCell(cell, -2);

            // if an item IS held, throw it :)
            } else if (currLevel->held != nullptr) {
                if (!tradeItem(currLevel->held->get_type(), currLevel->held->get_HP(), mPos)) 
                {
                    throwHeldObject();
                }
            }
            break;

    }
}

bool Game::tradeItem(int heldType, int heldHP, Vector2 mPos)
{
    int requiredHP, spawnedHP;
    EntityType targetType, spawnedType;

    switch (heldType)
    {
        case stoneItem:
            targetType = bearNPC;
            requiredHP = 2;
            spawnedType = berryItem;
            spawnedHP = 1;
            break;
        case logItem:
            targetType = rabbitNPC;
            requiredHP = 1;
            spawnedType = stoneItem;
            spawnedHP = 2;
            break;

        default: return false; // not holding a tradable item
    }

    if (heldHP < requiredHP) return false; // don't have enough of the item

    auto vec = &currLevel->gameObjects;
    for (int i = 0; i < vec->size(); i++)
    {
        auto obj = (*vec)[i];

        if (obj->get_type() == targetType) {
            if (isInRegion(mPos, obj->get_hitbox())) {
                // clicked on the correct entity for trading
                // remove the items from the players inventory
                currLevel->held->add_HP(-requiredHP);
                // spawn the item that was traded for
                spawnItemStack(spawnedType, obj->get_pos(), spawnedHP);
                repairSound->play();
                return true;
            }
        }
    }

    return false;
}

void Game::rightClickFunc()
{
    Vector2 mPos = find_mouse_pos();
    float r = (mPos - currLevel->player->get_pos()).length();
    // clicked too far away, do nothing
    if (r > interactRange) return;
    auto held = currLevel->held;

    if (held == nullptr)
    {
        // check to see if the player clicked on ANY entities
        auto vec = &currLevel->gameObjects;
        for (int i = 0; i < vec->size(); i++)
        {
            auto obj = (*vec)[i];
            auto item = std::dynamic_pointer_cast<Item>(obj);
            // nothing happens when clicking on non items
            if (item == nullptr) continue;


            if (isInRegion(mPos, item->get_hitbox())) {
                // right clicked an item stack, attempt to craft
                auto crafted = craftItem(item);
                // if successful, return
                if (crafted != nullptr) return;
            }
        }
    // if the player is holding berries, heal them
    } else if (held->get_type() == berryItem) {
        auto player = currLevel->player;
        int pHP = player->get_HP();
        if (pHP < player->get_maxHP()) {
            player->add_HP(1);
            held->add_HP(-1);
            healSound->play();
        }
        return;
    } 

    // the player didn't craft anything
    // find the cell they selected
    int sideLen = currLevel->cell_sideLen;
    Vector2Int cell(mPos.x/sideLen, mPos.y/sideLen);

    // the type of building to be placed in the cell
    int building = (held == nullptr)? EMPTY : get_building(held->get_type());

    int status = PlaceObjectInCell(cell, building, true);
    if (status == 0 && held != nullptr) held->add_HP(-1);
}


void Game::setHeldObject( std::shared_ptr<Item> obj )
{
    currLevel->held = obj;
    obj->make_held();
}

void Game::throwHeldObject()
{
    // release the held object
    auto obj = currLevel->held;
    currLevel->held = nullptr;

    Vector2 mPos = find_mouse_pos();
    // move away from the player, in the direction of the mouse
    Vector2 dir = getUnitVector(currLevel->player->get_pos(), mPos);

    // copied from player position func
    float s = currLevel->player->get_moveSpeed();
    if (inputKeys & 16) s *= 2.0f;
    Vector2 vel((bool(inputKeys&1)-bool(inputKeys&4)), (bool(inputKeys&2)-bool(inputKeys&8)));
    vel.normalise(); vel *= s;
    Vector2 playerVel = vel + currLevel->player->get_vel();

    Vector2 itemVel = playerVel + (dir * (5.2f * currLevel->cell_sideLen));
    itemVel *= THROWN_ITEM_SPEED_MULT;
    Vector2 accel = itemVel * -0.7f;

    obj->make_thrown( itemVel, accel );
}

void Game::throwSingleItem()
{
    int heldHP = currLevel->held->get_HP();
    // if there is only one item in the stack, just throw the whole thing
    if (heldHP == 1) throwHeldObject();

    else {
        // spawn a single item in the direction of the player's mouse, and give it velocity in said direction
        Vector2 mPos = find_mouse_pos();
        Vector2 pPos = currLevel->player->get_pos();
        Vector2 dir = getUnitVector(pPos, mPos);

        // copied from player position func
        float s = currLevel->player->get_moveSpeed();
        if (inputKeys&16) s *= 2.0f;
        Vector2 vel((bool(inputKeys&1)-bool(inputKeys&4)), (bool(inputKeys&2)-bool(inputKeys&8)));
        vel.normalise(); vel *= s;

        vel += currLevel->player->get_vel() + (dir * (4.5f * currLevel->cell_sideLen));
        vel *= THROWN_ITEM_SPEED_MULT;
        Vector2 accel  = vel * -0.7f;

        float r = 2.0f * (currLevel->player->get_radius() + currLevel->held->get_radius());
        Vector2 p = pPos + (dir * r);

        auto obj = Instantiate(currLevel->held->get_type(), p, 1);
        auto item = std::dynamic_pointer_cast<Item>(obj);
        item->make_thrown( vel, accel );

        currLevel->held->add_HP(-1);
    }
}

void Game::spawnNPCs() 
{
    Uint8 flags = 0b00000000; // ...... fox, rabbit
    // spawn NPCs in the town level

    int n = Town.gameObjects.size();
    for (int i = 0; i < n && flags != 3; i++) 
    {
        EntityType type = Town.gameObjects[i]->get_type();
        if (type == foxNPC) flags |= 2;
        else if (type == rabbitNPC) flags |= 1;
    }

    if (!(flags & 2)) { // no fox, spawn fox
        Vector2 pos(913.0f, -50.0f);
        Instantiate(foxNPC, pos, 1, &Town);
    }
    if (!(flags & 1)) { // no rabbit, spawn rabbit
        Vector2 pos(-50.0f, 850.0f);
        Instantiate(rabbitNPC, pos, 1, &Town);
    }

    // spawn NPCS in the base level
    if (showTutorial) {
        Vector2 pos(-50.0f, -50.0f);
        Instantiate(tutorialText, pos, 1, &Base);
    }
}


void Game::save_gameData( std::string filename )
{
    std::fstream file;
    file.open(filename, std::ios::out);
    if (!file) {
        std::cerr << "Failed to open " << filename <<std::endl;
    } else {
        file << std::dec << isNight <<'\t'<< riverDammed <<'\t' << enemySpawnRate;
        file.close();
    }
}

bool Game::is_under_tree( Vector2Int cell )
{
    int minY = max(cell.y, 0), maxY = min(currLevel->gridDimensions.y-1, cell.y+(TREE_HEIGHT-1)),
        minX = max(cell.x-(TREE_WIDTH/2), 0), maxX = min(cell.x+(TREE_WIDTH/2), currLevel->gridDimensions.x-1);


    for (int x = minX; x <= maxX; x++) 
    {
        for (int y = minY; y <= maxY; y++) 
        {
            if ((currLevel->grid[x][y]&CELL_ID) == 3) return true;
        }
    }
    return false;
}