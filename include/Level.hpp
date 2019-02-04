#pragma once

#include <vector>

#include "Player.hpp"

struct Enemy
{
    static constexpr double moveSpeed = 0.06;

    double startX, startY, startRot;
    double x, y, rot;

    int offset;
};

class Level
{
    public:

        size_t number; // level number
        size_t width, height; // map dimensions
        std::vector<int> levelMap;

        Level(Player& player, std::vector<Enemy>& enemies)
            : player_(player), enemies_(enemies)
        {}

        void load(size_t);


    private:

        Player& player_;
        std::vector<Enemy>& enemies_;

};
