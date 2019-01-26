#pragma once

#include <windows.h>

#include "Level.hpp"

class Engine
{
    public:

        static constexpr DWORD frameDelay = 16; // 60 FPS

        Engine(HWND);
        void start();


    private:

        HWND hwnd_;
        Level level_;

};
