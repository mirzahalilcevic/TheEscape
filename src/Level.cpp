#include <fstream>
#include <string>
#include <stdexcept>

#include "Level.hpp"

#define LEVEL(n) ("Levels/Level" + std::to_string(n) + ".txt")

using namespace std;

void Level::load(size_t n)
{
    number = n;

    ifstream levelFile(LEVEL(number));
    if (levelFile.is_open())
    {
        // load player position
        levelFile >> player_.x >> player_.y >> player_.rot;

        // load enemies

        size_t enemyNum;
        levelFile >> enemyNum;

        enemies_.resize(enemyNum);
        for (auto i = 0u; i < enemyNum; ++i)
        {
            levelFile >> enemies_[i].startX >> enemies_[i].startY;
            enemies_[i].x = enemies_[i].startX;
            enemies_[i].y = enemies_[i].startY;
        }

        // load width and height of map
        levelFile >> width >> height;

        // load map data
        levelMap.resize(height * width);
        for (auto i = 0u; i < height; ++i)
            for (auto j = 0u; j < width; ++j)
                levelFile >> levelMap[height * i + j];

    }
    else
        throw invalid_argument("unable to open file \"" + LEVEL(n) + "\"");

    levelFile.close();
}
