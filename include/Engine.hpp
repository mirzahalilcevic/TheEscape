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

#include "Level.hpp"
#include "Player.hpp"

enum class GameState
{
    MENU,
    GAME,
    PAUSE
};

class Engine
{
    public:

        // commonly used angles
        static constexpr double rot90  =  90 * PI / 180;
        static constexpr double rot270 = 270 * PI / 180;
        static constexpr double rot360 = 360 * PI / 180;

        // other constants
        static constexpr size_t miniMapScale = 8;
        static constexpr size_t texSize = 64;

        // projection plane
        size_t projPlaneWidth, projPlaneHeight;
        size_t projPlaneWidthHalf;

        // constructors
        Engine();
        ~Engine();

        // main stuff
        void init(HWND hwnd);
        void start();

        // input handlers
        void handleLButtonDown(int, int);
        void handleKeyDown(int);


    private:

        DWORD frameTime_;

        // pens
        HPEN blackPen_;
        HPEN whitePen_;
        HPEN greenPen_;

        // brushes
        HBRUSH blackBrush_;
        HBRUSH whiteBrush_;
        HBRUSH greenBrush_;

        // window related
        HWND hwnd_;
        RECT cRect_;

        // gameplay related
        Player player_;
        Level level_{player_};

        // bitmaps and textures
        std::vector<HBITMAP> bitmaps_;
        std::vector<HDC> textures_;

        // flags
        bool miniMap_ = false;
        bool fps_ = false;
        GameState gameState_ = GameState::GAME;

        /// methods

        // updating
        void checkInput();
        void updatePlayerPos(double, double);

        // drawing
        void render();
        void castRays(HDC);
        void drawMiniMap(HDC);

};
