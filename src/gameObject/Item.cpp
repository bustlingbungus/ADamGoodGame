#include "Item.hpp"
#include "../game/Game.hpp"
#include "../engine/GameMath.hpp"

using namespace Math;

Item::Item(int Type, Vector2 pos, int idx, int count, Game *game, int cell_sideLen)
: GameObject((EntityType)Type, pos, idx, count, game, cell_sideLen, false, cell_sideLen-20)
{
    switch (Type)
    {
        case logItem:
            tex = game->logTex;
            damage = 0.5f;
            break;
        case pineConeItem:
            tex = game->pine_coneTex;
            damage = 0.333333333f;
            break;
        case plankItem:
            tex = game->plankTex;
            damage = 0.5f;
            break;
        case damItem:
            tex = game->damTex;
            damage = 0.5f;
            break;
        case doorItem:
            tex = game->closed_doorTex;
            damage = 0.5f;
            break;
        case stoneItem:
            tex = game->stoneTex;
            damage = 1.5f;
            break;
        case berryItem:
            tex = game->berryTex;
            damage = 0.0f;
            break;
        default:
            std::cerr << "Invalid item spawned: " << Type << std::endl;
    }
}

int Item::get_damage() { return ceilToInt(damage * get_HP()); }
float Item::get_damageMult() { return damage; }
float Item::get_playerCollisionTimer() {return playerCollisionTimer; }
bool Item::is_held() { 
    auto lvl = game->currLevel;
    if (lvl->held != nullptr) {
        int idx = get_idx();
        if (idx>=0 && idx<lvl->gameObjects.size()) {
            return  lvl->gameObjects[idx]->get_idx() == lvl->held->get_idx();
        }
    }
    return false; 
}
float Item::get_craftTimer() { return craftTimer; }
void Item::set_craftTimer() { craftTimer = craftInterval; }


void Item::update()
{
    (this->*updatePos)();
    (this->*updateVel)();
    collision();

    if (get_HP() <= 0) die();
    else set_HP(clamp(0, get_maxHP(), get_HP()));
}

void Item::spawn()
{
    Vector2Int cell = get_cell(), gridDimension = game->currLevel->gridDimensions;

    if (cell.x == clamp(0, gridDimension.x-1, cell.x) && cell.y == clamp(0, gridDimension.y-1, cell.y))
    {
        int num = game->currLevel->grid[cell.x][cell.y];
        if (!(num&BARRIER) || (num&WATER) || is_held())
        {
            set_accel(get_vel() * -1.0f);
            updateVel = &GameObject::default_update_velocity;
            updatePos = (is_held())? &Item::held_updatePos : &Item::thrown_updatePos;
            set_collision(true);
            return;
        }
    }

    Vector2Int target = get_mapDimensions() / 2;
    Vector2 dir = getUnitVector(get_pos(), Vector2(target.x, target.y));

    set_vel(dir * 3.0f * get_cellSidelen());
}

void Item::held_updatePos()
{
    // get a unit vector from the player's centre to the mouse position's centre
    Vector2 mPos = game->find_mouse_pos(), pPos = game->get_playerPos();
    Vector2 dir = getUnitVector( pPos, mPos );

    // place the item a fixes distance from the player, in the direction of the mouse
    float r = game->currLevel->player->get_radius() + get_radius() + 5.0f;
    set_pos( pPos + (dir * r) );
    halt();
}

void Item::thrown_updatePos()
{
    float dt = get_deltaTime();

    Vector2 vel = get_vel();
    Vector2 newPos = get_pos() + (vel * dt);
    set_pos(newPos);

    if (vel.length() < game->ITEM_MINIMUM_DAMAGE_VELOCITY * get_cellSidelen()) {
        damageTimer = damageInterval;
    } else damageTimer -= dt;
    craftTimer -= dt;
    playerCollisionTimer -= dt;
}

void Item::die()
{
    Destroy();
}

void Item::make_held(bool makeGhostBuilding)
{
    craftTimer = damageTimer = 0.0f;
    halt();
    updatePos = &Item::held_updatePos;
    // make a ghost building when the player picks something up
    Vector2Int cell = get_cell();
    if (makeGhostBuilding) game->Instantiate(ghostBuilding, Vector2(cell.x, cell.y), 1);
}

