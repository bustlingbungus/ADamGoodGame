#include "Scene.hpp"
#include "../gameObject/Bird.hpp"
#include "../gameObject/Bomb.hpp"
#include "../gameObject/CosmeticGameObjects.hpp"
#include "../gameObject/Item.hpp"
#include "../gameObject/NPC.hpp"
#include "../gameObject/Player.hpp"
#include "../gameObject/Wolf.hpp"

Scene::Scene( std::string filePath, Game *game ) : game(game)
{
    std::ifstream file(filePath);
    if (!file) {
        std::cout << "Couldn't open "<<filePath <<"! :(\n";
    
    } else {

        // get the scene's name from the first line
        std::getline( file, name );

        // get the line of the file as a string
        std::string line;
        std::getline( file, line );
        // convert the string to istringstream
        std::istringstream iss( line );

        int idxPlayer, idxHeld;
        iss >> std::dec>>idxPlayer >> idxHeld >> cell_sideLen;
        // if there is no player, there also shouldn't be a held object
        if (idxPlayer == -1) idxHeld = -1;


        // next line
        std::getline( file, line );
        iss = std::istringstream( line );

        int n;
        iss >> n;

        // make sure gameObjects is empty
        gameObjects.clear();
        gameObjects = std::vector<std::shared_ptr<GameObject>>(n);
        // for the next n lines, create game objects
        for (int i = 0; i < n; i++)
        {
            // next line
            std::getline( file, line );
            iss = std::istringstream( line );

            // create the game object
            auto obj = CreateGameObjectFromFile( &iss );
            if (obj == nullptr) {
                gameObjects[i] = nullptr;
                continue;
            }

            // obj will already have an index, so insert it at that index in lvlObjects
            int id = obj->get_idx();
            gameObjects[id] = obj;

            if (id == idxPlayer) {
                // assign obj to the player pointer
                player = std::dynamic_pointer_cast<Player>(obj);
            } else if (id == idxHeld) {
                // assign obj to the held object pointer
                held = std::dynamic_pointer_cast<Item>(obj);
                held->make_held(false);
            }
        }
        
        // clear out all invalid entity types
        for (int i = 0; i < gameObjects.size(); i++)
        {
            auto obj = gameObjects[i];
            if (obj == nullptr) {
                gameObjects.erase(gameObjects.begin() + i);
                for (int j = i; j < gameObjects.size(); j++) {
                    auto obj2 = gameObjects[j];
                    if (obj2 != nullptr) obj2->decrement_idx();
                }
            }
        }


        // next line
        std::getline( file, line );
        iss = std::istringstream( line );


        iss >> gridDimensions.x >> gridDimensions.y;

        grid.clear();
        grid = std::vector<std::vector<int>>(gridDimensions.x);
        for (int i = 0; i < gridDimensions.x; i++) {
            std::vector<int> vec(gridDimensions.y);

            // next line
            std::getline( file, line );
            iss = std::istringstream( line );

            // read the next ny hex integers into vec
            for (int j = 0; j < gridDimensions.y; j++) {
                iss >> std::hex >> vec[j];
            }

            // add vec into the grid
            grid[i] = vec;
        }

        // close the file
        file.close();
    }
}
Scene::Scene() {}

Scene::~Scene() { free(); }

void Scene::free()
{
    gameObjects.clear();
    for (int i = 0; i < gridDimensions.x; i++) grid[i].clear();
    grid.clear();

    player = nullptr;
    held = nullptr;
}


std::shared_ptr<GameObject> Scene::CreateGameObjectFromFile( std::istringstream *iss )
{
    // all object attributes saved in iss
    Vector2 pos;
    int type, idx, HP;
    
    // read in values from iss
    *iss >> idx >> type >> pos.x >> pos.y >> HP;

    std::shared_ptr<GameObject> obj = nullptr;

    switch (type)
    {
        case 0:
            obj = std::make_shared<Player>(pos, idx, HP, game, cell_sideLen);
            break;
        case wolf:
            obj = std::make_shared<Wolf>(pos, idx, HP, game, cell_sideLen, nullptr);
            break;
        case bird:
            obj = std::make_shared<Bird>(pos, idx, game, cell_sideLen);
            break;
        case bomb:
            obj = std::make_shared<Bomb>(pos, idx, game, cell_sideLen);
            break;
        // cosmetic entities
        case target:
        case bombExplosionIndicator:
        case ghostBuilding:
        case tutorialText:
            std::cerr << "ERROR: entity type "<< (int)type <<" may not be loaded from file!\n";
            break;
        // NPCs
        case foxNPC:
        case bearNPC:
        case rabbitNPC:
            obj = std::make_shared<NPC>(type, pos, idx, game, cell_sideLen);
            break;
        // items
        case logItem:
        case pineConeItem:
        case plankItem:
        case damItem:    
        case doorItem:      
        case stoneItem:     
        case berryItem:
            obj = std::make_shared<Item>(type, pos, idx, HP, game, cell_sideLen);
            break;
        default:
            std::cerr << "Invalid Entity type loaded: " << (int)type <<'\n';
            break;
    }
    return obj;
}






void Scene::Save( std::string directory )
{
    std::string filePath = directory;
    char last = directory[directory.size()-1];
    filePath += (last=='/' || last == '\\')? name+".txt" : '\\'+name+".txt";

    // open the specified file for writing
    std::fstream file;
    file.open(filePath, std::ios::out);
    if (!file) return;

    file << name <<'\n';

    int idxPlayer = (player == nullptr)? -1 : player->get_idx(),
        idxHeld = (held == nullptr)? -1 : held->get_idx();

    file << std::dec<<idxPlayer <<'\t'<< idxHeld <<'\t'<< cell_sideLen <<'\n';


    int n = gameObjects.size();
    file <<n<<'\n';

    for (int i = 0; i < n; i++) {
        SaveGameObjectToFile( &file, gameObjects[i] );
    }

    file << gridDimensions.x <<'\t'<< gridDimensions.y <<'\n';
    for (int i = 0; i < gridDimensions.x; i++) {
        for (int j = 0; j < gridDimensions.y; j++) {
            file << std::hex << grid[i][j] <<'\t';
        }
        file << '\n';
    }
    // close the file
    file.close();
}

void Scene::assignNeighbours(Scene *Above, Scene *Below, Scene *Left, Scene *Right)
{
    above = Above; below = Below; left = Left; right = Right;
}

void Scene::SaveGameObjectToFile( std::fstream *file, std::shared_ptr<GameObject> obj )
{
    // validate the file and the game object
    if (file && obj != nullptr) 
    {
        Vector2 pos = obj->get_pos();
        int idx = obj->get_idx(), type = obj->get_type(), HP = obj->get_HP();

        *file <<std::dec<< idx <<'\t'<< type <<'\t'<< pos.x <<'\t'<< pos.y <<'\t'<< HP <<'\n';
    }
}