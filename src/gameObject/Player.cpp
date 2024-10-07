#include "Player.hpp"
#include "Animations.hpp"
#include "../game/Game.hpp"

Player::Player(Vector2 pos, int Idx, int Health, Game *game, int cell_sidelen)
: GameObject(player, pos, Idx, Health, game, cell_sidelen, true, cell_sidelen-2), animations(game->playerAnimations)
{
    moveSpeed = 2.6666667f * cell_sidelen;
}

float Player::get_moveSpeed() { return moveSpeed; }

void Player::set_dialogueTimer(float timer) { dialogueTimer = timer; }

void Player::set_HP(int newHP)
{
    if (newHP < get_HP()) game->playerDamage->play();
    else game->healSound->play();
    GameObject::set_HP(newHP);
}

void Player::add_HP(int amount)
{
    if (amount < 0) game->playerDamage->play();
    else if (amount > 0) game->healSound->play();
    GameObject::add_HP(amount);
}

void Player::update()
{
    (this->*updatePos)();
    (this->*updateVelocity)();
    collision();
    dialogue();

    // game ends when the player dies
    if (get_HP() <= 0) gameOver();
    else GameObject::set_HP(clamp(0, get_maxHP(), get_HP()));
}

void Player::gameOver()
{
    game->currSong = game->deathMusic;
    game->play_current();
    game->gameOverSound->play();
    game->gameOver = true;
}

void Player::move_from_user_input()
{
    float s = moveSpeed;
    Uint8 inputs = get_inputKeys();
    float dt = get_deltaTime();

    if (inputs&16) s *= 2.0f;

    Vector2 vel(
        // horizontal velocity: bit 1=d; move right (+x), bit 3=a; move left (-x)
        (bool(inputs&1)-bool(inputs&4)),
        // vertical velocity: bit 2=s; move down (+y), bit 4=w; move up (-y)
        (bool(inputs&2)-bool(inputs&8))
    );

    // normalise the vector so that players can't walk faster by moving diagonally
    vel.normalise(); 
    vel *= s; // multiply the unit vector by movement speed

    Vector2 p = get_pos() + (get_vel()+vel)*dt;

    set_pos(p);
}

void Player::return_to_shore()
{
    Vector2Int cell = get_cell();
    int num = game->currLevel->grid[cell.x][cell.y];

    if (!(num&WATER))
    {
        // return to normal behaviour
        set_vel(Vector2_Zero);
        updatePos = &Player::move_from_user_input;
        updateVelocity = &GameObject::default_update_velocity;
        set_collision(true);
    }
    else
    {
        Vector2Int dimensions = get_mapDimensions();
        Vector2 dir = (get_pos().y >= dimensions.y/2)? Vector2_Up : Vector2_Down;

        dir.x = 0.5f;
        dir.normalise();

        set_vel(dir * 3.0f * get_cellSidelen());
    }
}


Uint8 Player::get_inputKeys() { return game->inputKeys; }

std::shared_ptr<LTexture> Player::animate(bool updateIdx)
{
    float s = moveSpeed;
    Uint8 inputs = get_inputKeys();
    float dt = get_deltaTime();
    if (inputs&16) s *= 2.0f;
    Vector2 vel((bool(inputs&1)-bool(inputs&4)), (bool(inputs&2)-bool(inputs&8)));
    vel.normalise(); 
    vel *= s; // multiply the unit vector by movement speed
    vel += get_vel();

    FacingDirection look = Default; float idx = -1.0f;
    if (vel == Vector2_Zero) 
    {
        Vector2 mPos = game->find_mouse_pos();
        Vector2 dir = getUnitVector(get_pos(), mPos);

        if (dir.y >= SIN45) look = Forwards;
        else if (dir.y <= -SIN45) look = Backwards;
        else if (dir.x <= 0.0f) look = Left;
        else look = Right;

        idx = 0.0f;
    }
    else 
    {
        Vector2 dir = vel.normalised();

        if (dir.y >= SIN45) look = Forwards;
        else if (dir.y <= -SIN45) look = Backwards;
        else if (dir.x <= 0.0f) look = Left;
        else look = Right;
    }

    return animations->getTexture(get_deltaTime(), look, idx, updateIdx);
}

void Player::render(int camX, int camY, Uint8 alpha)
{
    // std::cout << "rendering player\n";
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    Vector2Int cell = get_cell();
    bool underTree = game->is_under_tree(cell), alpha255 = alpha == 255;

    if (game->isPaused || tex == nullptr) tex = animate(!alpha255 || (alpha255 && !underTree));

    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);

    if (underTree && alpha255) {
        game->secondRenders.push(this);
    } 
    // std::cout << "rendered player\n";
}

void Player::collision()
{
    if (!has_collision()) return;
    /* collide with other game objects */
    handleCollisionWithGameObjects();
    if (collideWithWorldBorders()) return;
    handleCornerCollisionsWithWalls();

    Vector2Int cell = get_cell();
    int num = game->currLevel->grid[cell.x][cell.y];
    if ((num&(WATER|BARRIER)) == (WATER|BARRIER))
    {
        add_HP(-4);
        updatePos = &GameObject::defualt_update_position;
        updateVelocity = &Player::return_to_shore;
        set_collision(false);
        int idx = rand()%2 + 1;
        game->splashSounds[idx]->play();
    }
}

