#pragma once

#include <windows.h>
#include <cmath>

#include "Level.hpp"
#include "Player.hpp"

class Engine
{
    public:

        static constexpr DWORD frameDelay = 1000/60; // ~60 FPS

        // projection plane dimensions
        static constexpr size_t projPlaneWidth = 794;
        static constexpr size_t projPlaneHeight = 571;

        // distance from player to the projection plane
        static constexpr double viewDistance = (projPlaneWidth / 2) / tan(Player::fov / 2.0);

        // angle between subsequent rays
        static constexpr double angleIncrement = Player::fov / projPlaneWidth;

        // commonly used angles
        static constexpr double rot90 = PI / 2.0;
        static constexpr double rot270 = 1.5 * PI;
        static constexpr double rot360 = 2.0 * PI;

        Engine();
        ~Engine();

        void init(HWND hwnd);
        void start();

        void handleLButtonDown(int, int);


    private:

        DWORD frameTime_;

        HBRUSH blackBrush_;
        HBRUSH whiteBrush_;
        HPEN blackPen_;
        HPEN whitePen_;
        HPEN grayPen_;

        HWND hwnd_;
        RECT cRect_;

        Player player_;
        Level level_{player_};

        void checkInput();
        void updatePlayerPos(double, double);

        void render();
        void drawMiniMap(HDC);
        void castRays(HDC);

};
