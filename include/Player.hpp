#pragma once

#define PI 3.14159265358979323846

class Player
{
    public:

        // movement
        static constexpr double moveSpeedWalk = 0.04;
        static constexpr double moveSpeedRun  = 0.10;

        // rotation
        static constexpr double rotSpeedWalk  = 2 * PI / 180;
        static constexpr double rotSpeedRun   = 3 * PI / 180;

        // stats
        static constexpr unsigned int maxLives = 5;
        static constexpr double fullStamina = 100.0;

        /// members

        unsigned int lives = 3;
        double stamina = fullStamina;

        double x, y, rot; // position

        double moveSpeed;
        double rotSpeed;

        int dir;

        bool running;

};
