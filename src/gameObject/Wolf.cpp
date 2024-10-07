#include "Wolf.hpp"
#include "Animations.hpp"
#include "../game/Game.hpp"
#include "AStarPathfinding.hpp"

Wolf::Wolf(Vector2 pos, int Idx, int Health, Game *game, int cell_sidelen, std::shared_ptr<LAudio> deathSound)
:   GameObject(wolf, pos, Idx, Health, game, cell_sidelen, false, cell_sidelen-2), 
    deathSound(deathSound),
    walkingAnimations(game->wolfWalkingAnimation)
{
    moveSpeed = 3.2f * cell_sidelen;
}
Wolf::~Wolf() { clearPath(); }

void Wolf::update()
{
    (this->*updatePos)();
    (this->*updateVel)();
    collision();

    if (get_HP() <= 0) die();
    else set_HP(clamp(0, get_maxHP(), get_HP()));
}

void Wolf::spawn()
{
    Vector2Int cell = get_cell();
    if (game->is_barrier(cell))
    {
        Vector2Int dest = get_mapDimensions()/2;
        Vector2 dir = getUnitVector(get_pos(), Vector2(dest.x, dest.y));
        set_vel(dir * 80.0f);
    }
    else
    {
        Vector2Int dimensions = game->currLevel->gridDimensions;
        if (cell.x == clamp(2, dimensions.x-2, cell.x) && cell.y == clamp(2, dimensions.y-2, cell.y)) {
            set_collision(true);
            updateVel = &Wolf::chase_player;
        }
    }
}

void Wolf::die()
{
    game->scores.mostEnemiesKilled++;
    game->scores.calculate_score();
    if (deathSound != nullptr) deathSound->play();
    Destroy();
}

void Wolf::chase_player()
{
    if (!game->isNight) beginRetreat();
    else 
    {
        if (pathfindTimer <= 0.0f || path == nullptr) updatePathfinding();

        if (pathfindingLength < 5)
        {
            Vector2 distToPlayer = game->get_playerPos() - get_pos();

            // set the pathfinding to a nullptr, so it's not stuck in this case forever
            // if the wolf is super close, just go straight for them
            if (distToPlayer.length() <= 2.0f * get_cellSidelen() || pathfindingLength < 2) 
            {
                clearPath();
                damage = 1;
                moveDirectlyToPlayer();
            }
            else if (attackTimer <= 0.5f) enterJumpAttack();
        }

        float dt = get_deltaTime();
        attackTimer -= dt; pathfindTimer -= dt; collisionPathfindTimer -= dt;

        if (path != nullptr) walkToNextCell();
    }
}

void Wolf::moveDirectlyToPlayer()
{
    Vector2 dir = getUnitVector(get_pos(), game->get_playerPos());
    set_vel((dir*moveSpeed) + (get_accel()*get_deltaTime()));
}

void Wolf::enterJumpAttack()
{
    clearPath();
    attackTimer = 2.75f;
    set_accel(get_vel() * -2.0f);
    updateVel = &Wolf::JumpAttack;
}

void Wolf::JumpAttack()
{
    float dt = get_deltaTime();
    Vector2 pos = get_pos(), vel = get_vel(), accel = get_accel();

    if (accel == Vector2_Zero && attackTimer <= 2.0f && attackTimer+dt > 2.0f)
    {
        Vector2 pPos = game->get_playerPos();
        Vector2 dir = getUnitVector(pos, pPos);

        // give the wolf a bunch of velocity in that direction,
        // with some negative acceleration
        vel = dir * (7.0f * get_cellSidelen());
        accel = dir * -300.0f;

        // wolf deals increased damage during this attack
        damage = 2;
    
    // when the wolf comes to a stop, go back to the regular pathfinding
    } else if (vel * accel > 0.0f) accel = vel = Vector2_Zero;
    // spend a second cooling down
    else if (attackTimer <= 0.0f)
    {
        attackTimer = 1.0f;
        // set the velocity back to regular pathfinding
        updatePathfinding();
        updateVel = &Wolf::chase_player;
        return;
    }


    // add acceleration to velocity
    vel += accel * dt;
    // decrease the timer
    attackTimer -= dt;

    set_vel(vel); set_accel(accel);
}

