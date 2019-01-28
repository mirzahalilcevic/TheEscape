#include <fstream>
#include <string>
#include <stdexcept>

#include "Level.hpp"

#define LEVEL(n) std::string("Levels/Level") + std::to_string(n) + ".txt"

using namespace std;

void Level::load(size_t n)
{
    number = n;

    ifstream levelFile(LEVEL(number));
    if (levelFile.is_open())
    {
        levelFile >> player_.x >> player_.y >> player_.rot;
        levelFile >> width >> height;

        levelMap.resize(height);
        for (auto i = 0u; i < height; ++i)
        {
            levelMap[i].resize(width);
            for (auto j = 0u; j < width; ++j)
                levelFile >> levelMap[i][j];

        }
    }
    else
        throw invalid_argument("unable to open file");


    levelFile.close();
}
