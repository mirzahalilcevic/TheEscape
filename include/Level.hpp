#pragma once

#include <vector>

#include "Player.hpp"

class Level
{
    public:

        size_t number;
        size_t width, height;
        std::vector<int> levelMap;

        Level(Player& player)
            : player_(player)
        {}

        void load(size_t);


    private:

        Player& player_;

};