void Wolf::beginRetreat()
{
    set_collision(false);
    halt();
    Vector2Int map = get_mapDimensions();
    Vector2 mCentre(map.x/2, map.y/2);
    set_vel(getUnitVector(get_pos(), mCentre) * 300.0f);
    updateVel = &Wolf::retreat;
}

void Wolf::retreat()
{
    Vector2Int map = get_mapDimensions();
    Vector2 mCentre(map.x/2, map.y/2);

    Vector2 dir = getUnitVector(mCentre, get_pos());
    // in case the entity is on the map's centre
    if (dir == Vector2_Zero) dir = Vector2_One;

    Vector2 accel = dir * 1500.0f;
    set_accel(accel);
    set_vel(get_vel() + accel*get_deltaTime());

    Vector2 pos = get_pos();
    if (pos.x != clamp(0.0f, (float)map.x, pos.x) || pos.y != clamp(0.0f, (float)map.y, pos.y)) {
        Destroy();
    }
}

void Wolf::updatePathfinding()
{
    if (path != nullptr) 
    {
        // save the current pathfinding cell
        Vector2Int currCell = path->cell;

        // clean up all existing pathfinding nodes
        clearPath();

        // update pathfinding
        pathfindToPlayer();

        pathfindTimer = pathfindInterval;
    }
    else pathfindToPlayer();
}

void Wolf::pathfindToPlayer(int depth)
{
    AStar::Open(&game->currLevel->grid, &game->barrier);

    // create a new pathfinding object
    Vector2Int  start = get_cell(), 
                end = game->currLevel->player->get_cell(),
                dimensions(game->currLevel->gridDimensions.x, game->currLevel->gridDimensions.y);

    AStar::Pathfinding pathfinding(start, end, dimensions, depth);

    // find the best path to the player
    AStar::LinkedCell *node = pathfinding.findPath();
    // reverse the list to be start to end
    path = AStar::ReverseLinkedList(node);
    pathfinding.close();

    // count the number of nodes in the linked list
    int count = 0;
    for (AStar::LinkedCell *curr = path; curr != nullptr; curr = curr->next) {
        count++;
    }
    pathfindingLength = count;
}

void Wolf::walkToNextCell()
{
    // try to get to the centre of the first pathfinding cell
    int sideLen = get_cellSidelen();
    Vector2Int cell = path->cell;
    Vector2 cellPos(((float)cell.x+0.5f)*sideLen, ((float)cell.y+0.5f)*sideLen);

    // get the displacement form the wolf to the centre of the next cell
    Vector2 disp = cellPos - get_pos();
    Vector2 dir = Vector2_Zero;
    float len = disp.length();

    // if it is close enough (within one unit), progress to the next node
    // otherwise, move towards the cell centre
    if (len <= 5.0f) {
        // make the position exact
        set_pos( cellPos );
        // progress to the next node
        path = path->next;
    } else {
        // move towards the cell's centre
        dir = disp / len;
    }

    Vector2 vel = get_vel(), accel = get_accel();
    if (vel * accel > 0.0f) accel = Vector2_Zero;
    vel = (dir * moveSpeed) + (accel * get_deltaTime());
    set_accel(accel); set_vel(vel);
}

void Wolf::clearPath()
{
    AStar::LinkedCell *curr = path;
    for (AStar::LinkedCell *next = curr; curr != nullptr; curr = next) {
        next = curr->next;
        delete curr;
    }
    path = nullptr;
}

void Wolf::collision()
{
    if (!has_collision()) return;

    handleCollisionsWithGameObjects();
    collideWithWorldBorders();
    handleCornerCollisionsWithWalls();

    // die if in water
    Vector2Int cell = get_cell();
    int num = game->currLevel->grid[cell.x][cell.y];
    if ((num&(WATER|BARRIER)) == (WATER|BARRIER)) {
        set_HP(0);
        int idx = rand()%2 + 1;
        game->splashSounds[idx]->play();
        game->Instantiate(splash, get_pos(), 1);
    } 
}

void Wolf::handleCollisionsWithGameObjects()
{
    auto gameObjects = &game->currLevel->gameObjects;

    // iterate through all the other game objects to find the distance between them
    for (int i = 0; i < gameObjects->size(); i++)
    {
        auto other = (*gameObjects)[i];
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
            Vector2 newVel = disp * (1.0f * get_cellSidelen());

            // damage non-enemies
            if (!other->is_enemy() && attackTimer <= 0.0f)
            {
                newVel *= float(damage + 1);
                other->add_HP(-damage);
                attackTimer = attackInterval;
            }
            else if (other->get_type() == wolf && collisionPathfindTimer <= 0.0f)
            {
                if (path != nullptr) path = path->next;
                collisionPathfindTimer = pathfindInterval;
            }
            other->set_vel(newVel);
            other->set_accel(newVel * -1.5f);
        }
    }
}

