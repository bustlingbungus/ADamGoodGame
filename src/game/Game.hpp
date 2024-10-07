#ifndef GAME_HPP
#define GAME_HPP

#include "../engine/LTexture.hpp"
#include "../engine/LTimer.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/GameMath.hpp"
#include "../engine/TextureManipulator.hpp"
#include "../engine/LAudio.hpp"

#include "../gameObject/GameObject.hpp"
#include "../gameObject/Bird.hpp"
#include "../gameObject/Bomb.hpp"
#include "../gameObject/CosmeticGameObjects.hpp"
#include "../gameObject/Item.hpp"
#include "../gameObject/NPC.hpp"
#include "../gameObject/Player.hpp"
#include "../gameObject/Wolf.hpp"
#include "../gameObject/Animations.hpp"

#include "Scene.hpp"
#include "CellTypes.hpp"
#include "Dialogue.hpp"

#include "../Menu/PlayerData.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_events.h>

#include <ctime>
#include <vector>
#include <random>
#include <stack>

using namespace Math;




class Game
{
    // declare GameObject as a friend of Game, so the private members can be accessed
    friend class GameObject;
    friend class Temp;
    friend class Player;
    friend class Wolf;
    friend class Item;
    friend class NPC;
    friend class FallingTree;
    friend class Bird;
    friend class Target;
    friend class Bomb;
    friend class ExplosionIndicator;
    friend class GhostBuilding;
    friend class TutorialText;
    friend class Explosion;
    friend class Splash;

    public:

        // constructor
        Game( std::shared_ptr<LWindow> Window, int resolutionWidth, int resolutionY );

        // deallocate memory
        ~Game();


        // handle events like player input
        void handle_events( SDL_Event& e );

        // sets the inputKeys member to zero
        void clear_input();
        void set_max_framerate( int maxFPS );

        // updates all the game objects
        void update_gameobjects();



        // rendering functions

        void center_camera_on_player();
        // renders the scene's background
        void render_background();
        // renders all the game objects
        void render_gameobjects();
        // renders trees
        void render_overlay();
        // shows how much health each cell has
        void render_cell_health();
        // shows the current frame rate
        void render_framerate();
        // display's the player's hp
        void render_player_health();
        // shows what mouse controls the player can use
        void render_controls();
        // renders the clock that tells how far into the day you are
        void render_clock();
        // renders the brightness levels
        void render_darkness();
        // reders certain game objects a second time, when they are under a tree
        void render_gameobjects_under_trees();
        // renders spoken dialogue
        void render_dialogue();


        // finds the time elapsed between frames
        void update_deltaTime();

        // finds the position of the mouse (on the map)
        Vector2 find_mouse_pos();
        // returns position of the player
        Vector2 get_playerPos();
        
        // adds a game object and returns a pointer to it
        std::shared_ptr<GameObject> Instantiate(EntityType type, Vector2 pos, int hp, Scene *level = NULL);

        // deletes the specified object
        void Destroy(std::shared_ptr<GameObject> obj, std::vector<std::shared_ptr<GameObject>> *vec = NULL);

        // spawns an item stack with random velocity. returns a pointer to the item created
        std::shared_ptr<Item> spawnItemStack( EntityType type, Vector2 pos, int count );

        // when throwing two different item stacks together, they may combine, and produce
        // a third, different item. returns a pointer to the crafted item
        std::shared_ptr<Item> craftTwoItems( std::shared_ptr<Item> item1, std::shared_ptr<Item> item2 );
        // when right clicking an item stack, it may be crafted into another type of item
        std::shared_ptr<Item> craftItem( std::shared_ptr<Item> item );
        // if the player is holding a tradable item, see if they clicked on an entity that trades for it
        bool tradeItem( int heldType, int heldHP, Vector2 mPos );


        // moves the player into the specified level, and makes said level the active scene
        void movePlayerToLevel( Scene *level, Vector2 newPlayerPos );

        // deals a specified amount of damage to a cell
        void damageCell( Vector2Int cell, int damage, Scene *level = NULL, bool playSound = true );

        // toggles day/night, when the appropriate amount of time has passed
        void dayNightCycle();

        // updates any cells dependent on time
        void update_cells();

        // attempts to spawn enemies
        void attempt_enemy_spawn();

        // sets the current dialogue to the specified option
        void enter_dialogue( Dialogue newDialogue );

        // if the player is currently in dialogue, this function will take player input to see which dialogue option they select
        void handle_dialogue( int e );

        // saves all scenes to file
        void save_game();

        // loads levels from files, chooses one to be the active level
        void load_levels( std::string dir );
        // determines some miscellaneous data about the game state
        void load_gameData( std::string filename );
        void save_gameData( std::string filename );

        // renders every cell in the level, to be used when loading levels
        void initialise_BGTexture();
        void createDarknessTex();

        // returns g_time
        float get_time();
        float get_deltaTime();
        // returns gameOver
        bool game_over();
        // makes game_over false
        void new_game(bool show_tutorial);

