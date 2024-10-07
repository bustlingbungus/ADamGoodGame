#include "CosmeticGameObjects.hpp"
#include "../game/Game.hpp"
#include "../game/CellTypes.hpp"
#include "Item.hpp"

/*          FALLING TREES          */

FallingTree::FallingTree(Vector2 pos, int Idx, Game* game, int cellSideLen)
:   GameObject(fallingTree, pos, Idx, 1, game, cellSideLen, false, game->TREE_WIDTH*cellSideLen, game->TREE_HEIGHT*cellSideLen),
    tex(game->falling_treeTex) 
{
    dir = (rand()%2)? -1.0f : 1.0f;
}

void FallingTree::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x - camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    // timer is initialised to 1.0f, use it as an interpolator
    float interp = 1.0f - timer;
    float theta = 100.0f * interp * dir;
    SDL_Point centre = { hitbox.w/2, hitbox.h };
    tex->render(p.x, p.y, &hitbox, NULL, theta, &centre);
}

void FallingTree::update()
{
    if (timer <= 0.0f)
    {
        // once the tree is gone, spawn some log + pine cone items in where it fell
        // the further log will be the height of the tree (size.x) to the LEFT of pos,
        // the nearest one could be at pos

        // 5 wood will spawn, 0-2 seeds will spawn
        int seeds = rand()%3, n = seeds+5;

        SDL_Rect hitbox = get_hitbox();
        Vector2Int size(hitbox.w/2, hitbox.h/2);
        int sideLen = get_cellSidelen();
        
        float step = (sideLen*10)/n, y = hitbox.y + hitbox.h;

        // spawn n items
        for (int i = 0; i < n; i++)
        {
            Vector2 p(hitbox.x + (step*(i+1)*dir), y);
            // spawn logs first, seeds last
            EntityType item = (i<5)? logItem : pineConeItem;
            game->spawnItemStack(item, p, 1);
        }
        Destroy();
    }
    timer -= get_deltaTime();
}




/*          GHOST BUILDINGS          */
GhostBuilding::GhostBuilding(Vector2Int cell, EntityType itemType, int Idx, Game *game, int cell_sideLen)
: GameObject(ghostBuilding, Vector2(((float)cell.x+0.5f)*cell_sideLen,((float)cell.x+0.5f)*cell_sideLen), Idx, 1, game, cell_sideLen, false, cell_sideLen)
{
    assign_texture(itemType);
}

void GhostBuilding::update()
{
    (this->*Update)();
}

void GhostBuilding::validate()
{
    if (!valid) Destroy();
    else Update = &GhostBuilding::go_to_mouse_cell;
}

void GhostBuilding::go_to_mouse_cell()
{
    // ensure that there is still something being held
    if (game->currLevel->held == nullptr) Destroy();
    else
    {
        Vector2 mPos = game->find_mouse_pos();
        float sideLen = get_cellSidelen();
        mPos /= sideLen;
        mPos.x = (int)mPos.x; mPos.y = (int)mPos.y;
        mPos *= sideLen;
        mPos += Vector2_One * 0.5f * sideLen;

        set_pos(mPos);

        Vector2Int cell = get_cell();
        int num = game->currLevel->grid[cell.x][cell.y];
        bool a = num&OCCUPIED,
             b = (num&(WATER|IS_DRIED))==WATER,
             c = game->currLevel->held->get_type() != damItem;
        show = !(a || (b && c));

        auto held = game->currLevel->held;
        if (held != nullptr) assign_texture(held->get_type());
    }
}

void GhostBuilding::assign_texture(EntityType itemType)
{
    switch (itemType)
    {
        case logItem: tex = game->logTex; valid = true; break;
        case pineConeItem: tex = game->saplingTex; valid = true; break;
        case damItem: tex = game->damTex; valid = true; break;
        case doorItem: tex = game->closed_doorTex; valid = true; break;
        default: valid = false; break;
    }
}

void GhostBuilding::render(int camX, int camY, Uint8 alpha)
{
    if (!valid) Destroy();
    else if (alpha == 255) {
        game->secondRenders.push(this);
    } else if (show) {
        SDL_Rect hitbox = get_hitbox();
        Vector2Int p( hitbox.x - camX, hitbox.y-camY );
        // not within the camera's view, don't render
        if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
            return;
        }
        Vector2 dist = game->get_playerPos() - get_pos();
        if (dist.length() > game->interactRange) return;

        tex->setAlpha(alpha);
        tex->render(p.x, p.y, &hitbox);
        tex->setAlpha(255);
    }
}




/*          TUTORIAL TEXT          */

