#pragma once

#include "../engine/GameMath.hpp"
#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/LAudio.hpp"

#include "Animations.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <string>

// forwards declarations (i hate ts)
class Game;

using namespace Math;


// entity types
enum EntityType
{
    player,             // 0

    // enemy types
    ENEMY_MIN,
    wolf,
    bird,
    ENEMY_MAX,          

    bomb,
    
    // end of enemy types

    fallingTree,
    target,
    bombExplosionIndicator,
    ghostBuilding,
    tutorialText,
    explosion,
    splash,

    // item types
    ITEM_MIN,
    logItem,       
    pineConeItem, 
    plankItem,     
    damItem,    
    doorItem,      
    stoneItem,     
    berryItem,
    ITEM_MAX,

    // end of item types

    // NPC types
    NPC_MIN,
    foxNPC,
    bearNPC,
    rabbitNPC,
    NPC_MAX
};

int get_max_hp(EntityType type);

class GameObject
{
    public:

        GameObject(EntityType type, Vector2 pos, int Idx, int Health, Game *game, int cell_sidelen, bool hasCollision, int width, int height = -1);
        ~GameObject();

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);


        // accessors
        SDL_Rect get_hitbox();
        float get_radius();
        int get_cellSidelen();
        int get_maxHP();
        int get_HP();
        Vector2 get_pos();
        Vector2 get_vel();
        Vector2 get_accel();
        Vector2Int get_cell();
        bool has_collision();
        int get_idx();
        bool is_item();
        bool is_enemy();
        bool is_NPC();
        EntityType get_type();

        // access game variables
        float get_deltaTime();
        Vector2Int get_mapDimensions();

        // mutators
        void set_hitbox(SDL_Rect newHitbox);
        void set_radius(float newRadius);
        void set_maxHP(int newMaxHP);
        virtual void set_HP(int newHP);
        virtual void add_HP(int amount);
        void set_vel(Vector2 newVel);
        void set_pos(Vector2 newPos);
        void set_accel(Vector2 newAccel);
        void set_collision(bool collision);
        void set_type(EntityType newType);
        void decrement_idx();
        void increment_idx();

        Game *game = nullptr;

        // sets velocity and acceleration to zero
        void halt();
        // removes itself from the game
        void Destroy();

        void default_update_velocity();
        void defualt_update_position();

    private:

        void set_cell();

        EntityType type;
        Vector2 pos, velocity, acceleration;
        Vector2Int cell;
        int hp, max_hp;
        int idx;
        SDL_Rect hitbox;
        float radius;

        int cell_sidelen;
        bool hasCollision;

};