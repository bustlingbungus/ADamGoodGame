#ifndef SCENE_HPP
#define SCENE_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "../engine/GameMath.hpp"

#include "../gameObject/GameObject.hpp"
#include "../gameObject/Bird.hpp"
#include "../gameObject/Bomb.hpp"
#include "../gameObject/CosmeticGameObjects.hpp"
#include "../gameObject/Item.hpp"
#include "../gameObject/NPC.hpp"
#include "../gameObject/Player.hpp"
#include "../gameObject/Wolf.hpp"

using namespace Math;

// forward declaration of GameObject class
class GameObject;
class Game;


// a class to store all the information about a level
class Scene
{
    public:

        // loads a scene from file
        Scene( std::string filePath, Game *game );
        Scene();

        ~Scene();

        void free();


        // saves to scene in the specified directory
        void Save( std::string directory );

        // assigns the pointers to other scenes
        void assignNeighbours(
            Scene *Above = nullptr,
            Scene *Below = nullptr,
            Scene *Left = nullptr,
            Scene *Right = nullptr
        );


        std::string name = "New Scene";

        // game objects
        std::shared_ptr<Player> player = nullptr; // pointer to the player
        std::shared_ptr<Item> held = nullptr; // pointer to an object held by the player

        // vector of all the game objects
        std::vector<std::shared_ptr<GameObject>> gameObjects;

        int cell_sideLen = 0; // dimensions of each cell
        Vector2Int gridDimensions; // dimensions of grid
        std::vector<std::vector<int>> grid; // 2d vector of all cells in the map

        // pointers to the levels connected to this one
        Scene *above = nullptr, *below = nullptr, *left = nullptr, *right = nullptr;

    private:

        // saves a game objects onto one line of a file
        void SaveGameObjectToFile( std::fstream *file, std::shared_ptr<GameObject> obj );
        // creates a game object from one line of a file, and returns a reference to it
        std::shared_ptr<GameObject> CreateGameObjectFromFile( std::istringstream *iss );

        // reference to the game itself
        Game *game = nullptr;
};

#endif