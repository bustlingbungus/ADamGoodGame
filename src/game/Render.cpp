#include "Game.hpp"

bool is_in_region(Vector2 p, SDL_Rect rect) {
    int r = rect.x + rect.w, b = rect.y + rect.h;
    return p.x>=rect.x && p.x<=r && p.y>=rect.y && p.y <= b;
}

void Game::render_controls()
{
    SDL_Rect mRect = {8+renderOffset.x, 864+renderOffset.y, 36, 45};
    std::string txt;

    Vector2 mPos = find_mouse_pos();

    // if something is held
    if (currLevel->held != nullptr) render_held_object_controls(mPos, &mRect);
    else {
        // ensure the mouse is actually within range of the player
        Vector2 disp = currLevel->player->get_pos() - mPos;
        if (disp.length() <= interactRange) 
        {
            if (!render_hovering_over_entity_controls(mPos, &mRect)) {
                render_cell_controls(mPos, &mRect);
            }
        }
    }
}

void Game::render_held_object_controls( Vector2 mPos, SDL_Rect *mRect )
{
    int type = -1;

    // check game objects vector for NPCs
    int n = currLevel->gameObjects.size();
    for (int i = 0; i < n; i++) 
    {
        auto obj = currLevel->gameObjects[i];
        if (isInRegion(mPos, obj->get_hitbox()) && obj->is_NPC()) {
            type = obj->get_type(); break;
        }
    }

    // if hovering over an npc, render trades if applicable
    bool flag = true;
    render_NPC_trade_controls(type, mRect, &flag);

    // if not hovering over any potential trades, show throw/build commands
    if (flag) render_BuildThrow_commands(mPos, mRect);
}

void Game::render_NPC_trade_controls( int type, SDL_Rect *mRect, bool *flag )
{
    std::string txt;

    if (type != -1)
    {
        // may left click to trade 2 stone for 1 berry
        if (type == bearNPC && currLevel->held->get_type() == stoneItem)
        {
            // render the left mouse icon
            LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

            // render the text
            txt = "Trade 2 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

            mRect->w = mRect->h = 30;
            stoneTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

            txt = " for 1 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

            berryTex->render(mRect->x, mRect->y, mRect);
            *flag = false;
            return;
        }

        else if (type == rabbitNPC && currLevel->held->get_type() == logItem)
        {
            // render the left mouse icon
            LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

            // render the text
            txt = "Trade 1 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

            mRect->w = mRect->h = 30;
            logTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

            txt = " for 1 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

            stoneTex->render(mRect->x, mRect->y, mRect);
            *flag = false;
            return;
        }
    }
    *flag = true;
}

