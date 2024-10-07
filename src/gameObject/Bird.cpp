#include "Bird.hpp"
#include "../game/Game.hpp"

Bird::Bird(Vector2 target, int Idx, Game *game, int cell_sidelLen)
:   GameObject(bird, Vector2(-cell_sidelLen, -cell_sidelLen), Idx, 1, game, cell_sidelLen, false, cell_sidelLen-2),
    tex(game->BirdTex), timer(game->BIRD_FLIGHT_DURATION), targetPos(target)
{
    int idx = game->currLevel->gameObjects.size();
    auto targ = std::make_shared<Target>(this, idx);
    game->currLevel->gameObjects.push_back(targ);
    increment_idx();
}

void Bird::update()
{
    float dt = get_deltaTime();
    Vector2Int dimensions = get_mapDimensions();

    float sideLen = get_cellSidelen();
    
    float duration = game->BIRD_FLIGHT_DURATION,
          t, target_t;
    
    getInterps(&t, &target_t);
    
    if (t <= target_t && !hasDroppedBomb) {
        hasDroppedBomb = true;
        game->Instantiate(bomb, get_pos(), 1);
    } else if (t <= 0) {
        Destroy();
        return;
    }

    float x = t * dimensions.x;

    float val = t - target_t;
    t = 4.0f * val * val;

    float maxY = targetPos.y - (4.0f * sideLen),
          minY = targetPos.y - sideLen;

    float y = (maxY * t) + (minY * (1.0f - t));

    set_pos(Vector2(x, y));
    timer -= dt;
}

void Bird::getInterps(float *t, float *target_t)
{
    *t = timer/game->BIRD_FLIGHT_DURATION;
    *target_t = targetPos.x/get_mapDimensions().x;
}

Vector2 Bird::get_target() { return targetPos; }
bool Bird::has_dropped_bomb() { return hasDroppedBomb; }

void Bird::render(int camX, int camY, Uint8 alpha)
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





Target::Target(Bird *bird, int Idx)
:   GameObject(target, bird->get_target(), Idx, 1, bird->game, bird->get_cellSidelen(), false, bird->get_cellSidelen()-20),
    tex(game->TargetTex), bird(bird) {}

void Target::update()
{
    float t, target_t;
    bird->getInterps(&t, &target_t);
    if (t <= target_t && !bird->has_dropped_bomb()) Destroy();
}

void Target::render(int camX, int camY, Uint8 alpha)
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