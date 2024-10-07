#include "Game.hpp"
#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/LAudio.hpp"


void Game::load_levels( std::string dir )
{
    std::string filename = dir + "Base.txt";
    showLoadMessage("loading levels (1/3)\n"+filename, 512, 512, window);
    Base = Scene(filename, this);
    if (Base.player != nullptr) currLevel = &Base;

    filename = dir + "Woods.txt";    
    showLoadMessage("loading levels (2/3)\n"+filename, 512, 512, window);
    Woods = Scene(filename, this);
    if (Woods.player != nullptr) currLevel = &Woods;

    filename = dir + "Town.txt";    
    showLoadMessage("loading levels (3/3)\n"+filename, 512, 512, window);
    Town = Scene(filename, this);
    if (Town.player != nullptr) currLevel = &Town;

    // set up pointers to adjacent levels
    Base.assignNeighbours(&Town, &Woods);
    Woods.assignNeighbours(&Base, nullptr);
    Town.assignNeighbours(nullptr, &Base);

    isNight = false;
    g_time = 0.0f;

    interactRange = 3.0f * currLevel->cell_sideLen;

    filename = dir + "gameData.txt";    
    showLoadMessage("loading game data\n"+filename, 512, 512, window);
    load_gameData(filename);
}

void Game::load_gameData( std::string filename )
{
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Couldn't open " << filename << std::endl;
    } else {
        std::string line;
        std::getline(file, line);

        std::istringstream iss( line );

        iss >> std::dec >> isNight >> riverDammed >> enemySpawnRate;
        file.close();
    }
}

