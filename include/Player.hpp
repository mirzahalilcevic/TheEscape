#pragma once

#define PI 3.1416

class Player
{
    public:

        double x, y;
        double rot;
        double moveSpeed = 1.0;
        double rotSpeed = 6 * PI / 180;

};