TutorialText::TutorialText(Vector2 pos, int Idx, Game *game, int cell_sideLen)
: GameObject(tutorialText, pos, Idx, 1, game, cell_sideLen, false, cell_sideLen) 
{
    Update = (game->isNight)? &TutorialText::night_0_update : &TutorialText::stage_0_update;
}

void TutorialText::update() {
    (this->*Update)();
}

void TutorialText::render(int camX, int camY, Uint8 alpha) {
    if (alpha == 255) {
        game->secondRenders.push(this);
    } else if (txt != "") {
        Vector2 pos = get_pos();
        Vector2Int p( pos.x-camX, pos.y-camY );
        // not within the camera's view, don't render
        clamp(game->renderOffset.x+166, game->camera.w+game->renderOffset.x-166, &p.x);
        clamp(game->renderOffset.y+56, game->camera.h+game->renderOffset.y-56, &p.y);
        renderText(txt, p.x, p.y, game->window, {255,255,255,255}, game->arcadeClassic24);
    }
}

bool TutorialText::tree_has_been_chopped()
{
    if ((game->currLevel->grid[treeCell.x][treeCell.y]&CELL_ID) == 3) return false;
    else {
        if (findItem(plankItem, 1, false)) return findItem(logItem);
        return true;
    }
}

bool TutorialText::findItem(EntityType itemType, int minHP, bool assignItem)
{
    auto vec = &game->currLevel->gameObjects;
    int n = vec->size();
    for (int i = 0; i < n; i++) 
    {
        auto obj = (*vec)[i];
        EntityType type = obj->get_type();
        int hp = obj->get_HP();
        if (type == itemType && hp >= minHP) 
        {
            if (assignItem) {
                item = std::dynamic_pointer_cast<Item>(obj);
                return item != nullptr;
            } else return true;
        }
    }
    return false;
}

bool TutorialText::findNearestItem()
{
    auto vec = &game->currLevel->gameObjects;
    int n = vec->size();

    Vector2 pPos = game->get_playerPos();

    int bestIdx = -1;
    float least = INFINITY;

    for (int i = 0; i < n; i++) 
    {
        auto obj = (*vec)[i];
        if (obj->is_item() && obj->get_type() != berryItem)
        {
            Vector2 dist = pPos - obj->get_pos();
            float len = dist.length();
            if (len < least) {
                bestIdx = i;
                least = len;
            }
        }
    }
    if (bestIdx >= 0) item = std::dynamic_pointer_cast<Item>((*vec)[bestIdx]);
    return item != nullptr;
}

void TutorialText::stage_0_update()
{
    // instruct player to destroy a tree
    if (tree_has_been_chopped())
    {
        Update = &TutorialText::stage_1_update;
        txt = "";
    }
    else if (findItem(logItem))
    {
        Update = &TutorialText::stage_2_update;
        txt = "";
    }
    else
    {
        int sideLen = get_cellSidelen();
        Vector2 pos((treeCell.x-2)*sideLen, treeCell.y*sideLen);
        set_pos(pos);

        int time = game->get_time() * 0.75f;

        txt = "chop down tree";
        txt += (time%2)? ">" : " >";
    }
}

void TutorialText::stage_1_update() 
{
    if (findItem(logItem)) {
        Update = &TutorialText::stage_2_update;
    }
}

void TutorialText::stage_2_update()
{
    if (!findItem(logItem)) Update = &TutorialText::stage_0_update;
    else if (item == nullptr || findItem(plankItem)) 
    {
        txt = "";
        Update = &TutorialText::stage_3_update;
    }
    else
    {
        Vector2 logPos = item->get_pos();
        logPos.y -= 1.25f * get_cellSidelen();
        set_pos(logPos);

        int time = game->get_time() * 0.75f;
        txt = "craft planks\n";
        txt += (time%2)? "V" : " \nV";
    }
}

void TutorialText::stage_3_update()
{
    if (!findItem(logItem, 1, false)) Update = &TutorialText::stage_0_update;
    else if (game->currLevel->gameObjects[item->get_idx()] != item) findItem(plankItem, 1);
    else if (item == nullptr || findItem(plankItem, 4))
    {
        txt = "";
        Update = &TutorialText::stage_4_update;
    }
    else 
    {
        Vector2 pos = item->get_pos();
        pos.y -= 1.25f * get_cellSidelen();
        set_pos(pos);

        int time = game->get_time() * 0.75f;
        txt = "gather 4 planks\n";
        txt += (time%2)? "V" : " \nV";
    }
}

void TutorialText::stage_4_update()
{
    if (item != nullptr) {
        if (item->is_held())
        {
            txt = "";
            Update = (findItem(logItem, 1, false))? &TutorialText::stage_5_update : &TutorialText::stage_0_update;
        }
        else if (!findItem(logItem, 1, false)) Update = &TutorialText::stage_0_update;
        else if (findItem(damItem)) Update = &TutorialText::stage_7_update;
        else
        {
            Vector2 pos = item->get_pos();
            pos.y -= 1.25f * get_cellSidelen();
            set_pos(pos);

            int time = game->get_time() * 0.75f;
            txt = "pick up\n";
            txt += (time%2)? "V" : " \nV";
        }
    }
}

