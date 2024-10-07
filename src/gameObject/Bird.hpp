#pragma once

#include "GameObject.hpp"

class Bird : public GameObject
{
    public:

        Bird(Vector2 target, int Idx, Game *game, int cell_sidelen);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

        void getInterps(float *t, float *target_t);

        Vector2 get_target();

        bool has_dropped_bomb();

    private:

        std::shared_ptr<LTexture> tex;

        float timer;

        bool hasDroppedBomb = false;

        Vector2 targetPos;
};

class Target : public GameObject
{
    public:

        Target(Bird *bird, int Idx);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        Bird *bird;
        std::shared_ptr<LTexture> tex;
};