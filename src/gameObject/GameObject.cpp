#include "GameObject.hpp"
#include "../game/Game.hpp"

int get_max_hp(EntityType type)
{
    switch (type)
    {
        case player: return 5;
        case wolf: return 3;
        case logItem: return 6;
        case pineConeItem: return 6;
        case plankItem: return 12;
        case damItem: return 8;
        case doorItem: return 4;
        case stoneItem: return 5;
        case berryItem: return 4;
        case foxNPC: 
        case bearNPC: 
        case rabbitNPC: return 255;
        default: return 1;
    }
}

/* constructor */
GameObject::GameObject(EntityType type, Vector2 pos, int Idx, int Health, Game *game, int cell_sidelen, bool hasCollision, int width, int height)
: type(type), pos(pos), idx(Idx), game(game), cell_sidelen(cell_sidelen), hasCollision(hasCollision)
{
    max_hp = get_max_hp(type);
    hp = (Health == -1)? max_hp : clamp(0, max_hp, Health);

    if (height == -1) height = width;
    Vector2Int size(width, height);
    Vector2Int p = Vector2Int(pos.x, pos.y) - (size/2);
    hitbox = { p.x, p.y, size.x, size.y };
    radius = max(size.x/2, size.y/2);

    set_pos(pos);
}
GameObject::~GameObject() {}

/* accessors */
SDL_Rect GameObject::get_hitbox() { return hitbox; }
float GameObject::get_radius() { return radius; }
int GameObject::get_cellSidelen() { return cell_sidelen; }
int GameObject::get_maxHP() { return max_hp; }
int GameObject::get_HP() { return hp; }
Vector2 GameObject::get_pos() { return pos; }
Vector2 GameObject::get_vel() { return velocity; }
Vector2 GameObject::get_accel() { return acceleration; }
Vector2Int GameObject::get_cell() { return cell; }
bool GameObject::has_collision() { return hasCollision; }
int GameObject::get_idx() { return idx; }
EntityType GameObject::get_type() { return type; }
bool GameObject::is_item() { return type > ITEM_MIN && type < ITEM_MAX; }
bool GameObject::is_enemy() { return type > ENEMY_MIN && type < ENEMY_MAX; }
bool GameObject::is_NPC() { return type > NPC_MIN && type < NPC_MAX; }

float GameObject::get_deltaTime() { return game->deltaTime; }
Vector2Int GameObject::get_mapDimensions() { return Vector2Int(game->map.w, game->map.h); }

/* mutators */
void GameObject::set_hitbox(SDL_Rect newHitbox) { hitbox = newHitbox; radius = hitbox.w/2; }
void GameObject::set_radius(float newRadius) { radius = newRadius; }
void GameObject::set_maxHP(int newMaxHP) { max_hp = newMaxHP; }
void GameObject::set_HP(int newHP) { hp = newHP; }
void GameObject::add_HP(int amount) { hp += amount; }
void GameObject::set_type(EntityType newType) { type = newType; }
void GameObject::decrement_idx() { idx--; }
void GameObject::increment_idx() { idx++; }
void GameObject::set_vel(Vector2 newVel) { velocity = newVel; }
void GameObject::set_accel(Vector2 newAccel) { acceleration = newAccel; }
void GameObject::set_pos(Vector2 newPos) {
    pos = newPos;
    hitbox.x = pos.x - (hitbox.w/2);
    hitbox.y = pos.y - (hitbox.h/2);
    set_cell();
}
void GameObject::set_collision(bool collision) { hasCollision = collision; }

void GameObject::set_cell() {
    cell = Vector2Int(pos.x/cell_sidelen, pos.y/cell_sidelen);
}

void GameObject::update() {}
void GameObject::render(int camX, int camY, Uint8 alpha) {}


void GameObject::defualt_update_position()
{
    Vector2 newPos = pos + velocity * get_deltaTime();
    set_pos(newPos);
}

void GameObject::default_update_velocity()
{
    if (acceleration * velocity > 0.0f) halt();
    velocity += acceleration * get_deltaTime();
}

void GameObject::halt()
{
    acceleration = velocity = Vector2_Zero;
}

void GameObject::Destroy()
{
    int idx = get_idx();
    auto vec = &game->currLevel->gameObjects;
    if (idx >= 0 && idx < vec->size()) game->Destroy((*vec)[idx]);
}