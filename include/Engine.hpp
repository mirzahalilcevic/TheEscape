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
#include <ctime>
#include <array>

#include "Level.hpp"
#include "Player.hpp"

enum class GameState
{
    RUNNING,
    MAIN_MENU,
    PAUSE_MENU
};

enum class Menu
{
    NEW_GAME,
    LOAD_GAME,
    QUIT,
    RESUME,
    SAVE_GAME,
    MAIN_MENU,
    NONE
};

enum class MainMenu
{
    NEW_GAME,
    LOAD_GAME,
    QUIT,
    NONE
};

struct Door
{
    size_t y, x;
    int state = 0;
    double length = 1.0;
};

// used for drawing
struct ProjInfo
{
    unsigned int col;
    double distance;
    double fishbowl;
    double texSrc;
    int tex;
    int sprite;
    int xOffset, yOffset;
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
        static constexpr size_t texNum = 9;
        static constexpr size_t spriteNum = 3;
        static constexpr size_t levelNum = 2;

        // IDs
        static constexpr int door = 6;
        static constexpr int doorSide = 9;
        static constexpr int openDoor = -1;
        static constexpr int exit = 7;
        static constexpr int entrance = 8;

        // texture dimensions
        static constexpr size_t texSize = 64;

        // door states
        static constexpr int closed = 0;
        static constexpr int opening = 1;
        static constexpr int open = 2;
        static constexpr int closing = 3;

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

        // timer handler
        void closeDoor(int);


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

        std::vector<Enemy> enemies_;
        std::vector<Life> lives_;
        std::vector<Door> doors_;

        Player player_;
        Level level_{player_, enemies_, lives_};

        // flags
        bool fps_ = true;
        bool miniMap_ = false;
        GameState gameState_ = GameState::MAIN_MENU;

        // graphics

        double viewDistance_;
        double angleIncrement_;

        std::vector<ProjInfo> projCols_;

        std::array<HBITMAP, texNum + spriteNum * 2 + 1> bitmaps_;
        std::array<HDC, texNum> textures_;
        std::array<HDC, spriteNum> sprites_;
        std::array<HDC, spriteNum> spriteMasks_;
        HDC background_;

        // menus

        std::array<RECT, 3> mainMenuItems_;
        std::array<RECT, 3> pauseMenuItems_;
        std::array<HBITMAP, 8> menuBitmaps_;
        std::array<HDC, 8> menus_;
        HDC menu_;

        /// methods

        // updating
        void checkInput();
        void update();

        // rendering
        void render();
        void castRays();
        void drawScene(HDC);
        void drawHud(HDC);
        void drawMiniMap(HDC);

        // saves
        void saveGame();
        void loadGame();

        // misc
        void displayFps(HDC);
        void loadLevel(size_t);
        Door& getDoor(size_t, size_t);
        Menu findMenuItem(int, int, GameState);
};
