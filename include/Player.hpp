#pragma once

#define PI 3.14159265358979323846

class Player
{
    public:

        static constexpr double walkSpeed = 0.05;
        static constexpr double runSpeed = 0.10;
        static constexpr double rotSpeed = 3 * PI / 180;
        static constexpr double fov = PI / 3;

        double x, y;
        double rot;
        double moveSpeed = walkSpeed;

};