        // toggles pause
        void set_paused(bool isPaused);

        // stops music from playing
        void stop_music();
        // plays the current song
        void play_current( bool enteringMenu = false );


        // tracks the player's score
        PlayerData scores;
        // where in the window to render the game
        Vector2Int renderOffset;


    private:

        // returns true if the type represents an Item
        bool is_item( int type );
        // returns true if the specified cell is a barrier
        bool is_barrier( Vector2Int cell, Scene *level = NULL );
        // returns true if there is some building in the cell
        bool is_occupied( Vector2Int cell, Scene *level = NULL );


        // for tracking player input
        // tracks which keys the player has held down
        Uint8 inputKeys = 0;
        // determines how far away the player can interact with things
        float interactRange = 0.0f;

        // tracks which dialogue (if any) the player is currently engaged in
        Dialogue currDialogue = None;



        // dimensions of the current map
        SDL_Rect map;


        // the time elapsed between frames
        float deltaTime = 0.0f;
        clock_t begin_time = 0; // used for calculating deltaTime
        int fps;


        // tracking the time of day/night
        float g_time = 0.0f;
        // an enemy will spawn every x seconds
        float enemySpawnRate = 4.0f, lastSpawn = 0.0f;


        // whether or not it is currently night time
        bool isNight, isPaused = false;

        // whether or not stone may be gathered from the riverbed
        bool riverDammed = false;

        // true when the player dies
        bool gameOver = false;
        // if this is the first day in the session
        bool firstDay = true;
        // whether or no to show the tutorial text
        bool showTutorial = true;


        // position and dimensions of the camera
        SDL_Rect camera;


        // all the levels in the game
        Scene *currLevel;
        Scene Base, Woods, Town;
        // true when switching between levels
        bool switching_scenes = false;

        // pointer to the npc the player is currently speaking to
        std::shared_ptr<NPC> currNPC = nullptr;

        // game objects that will be rendered a second time
        std::stack<GameObject*> secondRenders;
        // all dialogue to be rendered
        std::stack<std::shared_ptr<DialogueRender>> dialogueRenders;









        /* --------  constants:  -------- */

        // the highest framerate you can achieve, -1 for uncapped
        float MIN_DELTATIME = -1.0f; // 120fps

        // the length (in seconds) of one day
        float DAY_LENGTH = 240.0f;
        // the amount of time it takes to transistion from day to night
        float DAY_TRANSITION_TIME = 10.0f;
        int barrier = BARRIER;

        // the radius (in cells) of the bomb's explosion
        float BOMB_RADIUS = 3.0f;
        // how much damage the bomb does upon detonation
        int BOMB_DAMAGE = 3;
        // the game will attempt an enemy spawn every x seconds
        float MAX_SPAWN_RATE = 0.5f, START_SPAWN_RATE = 4.0f;
        // how much the spawn rate decreases each day
        float DIFF_SCALING = 0.5f;
        // the amount of time (int seconds) it takes for birds to fly across the map
        float BIRD_FLIGHT_DURATION = 3.0f;
        // the number of times each day or night buildings take damage from water
        int WATER_DAMAGE_INTERVAL = 2;
        // multiplies the amount of damage taken by buildings placed in water
        float WATER_DAMAGE_MULT = 0.5f;

        // the probability of spawning a stone item in each cell when clearing the river
        float STONE_SPAWN_CHANCE = 0.075f;
        // chances for an enemy to spawn each successful spawn attempt
        float BIRD_SPAWN_CHANCE = 0.25f;
        // currently, wolf spawn chance will be 1 - this ^^

        // how fast an object needs to be going (in number of cells/sec) to deal damage
        float ITEM_MINIMUM_DAMAGE_VELOCITY = 1.0f;
        // multiplier that alters the speed of thrown items
        float THROWN_ITEM_SPEED_MULT = 1.15f;

        // dimensions of trees
        int TREE_HEIGHT = 9, TREE_WIDTH = 3;
        











        // images/textures

        // used for editing textures
        TextureManipulator tEditor;

        // loads all of the games textures from file
        void load_textures();
        // loads textures for all the animation containers
        void load_animations();
        // loads all of the audio mp3s from file
        void load_audio();
        // loads all the fonts from file
        void load_fonts();
        // creates some miscellaneous textures
        void create_textures();

        


