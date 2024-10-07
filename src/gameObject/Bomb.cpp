#include "Bomb.hpp"
#include "../game/Game.hpp"

Bomb::Bomb(Vector2 pos, int Idx, Game *game, int cell_sideLen)
:   GameObject(bomb, pos, Idx, 1, game, cell_sideLen, false, cell_sideLen-20),
    tex(game->BombTex), speed(cell_sideLen), timer(0.75f)
{
    set_accel(Vector2_Up * 8.0f * cell_sideLen);

    int idx = game->currLevel->gameObjects.size();
    indicator = std::make_shared<ExplosionIndicator>(Vector2(get_pos().x, get_pos().y+cell_sideLen), idx, game, cell_sideLen);
    game->currLevel->gameObjects.push_back(indicator);
    increment_idx();
}

void Bomb::update()
{
    (this->*updateVel)();

    float dt = get_deltaTime();

    if (timer <= 0.25f) {
        if (timer <= 0.0f) {
            explode();
            return;
        } else if (timer + dt > 0.25f) {
            set_vel(Vector2_Down * get_cellSidelen());
            updateVel = &GameObject::default_update_velocity;
        }
    }

    Vector2 newPos = get_pos() + get_vel() * dt;
    set_pos(newPos);

    timer -= dt;
}

void Bomb::fall() {
    set_vel(get_vel() + get_accel()*get_deltaTime());
}

void Bomb::explode()
{
    auto vec = &game->currLevel->gameObjects;
    int n = vec->size(), damage = game->BOMB_DAMAGE,
        sideLen = get_cellSidelen();
    float r = game->BOMB_RADIUS * sideLen;

    // damage nearby entities
    for (int i = 0; i < n; i++) 
    {
        // won't interact with itself
        if (i != get_idx()) 
        {
            auto obj = (*vec)[i];

            Vector2 disp = obj->get_pos() - get_pos();
            if (disp.length() < r && (obj->is_item() || obj->get_type() == player || obj->get_type() == wolf)) 
            {
                // damage the object
                obj->add_HP(-damage);
                // knock the object away
                disp.normalise();
                Vector2 vel = disp * 10.0f * sideLen;
                obj->set_vel(vel);
                obj->set_accel(vel * -1.0f);
            }
        }
    }

    // damage nearby cells
    Vector2Int cell = get_cell();
    int cellRadius = (int)game->BOMB_RADIUS;
    Vector2Int gridDimensions = game->currLevel->gridDimensions;

    int minX = max(0, cell.x-cellRadius), 
        maxX = min(gridDimensions.x, cell.x + cellRadius),
        minY = max(0, cell.y-cellRadius),
        maxY = min(gridDimensions.y, cell.y + cellRadius);

    for (int x = minX; x < maxX; x++) {
        for (int y = minY; y < maxY; y++) {
            Vector2Int currCell(x, y);
            game->damageCell(currCell, damage);
        }
    }

    game->Instantiate(explosion, get_pos(), 1);
    game->explosionSound->play();

    indicator->Destroy();
    Destroy();
}

void Bomb::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);


    Vector2Int cell = get_cell();
    if (game->is_under_tree(cell) && alpha == 255 && alpha == 255) {
        game->secondRenders.push(this);
    }
}





ExplosionIndicator::ExplosionIndicator(Vector2 pos, int Idx, Game *game, int cell_sideLen)
:   GameObject(bombExplosionIndicator, pos, Idx, 1, game, cell_sideLen, false, game->BOMB_RADIUS*2.0f*cell_sideLen), 
    tex(game->dashed_circleTex) {}

void ExplosionIndicator::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }

    if (alpha == 255) {
        game->secondRenders.push(this);
    } else {
        tex->setAlpha(alpha);
        tex->render(p.x, p.y, &hitbox);
    }
}