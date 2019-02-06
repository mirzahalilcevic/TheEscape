#pragma once

#include <vector>

#include "Player.hpp"

struct Enemy
{
    static constexpr double moveSpeed = 0.05;

    double startX, startY, startRot;
    double x, y, rot;

    int offset;
};

struct Life
{
    bool visible = true;
    double x, y;
};

class Level
{
    public:

        size_t number; // level number
        size_t width, height; // map dimensions
        std::vector<int> levelMap;

        Level(Player& player, std::vector<Enemy>& enemies, std::vector<Life>& lives)
            : player_(player), enemies_(enemies), lives_(lives)
        {}

        void load(size_t);


    private:

        Player& player_;
        std::vector<Enemy>& enemies_;
        std::vector<Life>& lives_;

};
