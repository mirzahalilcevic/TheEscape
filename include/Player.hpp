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

        // field of view
        static constexpr double fovWalk = 60 * PI / 180;
        static constexpr double fovRun  = 70 * PI / 180;
        static constexpr double fovStep = 0.03;

        // stats
        static constexpr double fullStamina = 100.0;
        static constexpr double fullHealth = 100.0;

        /// members

        double x, y, rot = 0.0; // position

        double moveSpeed = moveSpeedWalk;
        double rotSpeed  = rotSpeedWalk;
        double fov       = fovWalk;

        double stamina = fullStamina;
        double health  = fullHealth;

};