void Game::load_textures()
{
    fpsTex = std::make_unique<LTexture>(window);
    controlsTex = std::make_unique<LTexture>(window);
    // load all the textures from their image files

    std::string filename = "../../assets/Ground/Grass.png";
    showLoadMessage("Loading textures (1/35)\n"+filename, 512, 512, window);
    grassTex = std::make_shared<LTexture>(window);
    if (!grassTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Buildings/Log.png";
    showLoadMessage("Loading textures (2/35)\n"+filename, 512, 512, window);
    logTex = std::make_shared<LTexture>(window);
    if (!logTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Buildings/Dam.png";
    showLoadMessage("Loading textures (3/35)\n"+filename, 512, 512, window);
    damTex = std::make_shared<LTexture>(window);
    if (!damTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename <<std::endl; 
    }

    filename = "../../assets/Ground/Water.png";
    showLoadMessage("Loading textures (4/35)\n"+filename, 512, 512, window);
    waterTex = std::make_shared<LTexture>(window);
    if (!waterTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Tree/Tree.png";
    showLoadMessage("Loading textures (5/35)\n"+filename, 512, 512, window);
    treeTex = std::make_shared<LTexture>(window);
    if (!treeTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Tree/Stump.png";
    showLoadMessage("Loading textures (6/35)\n"+filename, 512, 512, window);
    stumpTex = std::make_shared<LTexture>(window);
    if (!stumpTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Tree/Sapling.png";
    showLoadMessage("Loading textures (7/35)\n"+filename, 512, 512, window);
    saplingTex = std::make_shared<LTexture>(window);
    if (!saplingTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Ground/Shoreline0.png";
    showLoadMessage("Loading textures (8/35)\n"+filename, 512, 512, window);
    shoreline0Tex = std::make_shared<LTexture>(window);
    if (!shoreline0Tex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Ground/Shoreline1.png";
    showLoadMessage("Loading textures (9/35)\n"+filename, 512, 512, window);
    shoreline1Tex = std::make_shared<LTexture>(window);
    if (!shoreline1Tex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Ground/Shoreline2.png";
    showLoadMessage("Loading textures (10/35)\n"+filename, 512, 512, window);
    shoreline2Tex = std::make_shared<LTexture>(window);
    if (!shoreline2Tex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Ground/Shoreline3.png";
    showLoadMessage("Loading textures (11/35)\n"+filename, 512, 512, window);
    shoreline3Tex = std::make_shared<LTexture>(window);
    if (!shoreline3Tex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Ground/Shoreline4.png";
    showLoadMessage("Loading textures (12/35)\n"+filename, 512, 512, window);
    shoreline4Tex = std::make_shared<LTexture>(window);
    if (!shoreline4Tex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Buildings/Door.png";
    showLoadMessage("Loading textures (13/35)\n"+filename, 512, 512, window);
    closed_doorTex = std::make_shared<LTexture>(window);
    if (!closed_doorTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Buildings/Open_Door.png";
    showLoadMessage("Loading textures (14/35)\n"+filename, 512, 512, window);
    open_doorTex = std::make_shared<LTexture>(window);
    if (!open_doorTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Entities/Player/front/0.png";
    showLoadMessage("Loading textures (15/35)\n"+filename, 512, 512, window);
    playerTex = std::make_shared<LTexture>(window);
    if (!playerTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Entities/Wolf.png";
    showLoadMessage("Loading textures (16/35)\n"+filename, 512, 512, window);
    wolfTex = std::make_shared<LTexture>(window);
    if (!wolfTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Tree/Falling_Tree.png";
    showLoadMessage("Loading textures (17/35)\n"+filename, 512, 512, window);
    falling_treeTex = std::make_shared<LTexture>(window);
    if (!falling_treeTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Items/Pine_Cone.png";
    showLoadMessage("Loading textures (18/35)\n"+filename, 512, 512, window);
    pine_coneTex = std::make_shared<LTexture>(window);
    if (!pine_coneTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Items/Plank.png";
    showLoadMessage("Loading textures (19/35)\n"+filename, 512, 512, window);
    plankTex = std::make_shared<LTexture>(window);
    if (!plankTex->loadFromFile("../../assets/Items/Plank.png")) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Entities/Fox.png";
    showLoadMessage("Loading textures (20/35)\n"+filename, 512, 512, window);
    foxTex = std::make_shared<LTexture>(window);
    if (!foxTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Items/Berry.png";
    showLoadMessage("Loading textures (21/35)\n"+filename, 512, 512, window);
    berryTex = std::make_shared<LTexture>(window);
    if (!berryTex->loadFromFile("../../assets/Items/Berry.png")) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Ground/Dirt.png";
    showLoadMessage("Loading textures (22/35)\n"+filename, 512, 512, window);
    dirtTex = std::make_shared<LTexture>(window);
    if (!dirtTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Buildings/Berry_Bush.png";
    showLoadMessage("Loading textures (23/35)\n"+filename, 512, 512, window);
    berry_bushTex = std::make_shared<LTexture>(window);
    if (!berry_bushTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Buildings/EmptyBush.png";
    showLoadMessage("Loading textures (24/35)\n"+filename, 512, 512, window);
    empty_bushTex = std::make_shared<LTexture>(window);
    if (!empty_bushTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Menu/Heart.png";
    showLoadMessage("Loading textures (25/35)\n"+filename, 512, 512, window);
    heartTex = std::make_shared<LTexture>(window);
    if (!heartTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Items/Stone.png";
    showLoadMessage("Loading textures (26/35)\n"+filename, 512, 512, window);
    stoneTex = std::make_shared<LTexture>(window);
    if (!stoneTex->loadFromFile(filename)) {
        std::cerr << "Failed  to load " << filename << std::endl;
    }
    filename = "../../assets/Menu/Mouse/LeftClick.png";
    showLoadMessage("Loading textures (27/35)\n"+filename, 512, 512, window);
    LMBTex = std::make_shared<LTexture>(window);
    if (!LMBTex->loadFromFile(filename)) {
        std::cerr << "failed to load " << filename << std::endl;
    }
    filename = "../../assets/Menu/Mouse/MiddleClick.png";
    showLoadMessage("Loading textures (28/35)\n"+filename, 512, 512, window);
    MMBTex = std::make_shared<LTexture>(window);
    if (!MMBTex->loadFromFile(filename)) {
        std::cerr << "failed to load " << filename << std::endl;
    }
    filename = "../../assets/Menu/Mouse/RightClick.png";
    showLoadMessage("Loading textures (29/35)\n"+filename, 512, 512, window);
    RMBTex = std::make_shared<LTexture>(window);
    if (!RMBTex->loadFromFile(filename)) {
        std::cerr << "failed to load " << filename << std::endl;
    }
    filename = "../../assets/Entities/Bear.png";
    showLoadMessage("Loading textures (30/35)\n"+filename, 512, 512, window);
    BearTex = std::make_shared<LTexture>(window);
    if (!BearTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Entities/Bird.png";
    showLoadMessage("Loading textures (31/35)\n"+filename, 512, 512, window);
    BirdTex = std::make_shared<LTexture>(window);
    if (!BirdTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Entities/Bomb.png";
    showLoadMessage("Loading textures (32/35)\n"+filename, 512, 512, window);
    BombTex = std::make_shared<LTexture>(window);
    if (!BombTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Target.png";
    showLoadMessage("Loading textures (33/35)\n"+filename, 512, 512, window);
    TargetTex = std::make_shared<LTexture>(window);
    if (!TargetTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/DashedCircle.png";
    showLoadMessage("Loading textures (34/35)\n"+filename, 512, 512, window);
    dashed_circleTex = std::make_shared<LTexture>(window);
    if (!dashed_circleTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Entities/Rabbit.png";
    showLoadMessage("Loading textures (35/35)\n"+filename, 512, 512, window);
    rabbitTex = std::make_shared<LTexture>(window);
    if (!rabbitTex->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
}

void Game::load_audio()
{
    std::string filename = "../../assets/Audio/Destruction/LogBreaking.wav";
    showLoadMessage("Loading audio (1/28)\n"+filename, 512, 512, window);
    logDestruction = std::make_shared<LAudio>();
    if (!logDestruction->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Audio/Destruction/TreeFalling.wav";
    showLoadMessage("Loading audio (2/28)\n"+filename, 512, 512, window);
    treeFalling = std::make_shared<LAudio>();
    if (!treeFalling->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Audio/DoorToggle.wav";
    showLoadMessage("Loading audio (3/28)\n"+filename, 512, 512, window);
    doorToggle = std::make_shared<LAudio>();
    if (!doorToggle->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    
    filename = "../../assets/Audio/Destruction/Leaves.wav";
    showLoadMessage("Loading audio (4/28)\n"+filename, 512, 512, window);
    leaves = std::make_shared<LAudio>();
    if (!leaves->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Audio/EntitySounds/Pop.wav";
    showLoadMessage("Loading audio (5/28)\n"+filename, 512, 512, window);
    pop = std::make_shared<LAudio>();
    if (!pop->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Audio/EntitySounds/Bonk.wav";
    showLoadMessage("Loading audio (6/28)\n"+filename, 512, 512, window);
    bonk = std::make_shared<LAudio>();
    if (!bonk->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/EntitySounds/BirdSpawn.wav";
    showLoadMessage("Loading audio (7/28)\n"+filename, 512, 512, window);
    birdSpawn = std::make_shared<LAudio>();
    if (!birdSpawn->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/GameOver.wav";
    showLoadMessage("Loading audio (8/28)\n"+filename, 512, 512, window);
    gameOverSound = std::make_shared<LAudio>();
    if (!gameOverSound->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/EntitySounds/Heal.wav";
    showLoadMessage("Loading audio (9/28)\n"+filename, 512, 512, window);
    healSound = std::make_shared<LAudio>();
    if (!healSound->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/EntitySounds/PlayerDamage.wav";
    showLoadMessage("Loading audio (10/28)\n"+filename, 512, 512, window);
    playerDamage = std::make_shared<LAudio>();
    if (!playerDamage->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Destruction/Explosion.wav";
    showLoadMessage("Loading audio (11/28)\n"+filename, 512, 512, window);
    explosionSound = std::make_shared<LAudio>();
    if (!explosionSound->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Audio/Speech/8bit_vocal_deep0.wav";
    showLoadMessage("Loading audio (12/28)\n"+filename, 512, 512, window);
    vocalDeep0 = std::make_shared<LAudio>();
    if (!vocalDeep0->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_deep1.wav";
    showLoadMessage("Loading audio (13/28)\n"+filename, 512, 512, window);
    vocalDeep1 = std::make_shared<LAudio>();
    if (!vocalDeep1->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_med0.wav";
    showLoadMessage("Loading audio (14/28)\n"+filename, 512, 512, window);
    vocalMed0 = std::make_shared<LAudio>();
    if (!vocalMed0->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_med1.wav";
    showLoadMessage("Loading audio (15/28)\n"+filename, 512, 512, window);
    vocalMed1 = std::make_shared<LAudio>();
    if (!vocalMed1->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_high0.wav";
    showLoadMessage("Loading audio (16/28)\n"+filename, 512, 512, window);
    vocalHigh0 = std::make_shared<LAudio>();
    if (!vocalHigh0->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_high1.wav";
    showLoadMessage("Loading audio (17/28)\n"+filename, 512, 512, window);
    vocalHigh1 = std::make_shared<LAudio>();
    if (!vocalHigh1->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Repair.wav";
    showLoadMessage("Loading audio (18/28)\n"+filename, 512, 512, window);
    repairSound = std::make_shared<LAudio>();
    if (!repairSound->loadFromFile(filename)) {
        std::cerr << "Failed to laod " << filename << std::endl;
    }
    

    for (int i = 0; i < 3; i++) {
        filename = "../../assets/Audio/EntitySounds/WolfSpawn"+std::to_string(i+1)+".wav";
        showLoadMessage("Loading audio ("+std::to_string(19+i)+"/28)\n"+filename, 512, 512, window);
        auto sound = std::make_shared<LAudio>();
        if (sound->loadFromFile(filename)) {
            wolfSpawnSounds[i] = sound;
        } else {
            std::cerr << "Failed to open " << filename << std::endl;
        }
    }
    
    for (int i = 0; i < 3; i++) {
        filename = "../../assets/Audio/Destruction/splash/"+std::to_string(i)+".wav";
        showLoadMessage("Loading audio ("+std::to_string(22+i)+"/28)\n"+filename, 512, 512, window);
        auto sound = std::make_shared<LAudio>();
        if (sound->loadFromFile(filename)) {
            splashSounds[i] = sound;
        } else {
            std::cerr << "Failed to open " << filename << std::endl;
        }
    }

    filename = "../../assets/Audio/Music/digiworld demigirl.wav";
    showLoadMessage("Loading audio (25/28)\n"+filename, 512, 512, window);
    nightMusic = std::make_shared<LAudio>();
    if (!nightMusic->loadFromFile(filename)) {
        std::cerr << "failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Music/Arp 9.wav";
    showLoadMessage("Loading audio (26/28)\n"+filename, 512, 512, window);
    dayMusic = std::make_shared<LAudio>();
    if (!dayMusic->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Music/acidruinedyourlifecore.wav";
    showLoadMessage("Loading audio (27/28)\n"+filename, 512, 512, window);
    menuMusic = std::make_shared<LAudio>();
    if (!menuMusic->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Music/internetboy.wav";
    showLoadMessage("Loading audio (28/28)\n"+filename, 512, 512, window);
    deathMusic = std::make_shared<LAudio>();
    if (!deathMusic->loadFromFile("../../assets/Audio/Music/internetboy.wav")) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
}

void Game::load_fonts()
{
    std::string filename = "../../assets/Fonts/Seven_Segment.ttf";
    showLoadMessage("Loading fonts (1/3)\n"+filename, 512, 512, window);
    sevenSegment = TTF_OpenFont(filename.c_str(), 48);
    if (sevenSegment == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }

    filename = "../../assets/Fonts/ARCADECLASSIC.TTF";
    showLoadMessage("Loading fonts (2/3)\n"+filename, 512, 512, window);
    arcadeClassic24 = TTF_OpenFont(filename.c_str(), 24);
    if (arcadeClassic24 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Fonts/ARCADECLASSIC.TTF";
    showLoadMessage("Loading fonts (3/3)\n"+filename, 512, 512, window);
    arcadeClassic36 = TTF_OpenFont(filename.c_str(), 36);
    if (arcadeClassic36 == NULL) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
}

void Game::load_animations()
{
    // player animations
    std::vector<std::shared_ptr<LTexture>> front, back, left, right;
    float df = 1.0f, db = 1.0f, dl = 1.0f, dr = 1.0f;

    std::string path = "../../assets/Entities/Player/";
    std::string path1 = path + "back/";
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+1)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        back.push_back(newTex);
    }

    path1 = path + "front/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+5)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }
    
    path1 = path + "left/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+9)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        left.push_back(newTex);
    }
    
    path1 = path + "right/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+13)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        right.push_back(newTex);
    }

    playerAnimations = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);


    // wolf walking animations
    front.clear(); back.clear(); left.clear(); right.clear();
    df = 1.0f, db = 1.0f, dl = 1.0f, dr = 1.0f;
    path = "../../assets/Entities/Wolf/Walking/";
    
    path1 = path + "forwards/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+17)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }
    
    path1 = path + "backwards/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+21)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        back.push_back(newTex);
    }
    
    path1 = path + "left/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+25)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        left.push_back(newTex);
    }
    
    path1 = path + "right/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+29)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        right.push_back(newTex);
    }

    wolfWalkingAnimation = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);


    // explosion
    front.clear(); back.clear(); left.clear(); right.clear();
    df = 0.25f, db = 0.25f, dl = 0.25f, dr = 0.25f;
    path = "../../assets/Entities/Explosion/";

    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+33)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }

    explosionAnimation = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);
    
    // splash
    front.clear(); back.clear(); left.clear(); right.clear();
    df = 0.3333f, db = 0.3333f, dl = 0.3333f, dr = 0.3333f;
    path = "../../assets/Entities/splash/";

    for (int i = 0; i < 3; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path + std::to_string(i) + ".png";
        showLoadMessage("Loading animations ("+std::to_string(i+37)+"/39)\n"+filename, 512, 512, window);
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }

    splashAnimation = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);
}

void Game::create_textures()
{
    showLoadMessage("creating textures", 512, 512, window);
    clockBackBar = tEditor.createSolidColour(75, 5, 0x000000D0, window);
    redBar = tEditor.createSolidColour(81, 11, 0xFF0000D0, window);
    clockTex = std::make_unique<LTexture>(window);
}