void Player::handleCollisionWithGameObjects()
{
    // get the vector of game objects in the current level
    auto vec = &game->currLevel->gameObjects;

    // iterate through all the other game objects to find the distance between them

    for (int i = 0; i < vec->size(); i++)
    {
        auto other = (*vec)[i];

        // doesn't collide with itself or objects that have no collision
        if (get_idx() == i || !other->has_collision()) continue;
        if (other->is_item()) {
            auto item = std::dynamic_pointer_cast<Item>(other);
            if (item->get_playerCollisionTimer() > 0.0f) continue;
        }

        // find the displacement between the two objects
        Vector2 disp = other->get_pos() - get_pos();
        // break up objects directly on each other
        if (disp == Vector2_Zero) disp = Vector2_One;

        // d is the magnitude of the displacement,
        // r is the sum of the radii of the two objects
        float d = disp.length(), r = get_radius() + other->get_radius();

        // if d < r, the objects are too close to each other and need to be pushed apart
        if ( d < r )
        {
            // player will not collide with held object
            if (other->is_item()) {
                auto item = std::dynamic_pointer_cast<Item>(other);
                if (item->is_held()) continue;
            }
            // normalise the displacement to find the direction the objects need to move
            disp.normalise();

            // knock objects apart
            float sideLen = get_cellSidelen();
            Vector2 newVel = disp * (2.0f * sideLen);

            other->set_vel(newVel);
            other->set_accel(newVel * -1.5f);
        }
    }
}

bool Player::collideWithWorldBorders()
{
    SDL_Rect hitbox = get_hitbox();
    Vector2 pos = get_pos();
    Vector2Int map = get_mapDimensions();
    if (hitbox.x <= 0.0f) {
        Scene *lvl = game->currLevel->left;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x-hitbox.w-2, newMap.y/2);
            game->movePlayerToLevel(lvl, newPos);
            return true;
        } else {
            hitbox.x = 0.0f;
            pos.x = hitbox.x + hitbox.w/2;
        }
    } else if (hitbox.x >= map.x-hitbox.w) {
        Scene *lvl = game->currLevel->right;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(hitbox.w, newMap.y/2);
            game->movePlayerToLevel(lvl, newPos);
            return true;
        } else {
            hitbox.x = map.x-hitbox.w;
            pos.x = hitbox.x + hitbox.w/2;
        }
    }
    if (hitbox.y <= 0.0f) {
        Scene *lvl = game->currLevel->above;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x/2, newMap.y-hitbox.h-2);
            game->movePlayerToLevel(lvl, newPos);
            return true;
        } else {
            hitbox.y = 0.0f;
            pos.y = hitbox.y + hitbox.h/2;
        }
    } else if (hitbox.y >= map.y-hitbox.h) {
        Scene *lvl = game->currLevel->below;
        if (lvl != nullptr) {
            Vector2Int newMap = lvl->gridDimensions * lvl->cell_sideLen;
            Vector2 newPos(newMap.x/2, hitbox.h);
            game->movePlayerToLevel(lvl, newPos);
            return true;
        } else {
            hitbox.y = map.y-hitbox.h;
            pos.y = hitbox.y + hitbox.h/2;
        }
    }

    set_pos(pos);
    return false;
}

void Player::handleCornerCollisionsWithWalls()
{
    int sideLen = get_cellSidelen();

    SDL_Rect hitbox = get_hitbox();
    Vector2 pos = get_pos(), velocity = get_vel(), acceleration = get_accel();

    // top left
    Vector2 disp = Vector2_Zero;
    Vector2Int cell(hitbox.x/sideLen, hitbox.y/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = ((Vector2(cell.x,cell.y)+Vector2_One)*sideLen)-Vector2(hitbox.x,hitbox.y);
    }
    if (disp != Vector2_Zero) {
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                velocity.y = acceleration.y = 0.0f;
            }
        }
    }
    // top right
    disp = Vector2_Zero;
    cell = Vector2Int((hitbox.x+hitbox.w)/sideLen, hitbox.y/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = (Vector2(cell.x,cell.y+1)*sideLen)-Vector2(hitbox.x+hitbox.w,hitbox.y);
    }
    if (disp != Vector2_Zero) {
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                velocity.y = acceleration.y = 0.0f;
            }
        }
    }
    // bottom left
    disp = Vector2_Zero;
    cell = Vector2Int(hitbox.x/sideLen, (hitbox.y+hitbox.h)/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = (Vector2(cell.x+1,cell.y)*sideLen)-Vector2(hitbox.x,hitbox.y+hitbox.h);
    }
    if (disp != Vector2_Zero) {
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                velocity.y = acceleration.y = 0.0f;
            }
        }
    }
    // bottom right
    disp = Vector2_Zero;
    cell = Vector2Int((hitbox.x+hitbox.w)/sideLen, (hitbox.y+hitbox.h)/sideLen);
    if (game->is_barrier(cell)) {
        // displacement bettwen TOP LEFT corner of hitbox and BOTTOM RIGHT corner of cell
        disp = (Vector2(cell.x,cell.y)*sideLen)-Vector2(hitbox.x+hitbox.w,hitbox.y+hitbox.h);
    }
    if (disp != Vector2_Zero) {
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                velocity.y = acceleration.y = 0.0f;
            }
        }
    }

    set_pos(pos); set_vel(velocity); set_accel(acceleration);
}