        // background texture
        std::shared_ptr<LTexture> BGTexture = nullptr, overlayTexture = nullptr;
        std::shared_ptr<LTexture>   logTex = nullptr, damTex = nullptr, waterTex = nullptr, grassTex = nullptr, treeTex = nullptr,
                                    stumpTex = nullptr, saplingTex = nullptr, playerTex = nullptr, wolfTex = nullptr, falling_treeTex = nullptr,
                                    pine_coneTex = nullptr, plankTex = nullptr, foxTex = nullptr, berryTex = nullptr,
                                    shoreline0Tex = nullptr, shoreline1Tex = nullptr, shoreline2Tex = nullptr, shoreline3Tex = nullptr,
                                    shoreline4Tex = nullptr, closed_doorTex = nullptr, open_doorTex = nullptr, dirtTex = nullptr,
                                    berry_bushTex = nullptr, empty_bushTex = nullptr, heartTex = nullptr,
                                    stoneTex = nullptr, LMBTex = nullptr, MMBTex = nullptr, RMBTex = nullptr, BearTex = nullptr, BirdTex = nullptr, BombTex = nullptr,
                                    TargetTex = nullptr, dashed_circleTex = nullptr, rabbitTex = nullptr;

        // animation containers
        std::shared_ptr<Animations> playerAnimations = nullptr, wolfWalkingAnimation = nullptr, 
                                    explosionAnimation = nullptr, splashAnimation = nullptr;

        // all of the audio used by the game
        std::shared_ptr<LAudio> logDestruction = nullptr, treeFalling = nullptr, doorToggle = nullptr, leaves = nullptr, pop = nullptr, 
                                bonk = nullptr, birdSpawn = nullptr, playerDamage = nullptr, healSound = nullptr, 
                                gameOverSound = nullptr, explosionSound = nullptr, repairSound = nullptr,
                                // dialogue sounds
                                vocalDeep0 = nullptr, vocalDeep1 = nullptr, vocalMed0 = nullptr, 
                                vocalMed1 = nullptr, vocalHigh0 = nullptr, vocalHigh1 = nullptr;

        std::shared_ptr<LAudio> wolfSpawnSounds[3], splashSounds[3];

        // music
        std::shared_ptr<LAudio> nightMusic = nullptr, dayMusic = nullptr, deathMusic = nullptr, menuMusic = nullptr;
        std::shared_ptr<LAudio> currSong = nullptr;

        // fonts
        TTF_Font *sevenSegment = nullptr, *arcadeClassic24 = nullptr, *arcadeClassic36 = nullptr;
        
        // the window the game will be rendered to
        std::shared_ptr<LWindow> window = nullptr;




        // misc pointers used for rendering
        std::unique_ptr<LTexture>   fpsTex = nullptr, controlsTex = nullptr, clockTex = nullptr;

        std::shared_ptr<LTexture>   clockBackBar = nullptr, redBar = nullptr, whiteBar = nullptr,
                                    darknessTex = nullptr;






        // functions for editing the grid (defined in Grid.cpp NOT Game.cpp!!!!)

        // updates any cells dependent on time in a level
        void update_cells( Scene *level );

        // place an object into a cell in the global grid
        int PlaceObjectInCell(Vector2Int cell, int objType, bool playerPlacement, Scene *level = NULL);
        // updates the background texture for rendering
        void drawCell( Vector2Int cell );


        // renders a tree texture into the overlay texture
        void AddTreeToOverlay( Vector2Int cell );
        // removes a tree from the overlay texture
        void RemoveTreeFromOverlay( Vector2Int cell );

        // fills a cell with the water texture, as well as any shoreline
        void DrawWaterToCell( Vector2Int cell, SDL_Rect cellRect );




        // returns true if the coordinates are within the rect
        bool isInRegion( Vector2 p, SDL_Rect rect );

        // returns the building corresponding to a given item type
        int get_building( EntityType type );

        // if the player is holding an object, throw it, otherwise try to pick one up
        void leftClickFunc();
        // if the player IS holding an item, attempt to place it, otherwise, do nothing (for now c:)
        void rightClickFunc();



        // sets the selected object to be held by the player
        void setHeldObject( std::shared_ptr<Item> obj );
        // throw the object the player is currently holding
        void throwHeldObject();
        // throw one of the item the player is currently holding
        void throwSingleItem();

        // spawns a wolf at a random location on the maps edge
        std::shared_ptr<GameObject> spawnWolf();
        std::shared_ptr<GameObject> spawnBird();

        // checks if the river has been walled off, and removes water from the subsequent tiles
        void damRiver();

        // recursive, returns true if the river is blocked
        bool blocksRiver( Vector2Int cell );

        // removes water from all the cells to the right of the specified one
        void removeWaterFollowingCell( Vector2Int cell );

        // spawns in NPCs at the beginning of each day
        void spawnNPCs();

        bool is_under_tree( Vector2Int cell );



        void render_held_object_controls( Vector2 mPos, SDL_Rect *mRect );
        void render_NPC_trade_controls( int type, SDL_Rect *mRect, bool *flag );
        void render_BuildThrow_commands( Vector2 mPos, SDL_Rect *mRect );
        bool render_hovering_over_entity_controls( Vector2 mPos, SDL_Rect *mRect );
        void render_crafting_controls( int type, SDL_Rect *mRect, int hp );
        void render_cell_controls( Vector2 mPos, SDL_Rect *mRect );
};

#endif