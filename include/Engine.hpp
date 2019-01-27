#pragma once

#ifdef __MINGW32__
#   define _WIN32_WINNT 0x0501
#endif // __MINGW32__

#include <windows.h>

#include "Level.hpp"
#include "Player.hpp"

class Engine
{
    public:

        static constexpr DWORD frameDelay = 16; // ~60 FPS

        Engine();
        ~Engine();

        void init(HWND hwnd) { hwnd_ = hwnd; }
        void start();
        void handleInput(RAWINPUT *);


    private:

        HWND hwnd_;

        HBRUSH blackBrush_;
        HBRUSH whiteBrush_;
        HPEN blackPen_;
        HPEN whitePen_;

        Player player_;
        Level level_{player_};

        void checkInput();
        void render();
        void drawMiniMap(HDC);

};
