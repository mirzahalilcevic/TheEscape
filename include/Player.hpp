#pragma once

#define PI 3.14159265358979323846

class Player
{
    public:

        // movement
        static constexpr double moveSpeedWalk = 0.05;
        static constexpr double moveSpeedRun  = 0.10;

        // rotation
        static constexpr double rotSpeedWalk  = 3 * PI / 180;
        static constexpr double rotSpeedRun   = 5 * PI / 180;

        // stats
        static constexpr double fullStamina = 100.0;
        static constexpr double fullHealth = 100.0;

        /// members

        double stamina = fullStamina;
        double health  = fullHealth;

        double x, y, rot; // position

        double moveSpeed;
        double rotSpeed;

        int dir;

        bool running;

};
