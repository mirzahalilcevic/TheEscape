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
        levelFile >> width >> height;

        levelMap.resize(width);
        for (int i = 0; i < width; ++i)
        {
            levelMap[i].resize(height);
            for (int j = 0; j < height; ++j)
                levelFile >> levelMap[i][j];

        }
    }
    else
        throw invalid_argument("unable to open file");


    levelFile.close();
}
