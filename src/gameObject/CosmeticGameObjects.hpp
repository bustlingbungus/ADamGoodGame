#pragma once

#include "GameObject.hpp"
#include "../engine/GameMath.hpp"

using namespace Math;

class FallingTree : public GameObject
{
    public:

        FallingTree(Vector2 pos, int Idx, Game *game, int cell_sidelen);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        std::shared_ptr<LTexture> tex;
        
        float timer = 1.0f;

        float dir;
};

class GhostBuilding : public GameObject
{
    public:

        GhostBuilding(Vector2Int cell, EntityType itemType, int Idx, Game *game, int cell_sideLen);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        void (GhostBuilding::*Update)() = &GhostBuilding::validate;
        void validate();
        void go_to_mouse_cell();

        void assign_texture(EntityType itemType);

        bool valid = false, show = true;
        std::shared_ptr<LTexture> tex = nullptr;
};

class Item; // forward declaration

class TutorialText : public GameObject
{
    public:

        TutorialText(Vector2 pos, int Idx, Game *game, int cell_sideLen);

        virtual void update();

        virtual void render(int camX, int camY, Uint8 alpha = 255);

    private:

        std::string txt = "";

        Vector2Int treeCell = Vector2Int(24, 23);
        std::shared_ptr<Item> item = nullptr;


        void (TutorialText::*Update)();
        void stage_0_update();
        void stage_1_update();
        void stage_2_update();
        void stage_3_update();
        void stage_4_update();
        void stage_5_update();
        void stage_6_update();
        void stage_7_update();
        void stage_8_update();

        void night_0_update();
        void night_1_update();
        void night_2_update();

        /* functions to check when to update the tutorial stage */
        bool tree_has_been_chopped();
        bool findItem(EntityType itemType, int minHP = 1, bool assignItem = true);
        bool findNearestItem();
};

class Explosion : public GameObject
{
    public:

        Explosion(Vector2 pos, int Idx, Game *game, int cell_sideLen);

        virtual void render(int camX, int camY, Uint8 alpha = 255);

        virtual void update();

    private:

        std::shared_ptr<LTexture> animate(bool updateIdx);
        std::shared_ptr<LTexture> tex = nullptr;
        std::shared_ptr<Animations> animation = nullptr;

        float timer = 1.0f;
};

class Splash : public GameObject
{
    public:

        Splash(Vector2 pos, int Idx, Game *game, int cell_sideLen);

        virtual void render(int camx, int camY, Uint8 alpha = 255);

        virtual void update();

    private:

        std::shared_ptr<LTexture> animate();
        std::shared_ptr<LTexture> tex = nullptr;
        std::shared_ptr<Animations> animation = nullptr;

        float timer = 1.0f;
};