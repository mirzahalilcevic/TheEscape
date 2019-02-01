/// *******************************************************************
///
///  This game was created for a school project as part of course
///  RI204 - 'Windows programming' at the University of Tuzla.
///
///  AUTHORS:
///  Mirza Halilcevic
///  Jasmin Hadzic
///
///  January, 2019
///
/// *******************************************************************

#pragma once

#include <windows.h>
#include <cmath>
#include <array>

#include "Level.hpp"
#include "Player.hpp"

enum class GameState
{
    RUNNING,
    MAINMENU,
    PAUSE
};

class Engine
{
    public:

        // field of view
        static constexpr double fov = 60 * PI / 180;

        // commonly used angles
        static constexpr double rot45  =  45 * PI / 180;
        static constexpr double rot90  =  90 * PI / 180;
        static constexpr double rot270 = 270 * PI / 180;
        static constexpr double rot360 = 360 * PI / 180;

        // number of resources
        static constexpr size_t texNum = 5;
        static constexpr size_t spriteNum = 1;

        // texture dimensions
        static constexpr size_t texSize = 64;

        // projection plane
        size_t projPlaneWidth;
        size_t projPlaneHeight;

        // frame time
        DWORD frameTime;

        // constructors
        Engine();
        ~Engine();

        // main stuff
        void init(HWND hwnd);
        void start();

        // input handlers
        void handleMouseMove(int, int);
        void handleLButtonDown(int, int);
        void handleKeyDown(int);


    private:

        // pens
        HPEN blackPen_;
        HPEN whitePen_;
        HPEN darkGrayPen_;
        HPEN lightGrayPen_;
        HPEN redPen_;
        HPEN greenPen_;
        HPEN bluePen_;

        // brushes
        HBRUSH blackBrush_;
        HBRUSH whiteBrush_;
        HBRUSH darkGrayBrush_;
        HBRUSH lightGrayBrush_;
        HBRUSH redBrush_;
        HBRUSH greenBrush_;
        HBRUSH blueBrush_;

        // window related
        HWND hwnd_;
        RECT cRect_;

        // buffer
        HDC memoryDC_;
        HBITMAP memoryBitmap_;

        // gameplay related
        Player player_;
        Level level_{player_};

        // flags
        bool fps_ = false;
        bool miniMap_ = false;
        GameState gameState_ = GameState::RUNNING;

        // graphics
        std::array<HBITMAP, texNum + spriteNum * 2> bitmaps_;
        std::array<HDC, texNum> textures_;
        std::array<HDC, spriteNum> sprites_;
        std::array<HDC, spriteNum> spriteMasks_;

        /// methods

        // updating
        void checkInput();
        void update();

        // rendering
        void render();
        void castRays(HDC);
        void drawMiniMap(HDC);

        // misc
        void displayFps(HDC);

};
