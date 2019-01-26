#pragma once

#include <vector>

class Level
{
    public:

    size_t number;
    size_t width, height;

    std::vector<std::vector<int>> levelMap;

    void load(size_t);

};
