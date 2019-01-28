#pragma once

#include <windows.h>

#include "Level.hpp"
#include "Player.hpp"

class Engine
{
    public:

        static constexpr DWORD frameDelay = 1000/60; // ~60 FPS

        Engine();
        ~Engine();

        void init(HWND hwnd);
        void start();

        void handleLButtonDown(int, int);

    private:

        HWND hwnd_;

        HBRUSH blackBrush_;
        HBRUSH whiteBrush_;
        HPEN blackPen_;
        HPEN whitePen_;
        HPEN grayPen_;

        RECT cRect_;

        Player player_;
        Level level_{player_};

        void checkInput();
        void updatePlayerPos(double, double);

        void render();
        void drawMiniMap(HDC);
        void castRays();

};
