#pragma once

#include "GameObject.hpp"

class ExplosionIndicator : public GameObject
{
    public:

        ExplosionIndicator(Vector2 pos, int Idx, Game *game, int cell_sideLen);
        
        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        std::shared_ptr<LTexture> tex;
};

class Bomb : public GameObject
{
    public:

        Bomb(Vector2 pos, int Idx, Game *game, int cell_sideLen);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        std::shared_ptr<LTexture> tex;

        /* for updating velocity */
        void (Bomb::*updateVel)() = &Bomb::fall;
        void fall();

        void explode();

        float speed, timer;

        std::shared_ptr<ExplosionIndicator> indicator;
};