void Wolf::collideWithWorldBorders()
{
    Vector2Int map = get_mapDimensions();
    Vector2 pos = get_pos();
    SDL_Rect hitbox = get_hitbox();
    float w = hitbox.w/2, h = hitbox.h/2;
    
    clamp(w, map.x-w, &pos.x);
    clamp(h, map.y-h, &pos.y);

    set_pos(pos);
}

void Wolf::handleCornerCollisionsWithWalls()
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
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) {
            game->damageCell(cell, damage);
            attackTimer = attackInterval;
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
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) {
            game->damageCell(cell, damage);
            attackTimer = attackInterval;
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
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) {
            game->damageCell(cell, damage);
            attackTimer = attackInterval;
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
        bool flag = false;
        if (absf(disp.x)<absf(disp.y)) {
            hitbox.x += disp.x; pos.x = hitbox.x + hitbox.w/2;
            if (velocity.x != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.x = acceleration.x = 0.0f;
            }
        } else {
            hitbox.y += disp.y; pos.y = hitbox.y + hitbox.h/2;
            if (velocity.y != 0.0f) {
                flag = attackTimer <= 0.0f;
                velocity.y = acceleration.y = 0.0f;
            }
        }
        if (flag) {
            game->damageCell(cell, damage);
            attackTimer = attackInterval;
        } 
    }

    set_pos(pos); set_vel(velocity); set_accel(acceleration);
}

void Wolf::render(int camX, int camY, Uint8 alpha)
{
    SDL_Rect hitbox = get_hitbox();
    Vector2Int p( hitbox.x-camX, hitbox.y-camY );
    // not within the camera's view, don't render
    if (p.x != clamp(game->renderOffset.x-hitbox.x, game->camera.w+game->renderOffset.x, p.x) || p.y != clamp(game->renderOffset.y-hitbox.h, game->camera.h+game->renderOffset.y, p.y)) {
        return;
    }
    Vector2Int cell = get_cell();
    bool underTree = game->is_under_tree(cell), alpha255 = alpha == 255;

    tex = animateWalking(!alpha255 || (alpha255 && !underTree));

    tex->setAlpha(alpha);
    tex->render(p.x, p.y, &hitbox);
    tex->setAlpha(255);


    int hp = get_HP(), max_hp = get_maxHP();
    if (underTree && alpha255) {
        game->secondRenders.push(this);
    } 
    // show hp
    else if (hp < max_hp && hp > 0) {
        float t = (float)hp / (float)max_hp;
        int wWhite = hitbox.w*t, wRed = hitbox.w * (1.0f-t);

        auto white = game->tEditor.createSolidColour(wWhite, 20, 0xFFFFFFFF, game->window);
        auto red   = game->tEditor.createSolidColour(wRed, 20, 0xFF0000FF, game->window);

        SDL_Rect wRect = {p.x, p.y, wWhite, 20}, rRect = {p.x+wWhite, p.y, wRed, 20};

        white->render(wRect.x, wRect.y, &wRect);
        red->render(rRect.x, rRect.y, &rRect);

        white->free(); red->free();
    }
}

std::shared_ptr<LTexture> Wolf::animateWalking(bool updateIdx)
{
    FacingDirection look = Default; float idx = -1.0f;

    if (get_vel() == Vector2_Zero)
    {
        Vector2 dir = getUnitVector(get_pos(), game->get_playerPos());

        if (dir.y >= SIN45) look = Forwards;
        else if (dir.y <= -SIN45) look = Backwards;
        else if (dir.x <= 0.0f) look = Left;
        else look = Right;

        idx = 0.0f;
    }
    else
    {
        Vector2 dir = get_vel().normalised();

        if (dir.y >= SIN45) look = Forwards;
        else if (dir.y <= -SIN45) look = Backwards;
        else if (dir.x <= 0.0f) look = Left;
        else look = Right;
    }

    return walkingAnimations->getTexture(get_deltaTime(), look, idx, updateIdx);
}