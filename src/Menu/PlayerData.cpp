#include "PlayerData.hpp"

using namespace Math;

bool PlayerData::loadFromFile( std::string filename )
{
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Couldn't open " << filename << std::endl;
        return false;

    } else {
        std::string line;
        std::getline(file, line);

        std::istringstream iss(line);
        iss >> name >>std::dec>> mostNightsSurvived >> mostEnemiesKilled >> highscore;

        return true;
    }
}


void PlayerData::set_newHighscores(int nightsSurvived, int enemiesKilled, int score)
{
    mostNightsSurvived = max(nightsSurvived, mostNightsSurvived);
    mostEnemiesKilled = max(enemiesKilled, mostEnemiesKilled);
    highscore = max(score, highscore);
}

void PlayerData::set_newHighscores(PlayerData scores)
{
    set_newHighscores(scores.mostNightsSurvived, scores.mostEnemiesKilled, scores.highscore);
}

void PlayerData::calculate_score()
{
    highscore = mostEnemiesKilled + (10 * mostNightsSurvived);
}

void PlayerData::reset()
{
    name = "---";
    highscore = mostEnemiesKilled = mostNightsSurvived = 0;
}

void PlayerData::loadFromIss( std::istringstream *iss )
{
    *iss >> name >>std::dec>> mostNightsSurvived >> mostEnemiesKilled >> highscore;
}

void PlayerData::Save( std::fstream *file )
{
    *file << name <<'\t'<<std::dec<< mostNightsSurvived <<'\t'<< mostEnemiesKilled <<'\t'<< highscore <<'\n';
}

void PlayerData::render(int x, int y, std::shared_ptr<LWindow> window, SDL_Color colour, TTF_Font *font, int colouredLetter, SDL_Color altColor)
{
    if (colouredLetter != -1) 
    {
        int n = name.size(), X = x;
        for (int i = 0; i < n; i++) 
        {
            std::string str = {name[i]};
            SDL_Color col = (i == colouredLetter)? altColor : colour;
            auto rend = std::make_unique<LTexture>(window);
            if (!rend->loadFromRenderedText(str, col, font)) {
                std::cerr << "failed to load rendered text!" << std::endl;
            }
            rend->render(X, y); 
            X += rend->getWidth();
            rend->free();
        }
    } 
    else {
        renderText(name, x, y, window, colour, font, Left_aligned);
    }
    x += 128;

    std::string txt;
    if (name == "---") 
    {
        txt = "-";
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 128;
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 128;
        renderText(txt, x, y, window, colour, font, Left_aligned);
    }
    else 
    {
        std::string txt = std::to_string(highscore);
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 128;
        txt = std::to_string(mostNightsSurvived);
        renderText(txt, x, y, window, colour, font, Left_aligned); x += 128;
        txt = std::to_string(mostEnemiesKilled);
        renderText(txt, x, y, window, colour, font, Left_aligned);
    }
}



bool Settings::loadFromFile( std::string filename )
{
    std::ifstream file(filename);

    if (!file)
    {
        std::cerr << "Couldn't open " << filename <<'\n';
        return false;
    }
    else 
    {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        iss >>std::dec>> volume >> flags >> max_framrate >> musicVolume;
        file.close();
        return true;
    }
}

void Settings::loadFromFile( std::ifstream *file )
{
    std::string line;
    std::getline(*file, line);
    std::istringstream iss(line);
    iss >>std::dec>> volume >> flags >> max_framrate >> musicVolume;
}

void Settings::Save( std::string filename )
{
    std::fstream file;
    file.open(filename, std::ios::out);

    if (!file) {
        std::cerr << "Failed to save to " << filename << std::endl;
    }
    else 
    {
        file <<std::dec<< volume <<'\t'<< flags <<'\t'<< max_framrate <<'\t'<< musicVolume <<'\n';
        file.close();
    }
}

void Settings::Save( std::fstream *file )
{
    *file <<std::dec<< volume <<'\t'<< flags <<'\t'<< max_framrate <<'\t'<< musicVolume <<'\n';
} 

void Settings::reset()
{
    volume = musicVolume = 64;
    // leave fullscreen unaltered
    flags = 0b00101010|(flags&FULLSCREEN);
    max_framrate = -1;
}