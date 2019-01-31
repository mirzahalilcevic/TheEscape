#pragma once

#include <vector>

#include "Player.hpp"

class Level
{
    public:

        size_t number; // level number
        size_t width, height; // map dimensions
        std::vector<int> levelMap; // data

        Level(Player& player)
            : player_(player)
        {}

        void load(size_t);


    private:

        Player& player_;

};
