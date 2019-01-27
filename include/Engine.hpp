#pragma once

#include <windows.h>

#include "Level.hpp"
#include "Player.hpp"

class Engine
{
    public:

        static constexpr DWORD frameDelay = 16; // 60 FPS

        Engine(HWND, HBRUSH);
        ~Engine();

        void start();


    private:

        HWND hwnd_;
        HBRUSH backgroundBrush_;
        HBRUSH blackBrush_;
        HPEN blackPen_;
        HPEN whitePen_;

        Player player_;
        Level level_{player_};

        void render();
        void drawMiniMap(HDC);

};
