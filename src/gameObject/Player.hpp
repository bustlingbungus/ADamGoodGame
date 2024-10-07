#pragma once

#include "GameObject.hpp"

class Player : public GameObject
{
    public:

        Player(Vector2 pos, int Idx, int Health, Game *game, int cell_sidelen);

        virtual void update();
        virtual void render(int camX, int camY, Uint8 alpha = 255);

        float get_moveSpeed();

        void set_dialogueTimer(float timer);

        virtual void set_HP(int newHP);
        virtual void add_HP(int amount);

    private:

        /* gets which input keys are being held in the game */
        Uint8 get_inputKeys();


        /* how the player updates position */
        void (Player::*updatePos)() = &Player::move_from_user_input;
        void move_from_user_input();

        /* how the player updates velocity */
        void (Player::*updateVelocity)() = &GameObject::default_update_velocity;
        void return_to_shore();


        /* chooses which image to get from the animations object */
        std::shared_ptr<LTexture> animate(bool updateIdx);
        std::shared_ptr<Animations> animations; // the animation object holding all player images
        std::shared_ptr<LTexture> tex; // the image that actually gets rendered

        /* create a dialogue object to send to the game */
        void dialogue();
        float dialogueTimer = 0.0f;

        float moveSpeed;
        
        
        /* collision functions */
        void collision();

        void handleCollisionWithGameObjects();
        bool collideWithWorldBorders();
        void handleCornerCollisionsWithWalls();

        /* for when the player dies */
        void gameOver();
};