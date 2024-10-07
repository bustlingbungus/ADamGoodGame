#include "NPC.hpp"
#include "../game/Game.hpp"

NPC::NPC(int type, Vector2 pos, int Idx, Game *game, int cell_sidelen)
: GameObject((EntityType)type, pos, Idx, 1, game, cell_sidelen, false, cell_sidelen-2)
{
    switch (type)
    {
        case foxNPC:
            tex = game->foxTex;
            updatePos = &NPC::foxPosition;
            updateVel = &NPC::foxVelocity;
            dialogue = &NPC::foxDialogue;
            moveSpeed = 2.66666667f * cell_sidelen;
            speechSound = game->vocalMed0;
            break;

        case bearNPC:
            SDL_Rect hitbox = get_hitbox();
            hitbox.w = hitbox.h = (3*cell_sidelen)-2;
            set_hitbox(hitbox);
            set_collision(true);
            tex = game->BearTex;
            updatePos = &NPC::bearPosition;
            updateVel = &GameObject::halt;
            dialogue = &NPC::bearDialogue;
            moveSpeed = 0.0f;
            speechSound = game->vocalDeep0;
            break;

        case rabbitNPC:
            tex = game->rabbitTex;
            updatePos = &NPC::rabbitPosition;
            updateVel = &NPC::foxVelocity;
            dialogue = &NPC::rabbitDialogue;
            moveSpeed = 2.66666667f * cell_sidelen;
            speechSound = game->vocalHigh0;
            break;
    }
}

void NPC::set_HP(int newHP) {
    GameObject::set_HP(newHP);
    if (newHP > 1) play_speech();
    else maySpeak = true;
}

void NPC::add_HP(int amount) {
    set_HP(get_HP()+amount);
}

void NPC::play_speech() { if (speechSound != nullptr && maySpeak) speechSound->play(); }

void NPC::set_dialogueTimer(float newTimer) { dialogueTimer = newTimer; }

float NPC::get_dialogueInterval() { return dialogueInterval; }

void NPC::update()
{
    

    (this->*updatePos)();
    (this->*updateVel)();
    (this->*dialogue)();
    collision();

    if (get_HP() == 0) GameObject::set_HP(1);
}

void NPC::foxPosition()
{
    // atGameObjectt to move back to a point
    Vector2 pos = get_pos();
    Vector2 target = (game->currLevel == &game->Base)? Vector2(2000.0f, 2600.0f) : Vector2(913.0f, 345.0f);
    Vector2 dist = target - pos;
    Vector2 vel = Vector2_Zero;

    if (dist.length() > 1.0f * game->currLevel->cell_sideLen) {
        dist.normalise();
        vel = dist * moveSpeed;
    }
    float dt = get_deltaTime();

    Vector2 p = pos + (get_vel()+vel) * dt;
    set_pos(p);
}

void NPC::foxVelocity()
{
    if (game->isNight) beginRetreat();
    else {
        if (!has_collision()) {
            Vector2Int cell = get_cell();
            if (!game->is_barrier(cell)) {
                set_collision(true);
            }
        }
        default_update_velocity();
    }
}

void NPC::beginRetreat()
{
    GameObject::set_HP(1);
    // turn off collision and switch to retreat function
    set_collision(false);
    // start with some velocity towards the centre of the map
    Vector2Int map = get_mapDimensions();
    Vector2 mCentre(map.x/2, map.y/2);
    set_vel(getUnitVector(get_pos(), mCentre) * 300.0f);
    // enter the retreat function
    updateVel = &NPC::retreat;
    updatePos = &GameObject::defualt_update_position;
}

void NPC::retreat()
{
    // move away from the centre of the map until out of bounds
    Vector2Int map = get_mapDimensions();
    Vector2 mCentre(map.x/2, map.y/2);

    Vector2 pos = get_pos(), dir = getUnitVector(mCentre, pos);
    // in case the entity is on the map's centre
    if (dir == Vector2_Zero) dir = Vector2_One;

    // accelerate off the map
    set_accel(dir * 1500.0f);
    set_vel(get_vel() + get_accel()*get_deltaTime());

    // destroy itself when it goes out of bounds
    if (pos.x != clamp(0.0f, (float)map.x, pos.x) || pos.y != clamp(0.0f, (float)map.y, pos.y)) {
        Destroy();
    }
}

void NPC::bearPosition()
{
    Vector2 newPos(1860.0f, 300.0f);
    set_pos(newPos);
}

void NPC::rabbitPosition()
{
    // atGameObjectt to move back to a point
    Vector2 pos = get_pos();
    Vector2 target = Vector2(540.0f, 840.0f);
    Vector2 dist = target - pos;
    Vector2 vel = Vector2_Zero;

    if (dist.length() > 1.0f * game->currLevel->cell_sideLen) {
        dist.normalise();
        vel = dist * moveSpeed;
    }
    float dt = get_deltaTime();

    Vector2 p = pos + (get_vel()+vel) * dt;
    set_pos(p);
}

void NPC::collision()
{
    if (!has_collision()) return;

    handleCollisionWithGameObjects();
    collideWithWorldBorders();
    handleCornerCollisionsWithWalls();
}

void NPC::handleCollisionWithGameObjects()
{
    // get the vector of game objects in the current level
    auto vec = &game->currLevel->gameObjects;

    // iterate through all the other game objects to find the distance between them

    for (int i = 0; i < vec->size(); i++)
    {
        auto other = (*vec)[i];

        // doesn't collide with itself or objects that have no collision
        if (get_idx() == i || !other->has_collision()) continue;

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

void NPC::collideWithWorldBorders()
{
    Vector2Int map = get_mapDimensions();
    Vector2 pos = get_pos();
    SDL_Rect hitbox = get_hitbox();
    float w = hitbox.w/2, h = hitbox.h/2;
    
    clamp(w, map.x-w, &pos.x);
    clamp(h, map.y-h, &pos.y);

    set_pos(pos);
}

void NPC::handleCornerCollisionsWithWalls()
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

void NPC::render(int camX, int camY, Uint8 alpha)
{
    // std::cout << "rendering NPC\n";
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    Vector2Int cell = get_cell();
    bool underTree = game->is_under_tree(cell), alpha255 = alpha == 255;

    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);

    if (underTree && alpha255) {
        game->secondRenders.push(this);
    } 
    // std::cout << "rendered NPC\n";
}