void TutorialText::stage_5_update()
{
    if (findItem(logItem)) {
        Update = &TutorialText::stage_6_update;
    } else Update = &TutorialText::stage_0_update;
}

void TutorialText::stage_6_update()
{
    if (item == nullptr || findItem(damItem))
    {
        txt = "";
        Update = &TutorialText::stage_7_update;
    }
    else if (!findItem(logItem, 1, false)) Update = &TutorialText::stage_0_update;
    else if (!findItem(plankItem, 4, false)) Update = &TutorialText::stage_1_update;
    else if (game->currLevel->held == nullptr) {
        findItem(plankItem, 4);
        Update = &TutorialText::stage_4_update;
    }
    else
    {
        Vector2 pos = item->get_pos();
        pos.y -= 1.5f * get_cellSidelen();
        set_pos(pos);

        int time = game->get_time() * 0.75f;
        txt = "combine stacks to\ncraft a dam\n";
        txt += (time%2)? "V" : " \nV";
    }
}

void TutorialText::stage_7_update()
{
    if (item->is_held())
    {
        treeCell = Vector2Int(20, 19);
        txt = "";
        Update = &TutorialText::stage_8_update;
    }
    // in case the player uncrafts the dam
    else if (!findItem(damItem))
    {
        txt = "";
        Update = &TutorialText::stage_5_update;
    }
    else
    {
        Vector2 pos = item->get_pos();
        pos.y -= 1.25f * get_cellSidelen();
        set_pos(pos);

        int time = game->get_time() * 0.75f;
        txt = "pick up\n";
        txt += (time%2)? "V" : " \nV";
    }
}

void TutorialText::stage_8_update()
{
    if (game->riverDammed) Destroy();
    else
    {
        int sideLen = get_cellSidelen();
        Vector2 pos(treeCell.x*sideLen, treeCell.y*sideLen);
        
        int time = game->get_time() * 0.75f;
        if (time%2) {
            pos .y -= 30.0f;
            txt = "^\n \n";
        } else txt = "^\n";

        txt += "bridge across river\nto block it";  
        set_pos(pos);
    }
}

void TutorialText::night_0_update()
{
    if (findNearestItem()) {
        txt = "";
        Update = &TutorialText::night_1_update;
    }
}

void TutorialText::night_1_update()
{
    if (item->is_held() || item == nullptr)
    {
        txt = "throw items to\ndamage enemies!";
        Update = &TutorialText::night_2_update;
    }
    else
    {
        Vector2 pos = item->get_pos();
        pos.y -= 1.25f * get_cellSidelen();
        set_pos(pos);

        int time = game->get_time() * 0.75f;
        txt = "pick up\n";
        txt += (time%2)? "V" : " \nV";
    }
}

void TutorialText::night_2_update() {
    if (!item->is_held()) Destroy();
}



/*          EXPLOSION         */

Explosion::Explosion(Vector2 pos, int Idx, Game *game, int cell_sideLen)
:   GameObject(explosion, pos, Idx, 1, game, cell_sideLen, false, game->BOMB_RADIUS*ROOT2*cell_sideLen),
    animation(game->explosionAnimation) {}

void Explosion::update() {
    if (timer <= 0.0f) Destroy();
    else timer -= get_deltaTime();
}

void Explosion::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x - camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    Vector2Int cell = get_cell();
    bool underTree = game->is_under_tree(cell), alpha255 = alpha == 255;

    tex = animate(!alpha255 || (alpha255 && !underTree));
    tex->setAlpha(min((int)alpha, 150));
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);

    if (underTree && alpha255) {
        game->secondRenders.push(this);
    }
}

std::shared_ptr<LTexture> Explosion::animate(bool updateIdx)
{
    return animation->getTexture(get_deltaTime(), Forwards, -1.0f, updateIdx);
}



/*          SPLASH          */

Splash::Splash(Vector2 pos, int Idx, Game *game, int cell_sideLen)
:   GameObject(splash, pos, Idx, 1, game, cell_sideLen, false, cell_sideLen),
    animation(game->splashAnimation) {}

void Splash::update() {
    if (timer <= 0.0f) Destroy();
    else timer -= get_deltaTime();
}

void Splash::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x - camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }

    tex = animate();
    tex->render(p.x, p.y, &hitbox);
}

std::shared_ptr<LTexture> Splash::animate()
{
    return animation->getTexture(get_deltaTime(), Forwards, -1.0f);
}