void Item::make_thrown(Vector2 newVel, Vector2 newAccel)
{
    set_vel(newVel); set_accel(newAccel);
    updatePos = &Item::thrown_updatePos;
    craftTimer = damageTimer = 0.0f;
    playerCollisionTimer = 0.5f;
}

void Item::collision()
{
    if (!has_collision()) return;
    handleCollisionsWithGameObjects();
    collideWithWorldBorders();
    handleCornerCollisionsWithWalls();

    Vector2Int cell = get_cell();
    int num = game->currLevel->grid[cell.x][cell.y];
    if ((num&(WATER|BARRIER)) == (WATER|BARRIER)) {
        if (!is_held()) {
            set_HP(0);
            int idx = rand()%2 + 1;
            game->splashSounds[idx]->play();
            game->Instantiate(splash, get_pos(), 1);
        }
    }
}

void Item::handleCollisionsWithGameObjects()
{
    auto vec = &game->currLevel->gameObjects;

    // iterate through all the other game objects to find the distance between them
    for (int i = 0; i < vec->size(); i++)
    {
        auto other = (*vec)[i];
        // doesn't collide with itself or objects that have no collision
        if (get_idx() == i || !other->has_collision()) continue;
        // held item won't collide with player, other items won't collide with held item
        if (other->get_type() == player) {
            if (is_held() || playerCollisionTimer > 0.0f) continue;
        }
        auto item = std::dynamic_pointer_cast<Item>(other);
        if (item != nullptr) if (item->is_held()) continue;

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
            if (other->is_item()) 
            {
                if (item->get_type() == get_type()) transferItems(item);
                else {
                    int idx = get_idx();
                    if (idx>=0 && idx < vec->size()) 
                        game->craftTwoItems(std::dynamic_pointer_cast<Item>((*vec)[idx]), item);
                }
            }

            // regular collision behaviour
            // normalise the displacement to find the direction the objects need to move
            disp.normalise();
            int sideLen = get_cellSidelen();
            Vector2 newVel = disp * (2.0f * sideLen);

            // if the item is moving, damage the enemy it hit
            if (other->is_enemy()) 
            {
                // make sure that it is moving fast enough
                if (damageTimer <= 0.0f) 
                {
                    // calculate the damage by multiplying the speed by the number of items in the stack
                    int dam = ceilToInt(get_HP() * damage);
                    // damage the enemy
                    other->add_HP(-dam);
                    game->bonk->play();
                    newVel *= clamp(1.0f, 3.0f, (float)dam);
                    damageTimer = damageInterval;
                    set_vel(newVel * -1.0f); set_accel(newVel);
                }

            }
            other->set_vel(newVel);
            other->set_accel(newVel * -1.5f);
        }
    }
}

void Item::transferItems(std::shared_ptr<Item> other)
{
    // add the other stack's item to this stack's, as far as is possible
    // items store their count in their hp variable
    int hp = get_HP();
    int spaceAvailable = get_maxHP() - hp; // how many more items can be held

    int otherHP = other->get_HP();
    // number of items being transfered
    int transfer = min(otherHP, spaceAvailable);

    // transfer the items
    add_HP(transfer);
    other->add_HP(-transfer);
    if (other->get_HP() <= 0) other->die();
}

void Item::collideWithWorldBorders()
{
    Vector2Int map = get_mapDimensions();
    Vector2 pos = get_pos();
    SDL_Rect hitbox = get_hitbox();
    float w = hitbox.w/2, h = hitbox.h/2;

    clamp(w, map.x-w, &pos.x);
    clamp(h, map.y-h, &pos.y);
    
    set_pos(pos);
}

void Item::handleCornerCollisionsWithWalls()
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

void Item::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x - camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);


    Vector2Int cell = get_cell();
    if (game->is_under_tree(cell) && alpha == 255) {
        game->secondRenders.push(this);
    
    } else {
        std::string txt = std::to_string(get_HP());
        auto itemCountTex = std::make_unique<LTexture>(game->window);
        if (!itemCountTex->loadFromRenderedText(txt, {255,255,255,255}, game->arcadeClassic24)) {
            std::cerr << "failed to load health display!" << std::endl;
        }
        itemCountTex->render(p.x+hitbox.w-(itemCountTex->getWidth()/2), p.y+hitbox.h-(itemCountTex->getHeight()/2));
        itemCountTex->free();
    }
}