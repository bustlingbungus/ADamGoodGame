#pragma once

#include "GameObject.hpp"

namespace AStar { class LinkedCell; }

class Wolf : public GameObject
{
    public:

        Wolf(Vector2 pos, int Idx, int Health, Game *game, int cell_sidelen, std::shared_ptr<LAudio> deathSound);
        ~Wolf();

        virtual void update();
        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        std::shared_ptr<LTexture> animateWalking(bool updateIdx);
        std::shared_ptr<Animations> walkingAnimations; // the animation object holding all wolf images
        std::shared_ptr<LTexture> tex; // the image that actually gets rendered

        std::shared_ptr<LAudio> deathSound = nullptr;

        float moveSpeed;

        float attackInterval = 1.0f, attackTimer = 0.0f;
        float pathfindInterval = 0.5f, pathfindTimer = 0.0f, collisionPathfindTimer = 0.0f;
        int damage = 1;

        void die();

        /* how the wolf moves */
        void (Wolf::*updatePos)() = &GameObject::defualt_update_position;

        /* how the wolf updates velocity */
        void (Wolf::*updateVel)() = &Wolf::spawn;
        void spawn();
        void retreat();
        void chase_player();
        void moveDirectlyToPlayer();
        void JumpAttack();

        /* pathfinding functions */
        AStar::LinkedCell *path = nullptr;
        int pathfindingLength = 0;
        void updatePathfinding();
        void pathfindToPlayer(int depth = 100);
        void walkToNextCell();
        void clearPath();

        /* collision functions */
        void collision();
        void handleCollisionsWithGameObjects();
        void collideWithWorldBorders();
        void handleCornerCollisionsWithWalls();


        /* transitions between behaviour functions */
        void beginRetreat();
        void enterJumpAttack();
};