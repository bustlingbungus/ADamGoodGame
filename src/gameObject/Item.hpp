#pragma once

#include "GameObject.hpp"

class Item : public GameObject
{
    public:

        Item(int Type, Vector2 pos, int idx, int count, Game *game, int cell_sideLen);

        virtual void update();
        virtual void render(int camX, int camY, Uint8 alpha = 255);

        bool is_held();

        int get_damage();
        float get_damageMult();
        float get_craftTimer();
        void set_craftTimer();
        float get_playerCollisionTimer();

        void make_held(bool makeGhostBuilding = true);
        void make_thrown(Vector2 newVel, Vector2 newAccel);

    private:

        std::shared_ptr<LTexture> tex;

        void die();

        /* updating position */
        void (Item::*updatePos)() = &GameObject::defualt_update_position;
        void held_updatePos();
        void thrown_updatePos();

        /* updating velocity */
        void (Item::*updateVel)() = &Item::spawn;
        void spawn();

        /* collision */
        void collision();
        void handleCollisionsWithGameObjects();
        void collideWithWorldBorders();
        void handleCornerCollisionsWithWalls();

        /* crafting functions */
        void transferItems(std::shared_ptr<Item> other);


        float craftInterval = 0.75f, craftTimer = craftInterval;
        float damage;
        float damageInterval = 1.0f, damageTimer = damageInterval;
        float playerCollisionTimer = 0.0f;
};