void Game::render_BuildThrow_commands( Vector2 mPos, SDL_Rect *mRect )
{
    int x = mRect->x;
    int type = currLevel->held->get_type(), hp = currLevel->held->get_HP();
    std::string txt;

    // find the amount of damage the item will do when thrown
    int damage = currLevel->held->get_damage();


    // if there is only one item, middle OR left click to throw it
    if (hp == 1)
    {
        LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w+5;
        MMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

        txt = "Throw item   ";
        controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
        txt = std::to_string(damage) + " damage";
        controlsTex->loadFromRenderedText(txt, {255,0,0,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
        mRect->x = x; mRect->y += mRect->h + 8;
    }
    // if multiple items are held, middle click to throw all, or left click to throw one
    else
    {
        LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

        txt = "Throw all " + std::to_string(hp) + " items   ";
        controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
        txt = std::to_string(ceilToInt(damage)) + " damage";
        controlsTex->loadFromRenderedText(txt, {255,0,0,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
        mRect->x = x; mRect->y += mRect->h + 8;

        txt = "Throw one item   ";
        MMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
        controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
        txt = std::to_string(ceilToInt(currLevel->held->get_damageMult())) + " damage";
        controlsTex->loadFromRenderedText(txt, {255,0,0,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
        mRect->x = x; mRect->y += mRect->h + 8;
    }

    // only render a right click command if the player is holding an item type that may be built with
    Vector2 dist = get_playerPos() - mPos;
    if (dist.length() < interactRange || type == berryItem)
    {
        int sideLen = currLevel->cell_sideLen;
        Vector2Int cell(mPos.x/sideLen, mPos.y/sideLen);
        int num = currLevel->grid[cell.x][cell.y];
        bool a = num&OCCUPIED,
             b = (num&(WATER|IS_DRIED))==WATER,
             c = type != damItem;
        if (!(a || (b && c)) || type == berryItem)
        {
            bool renderFlag = false;

            switch (type)
            {
                case logItem:
                    txt = "Build log";
                    renderFlag = true;
                    break;
                case pineConeItem:
                    txt = "Plant sapling";
                    renderFlag = true;
                    break;
                case damItem:
                    txt = "Build dam";
                    renderFlag = true;
                    break;
                case doorItem:
                    txt = "Build door";
                    renderFlag = true;
                    break;
                case berryItem:
                    txt = "Heal 1 hp";
                    renderFlag = true;
                    break;
            }

            if (renderFlag) 
            {
                RMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
                controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
                controlsTex->render(mRect->x, mRect->y + 8);
                mRect->x = x; mRect->y += mRect->h + 8;
            }
        }
    }
}

bool Game::render_hovering_over_entity_controls( Vector2 mPos, SDL_Rect *mRect )
{
    std::string txt;
    bool res = false;

    // check to see if the player is hovering over any game objects
    int type = -1, hp;
    int n = currLevel->gameObjects.size();
    for (int i = 0; i < n; i++) {
        auto obj = currLevel->gameObjects[i];
        if (isInRegion(mPos, obj->get_hitbox())) {
            type = obj->get_type(); 
            hp = obj->get_HP();
            break;
        }
    }

    // hovering over an item
    if (type > ITEM_MIN && type < ITEM_MAX)
    {
        int x = mRect->x;
        // left click to pick up the item
        LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
        txt = "Pick up item";
        controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8);
        mRect->x = x; mRect->y += mRect->h + 8;
        render_crafting_controls(type, mRect, hp);
        res = true;
    }
    // hovering over an npc
    else if (type > NPC_MIN && type < NPC_MAX)
    {
        // left click to talk
        LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
        txt = "Talk";
        controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8);
        res = true;
    }

    // return whether or not any controls were rendered
    return false;
}

void Game::render_crafting_controls( int type, SDL_Rect *mRect, int hp )
{
    std::string txt;

    switch (type)
    {
        // right click to craft 2 planks from one log
        case logItem:
        {
            RMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            int w = mRect->w, h = mRect->h;

            txt = "Craft 2 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

            mRect->w = mRect->h = 30;
            plankTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

            txt = " for 1 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
            logTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            break;
        }

        // right click to craft 1 door for 4 planks
        case plankItem:
        {
            if (hp >= 4)
            {
                RMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
                int w = mRect->w, h = mRect->h;

                txt = "Craft 1 ";
                controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
                controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

                mRect->w = mRect->h = 30;
                closed_doorTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

                txt = " for 4 ";
                controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
                controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
                plankTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            }
            break;
        }

        // right click to craft 4 planks for 1 door
        case doorItem:
        {
            RMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            int w = mRect->w, h = mRect->h;

            txt = "Craft 4 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

            mRect->w = mRect->h = 30;
            plankTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

            txt = " for 1 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
            closed_doorTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            break;
        }

        // right click to craft 4 planks and 1 log for 1 dam
        case damItem:
        {
            RMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            int w = mRect->w, h = mRect->h;

            txt = "Craft 4";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();

            mRect->w = mRect->h = 30;
            plankTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;

            txt = "and 1 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
            logTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            

            txt = " for 1 ";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8); mRect->x += controlsTex->getWidth();
            damTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            break;
        }
    }
}

void Game::render_cell_controls( Vector2 mPos, SDL_Rect *mRect )
{
    std::string txt;

    Vector2Int cell(mPos.x/currLevel->cell_sideLen, mPos.y/currLevel->cell_sideLen);
    int type = currLevel->grid[cell.x][cell.y];
    int x = mRect->x;

    // if the cell is not full hp, left click to repair it
    int hp = (type&HEALTH)>>8, max_hp = (type&MAX_HEALTH)>>17;
    if (hp < max_hp)
    {
        LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
        txt = "Repair";
        controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8);
        mRect->x = x; mRect->y += mRect->h + 8;
    }
    // left click toggle doors
    else if ((type&CELL_ID) == 7 || (type&CELL_ID) == 8)
    {
        if ((type&CELL_ID) == 7) 
        {
            LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            txt = "Open door";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8);
            mRect->x = x; mRect->y += mRect->h + 8;
        }
        else
        {
            LMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
            txt = "Close door";
            controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
            controlsTex->render(mRect->x, mRect->y + 8);
            mRect->x = x; mRect->y += mRect->h + 8;
        }
    }
    // if the cell may be destroyed by the player, right click to destroy
    if ((type&(OCCUPIED|INDESTRUBTIBLE)) == OCCUPIED)
    {
        RMBTex->render(mRect->x, mRect->y, mRect); mRect->x += mRect->w + 8;
        txt = "Destroy";
        controlsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24);
        controlsTex->render(mRect->x, mRect->y + 8);
    }
}




void Game::center_camera_on_player()
{
    Vector2 pPos = currLevel->player->get_pos(),
            cPos = pPos - Vector2(camera.w/2, camera.h/2);
    
    // if the camera is on the edge of the map, snap it back in bounds
    clamp(0.0f, float(map.w-camera.w), &cPos.x);
    clamp(0.0f, float(map.h-camera.h), &cPos.y);

    camera.x = cPos.x; camera.y = cPos.y;
}

void Game::render_framerate()
{
    // get the framerate
    int newFPS = 1.0f / deltaTime;
    fps = (newFPS > 0)? newFPS : fps; // validation
    std::string txt = std::to_string(fps) + "FPS"; // get the fps as a string

    // get the string as a texture, and render it
    if (!fpsTex->loadFromRenderedText(txt, {255,255,255,255}, arcadeClassic24)) {
        std::cerr << "couldn't render FPS!" << std::endl;
        return;
    }
    fpsTex->render(renderOffset.x, renderOffset.y);
}

void Game::render_player_health()
{
    int hp = currLevel->player->get_HP(); // the number of full hearts there will be
    int x = (camera.w-(60*hp))/2;
    // the space each heart will take up
    SDL_Rect heartRect = {x+renderOffset.x, 816+renderOffset.y, 50, 50};

    for (int i = 0; i < hp; i++)
    {
        heartTex->render(heartRect.x, heartRect.y, &heartRect);
        heartRect.x += heartRect.w + 10;
    }
}

// renders the scene's background
void Game::render_background() {
    BGTexture->render(renderOffset.x, renderOffset.y, &camera, &camera);
}

void Game::render_overlay() {
    overlayTexture->render(renderOffset.x, renderOffset.y, &camera, &camera);
}

void Game::render_darkness() 
{
    // only update the darkness cover at the beginning of each day/night
    if (g_time < 10.0f) createDarknessTex();
    // render the darkness over the scene
    darknessTex->render(renderOffset.x, renderOffset.y, &camera);
}

void Game::createDarknessTex()
{
    if (g_time < 10.0f)
    {
        // use an interpolator to determine how dark the texture should be
        float t = g_time / 10.0f;
        int alpha = (isNight)? 100.0f * t : 100.0f * (1.0f - t);

        if (darknessTex != nullptr) darknessTex->free(); // cleanup
        darknessTex = tEditor.createSolidColour(camera.w, camera.h, alpha, window);
    }
    else if (isNight) darknessTex = tEditor.createSolidColour(camera.w, camera.h, 100, window);
    else darknessTex = tEditor.createSolidColour(camera.w, camera.h, 0, window);
}


// renders all the game objects
void Game::render_gameobjects() {
    auto vec = &currLevel->gameObjects;
    for (int i = 0; i < vec->size(); i++) {
        if (switching_scenes) {
            switching_scenes = false; return;
        }
        (*vec)[i]->render( camera.x-renderOffset.x, camera.y-renderOffset.y );
    }
}

void Game::render_clock()
{
    // show how far into the day the player is, as an actual number
    // get the time as a number of minutes and seconds, and convert to a string
    int minutes = (int)g_time / 60, seconds = (int)g_time % 60;
    std::string txt = (seconds < 10)?
        std::to_string(minutes) + ":0" + std::to_string(seconds) : // make sure to render the leading zero on seconds
        std::to_string(minutes) + ':' + std::to_string(seconds);

    // create a texture from the time
    if (!clockTex->loadFromRenderedText(txt, {255,0,0,255}, arcadeClassic24)) {
        std::cerr << "Failed to create clock" << std::endl;
    }
    // render the clock
    int x = camera.w-106 + renderOffset.x;
    clockTex->render(x, renderOffset.y + 25);



    // render a progress bar to visualise how much time is left
    // use an interpolator to represent how far into the day the player is
    float t = g_time / DAY_LENGTH;

    // the rect the progress bar will be rendered to
    SDL_Rect barRect = {x, renderOffset.y + 7, 75, 5};

    clockBackBar->render(barRect.x+3, barRect.y + 3, &barRect);
    barRect.h = 11;

    // during the day, the bar will be empty and get more full. during the night, the bar will
    // start full and become empty
    int wRed = (isNight)? 81.0f * (1.0f-t) : 81.0f * t;
    barRect.w = wRed;
    if (isNight) barRect.x += 81 - wRed;

    if (wRed > 0) redBar->render(barRect.x, barRect.y, &barRect);
}

void Game::render_gameobjects_under_trees()
{
    // while rendering each game object, a pointer is added to second renders if the object
    // is behind a tree. render these objects again but slightly transparent
    while (secondRenders.size())
    {
        auto obj = secondRenders.top();
        obj->render( camera.x-renderOffset.x, camera.y-renderOffset.y, 100);
        secondRenders.pop();
    }
}

void Game::render_dialogue()
{
    // when applicable, dialogue items will be added to this stack. render them
    while (dialogueRenders.size())
    {
        auto diag = dialogueRenders.top();
        diag->render();
        dialogueRenders.pop();
    }
}