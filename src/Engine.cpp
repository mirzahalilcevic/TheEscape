#include <iostream>

#include "Engine.hpp"

#define PRESSED(key) GetAsyncKeyState(key) & 0x8000
#define TEXTURE(n) (std::string("Textures/Texture") + std::to_string(n) + ".bmp").c_str()

using namespace std;

Engine::Engine()
{
    // pens
    blackPen_ = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    whitePen_ = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    greenPen_ = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));

    // brushes
    blackBrush_ = CreateSolidBrush(RGB(0, 0, 0));
    whiteBrush_ = CreateSolidBrush(RGB(255, 255, 255));
    greenBrush_ = CreateSolidBrush(RGB(0, 255, 0));
}

Engine::~Engine()
{
    // pens
    DeleteObject(blackPen_);
    DeleteObject(whitePen_);
    DeleteObject(greenPen_);

    // brushes
    DeleteObject(blackBrush_);
    DeleteObject(whiteBrush_);
    DeleteObject(greenBrush_);

    // bitmaps
    for (auto bitmap : bitmaps_)
        DeleteObject(bitmap);

    // textures
    for (auto hdc : textures_)
        DeleteDC(hdc);
}

void Engine::init(HWND hwnd)
{
    // window related
    hwnd_ = hwnd;
    GetClientRect(hwnd_, &cRect_);

    // projection plane
    projPlaneWidth = cRect_.right;
    projPlaneHeight = cRect_.bottom;
    projPlaneWidthHalf = projPlaneWidth / 2;

    // load first level
    level_.load(2);

    // load bitmaps
    for (auto i = 0u; i < 5; ++i)
    {
        HBITMAP bitmap = (HBITMAP) LoadImage(NULL, TEXTURE(i + 1), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        bitmaps_.push_back(bitmap);
    }

    // prepare textures
    for (auto bitmap : bitmaps_)
    {
        HDC hdc = CreateCompatibleDC(NULL);
        SelectObject(hdc, bitmap);
        textures_.push_back(hdc);
    }
}

void Engine::start()
{
    MSG message;
    DWORD startTime, lastTime;

    lastTime = GetTickCount();
    while (true) // game loop
    {
        startTime = GetTickCount();
        frameTime_ = startTime - lastTime;

        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
                break;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        ///

        if (gameState_ == GameState::GAME)
            checkInput();

        render();

        ///

        lastTime = startTime;
    }
}

void Engine::handleLButtonDown(int x, int y)
{
    /// TODO: handle left click
}

void Engine::handleKeyDown(int key)
{
    switch (key)
    {
        case 'F': // toggle fps
            fps_ = !fps_;
            break;
        case 'M': // toggle mini map
            miniMap_ = !miniMap_;
            break;

        case VK_ESCAPE: // pause the game
            gameState_ = (gameState_ == GameState::PAUSE) ? GameState::GAME
                                                          : GameState::PAUSE;
            break;

        case VK_SPACE: // action key
            /// TODO
            break;

    }
}

void Engine::checkInput()
{
    double dt = frameTime_ / 16; // delta time

    // walk forward/backwards
    if (PRESSED(VK_UP) || PRESSED('W'))
    {
        auto newX = player_.x + player_.moveSpeed * cos(player_.rot) * dt;
        auto newY = player_.y + player_.moveSpeed * sin(player_.rot) * dt;
        updatePlayerPos(newX, newY);
    }
    if (PRESSED(VK_DOWN) || PRESSED('S'))
    {
        auto newX = player_.x - player_.moveSpeed * cos(player_.rot) * dt;
        auto newY = player_.y - player_.moveSpeed * sin(player_.rot) * dt;
        updatePlayerPos(newX, newY);
    }

    // turn left/right
    if (PRESSED(VK_LEFT))
    {
        player_.rot = fmod(player_.rot - player_.rotSpeed * dt, rot360);
    }
    if (PRESSED(VK_RIGHT))
    {
        player_.rot = fmod(player_.rot + player_.rotSpeed * dt, rot360);
    }

    // strafe left/right
    if (PRESSED('A'))
    {
        auto angle = player_.rot - rot90;
        auto newX = player_.x + player_.moveSpeed * cos(angle) * dt;
        auto newY = player_.y + player_.moveSpeed * sin(angle) * dt;
        updatePlayerPos(newX, newY);
    }
    if (PRESSED('D'))
    {
        auto angle = player_.rot + rot90;
        auto newX = player_.x + player_.moveSpeed * cos(angle) * dt;
        auto newY = player_.y + player_.moveSpeed * sin(angle) * dt;
        updatePlayerPos(newX, newY);
    }

    // run while shift is pressed
    if (PRESSED(VK_SHIFT))
    {
        player_.moveSpeed = Player::moveSpeedRun;
        player_.rotSpeed = Player::rotSpeedRun;

        // fov effect
        if (player_.fov < Player::fovRun)
            player_.fov += Player::fovStep * dt;

    }
    else
    {
        player_.moveSpeed = Player::moveSpeedWalk;
        player_.rotSpeed = Player::rotSpeedWalk;

        // fov effect
        if (player_.fov > Player::fovWalk)
            player_.fov -= Player::fovStep * dt;

    }
}

void Engine::updatePlayerPos(double x, double y)
{
    /// TODO: implement collision offset

    if (level_.levelMap[level_.height * (int) player_.y + (int) x] == 0)
        player_.x = x;

    if (level_.levelMap[level_.height * (int) y + (int) player_.x] == 0)
        player_.y = y;

}

void Engine::render()
{
    static int counter = 0;

    HDC hdc = GetDC(hwnd_);

    // prepare for double buffering
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmp = CreateCompatibleBitmap(hdc, cRect_.right, cRect_.bottom);
    HBITMAP hbmpOld = (HBITMAP) SelectObject(hdcMem, hbmp);

    /// rendering

    switch (gameState_)
    {
        case GameState::MENU:
            /// TODO: main menu
            break;

        case GameState::GAME:

            castRays(hdcMem);

            if (miniMap_)
                drawMiniMap(hdcMem);

            if (fps_)
            {
                static string fps;

                if (!counter)
                {
                    string out = to_string(1000.0 / frameTime_);
                    if (out != "inf")
                        fps = out;
                }

                TextOut(hdcMem, cRect_.right - 60, 20, fps.c_str(), 5);
                counter = (counter + 1) % 10;
            }

            break;

        case GameState::PAUSE:
            /// TODO: pause screen
            break;

    }

    ///

    BitBlt(hdc, 0, 0, cRect_.right, cRect_.bottom,
           hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmpOld);

    DeleteDC(hdcMem);
    DeleteObject(hbmp);

    ReleaseDC(hwnd_, hdc);
}

void Engine::drawMiniMap(HDC hdc)
{
    size_t x, y;

    // mini map
    SelectObject(hdc, blackPen_);
    SelectObject(hdc, whiteBrush_);
    for (auto i = 0u; i < level_.height; ++i)
    {
        for (auto j = 0u; j < level_.width; ++j)
        {
            if (level_.levelMap[level_.height * i + j])
            {
                x = j * miniMapScale;
                y = i * miniMapScale;
                Rectangle(hdc, x, y,
                          x + miniMapScale, y + miniMapScale);
            }
        }
    }

    // player
    SelectObject(hdc, greenBrush_);
    x = player_.x * miniMapScale;
    y = player_.y * miniMapScale;
    Rectangle(hdc, x - miniMapScale / 2, y - miniMapScale / 2,
              x + miniMapScale / 2, y + miniMapScale / 2);

    // rotation indicator
    SelectObject(hdc, greenPen_);
    MoveToEx(hdc, x, y, NULL);
    x = 2 * miniMapScale * cos(player_.rot) + x;
    y = 2 * miniMapScale * sin(player_.rot) + y;
    LineTo(hdc, x, y);
}

void Engine::castRays(HDC hdc) /// ray casting algorithm
{
    HDC hdcTex;

    double viewDistance, angleIncrement;

    double x, y;
    double dX, dY;

    double sine, cosine, slope;

    double vDistance, hDistance, distance;

    double fishbowl;
    double projDistance;

    double yTexSrc, xTexSrc, texSrc;

    bool up, left;
    bool hHit, vHit;

    size_t mapY, mapX;
    size_t offset;

    int hTex, vTex, tex;

    double fovHalf = player_.fov / 2.0;

    viewDistance = projPlaneWidthHalf / tan(fovHalf);
    angleIncrement = player_.fov / projPlaneWidth;

    double angle = player_.rot - fovHalf;
    for (auto i = 0u; i < projPlaneWidth; ++i,
         angle = fmod(angle + angleIncrement, rot360))
    {
        hHit = false;
        vHit = false;

        // determine which direction the angle is facing

        up = (angle < 0.0 && angle > -PI)
            || (angle > PI && angle < rot360);

        left = (angle > rot90 && angle < rot270)
            || (angle < -rot90 && angle > -rot270);

        // calculate sine, cosine and tangent
        sine = sin(angle);
        cosine = cos(angle);
        slope = sine / cosine;

        // used to remove the unwanted 'fishbowl' effect
        fishbowl = cos(player_.rot - angle);

        /// calculate horizontal intersection distance

        dY = up ? -1.0 : 1.0;
        dX = dY / slope;

        y = up ? floor(player_.y) : ceil(player_.y);
        x = player_.x + (y - player_.y) / slope;

        while (y >= 0.0 && y < level_.height && x >= 0.0 && x < level_.width)
        {
            mapY = y + (up ? -1.0 : 0.0);
            mapX = x;

            if ((hTex = level_.levelMap[level_.height * mapY + mapX]))
            {
                hHit = true;
                hDistance = pow(x - player_.x, 2) + pow(y - player_.y, 2);
                xTexSrc = x - mapX;
                break;
            }

            y += dY;
            x += dX;
        }

        /// calculate vertical intersection distance

        dX = left ? -1.0 : 1.0;
        dY = dX * slope;

        x = left ? floor(player_.x) : ceil(player_.x);
        y = player_.y + (x - player_.x) * slope;

        while (y >= 0.0 && y < level_.height && x >= 0.0 && x < level_.width)
        {
            mapX = x + (left ? -1.0 : 0.0);
            mapY = y;

            if ((vTex = level_.levelMap[level_.height * mapY + mapX]))
            {
                vHit = true;
                vDistance = pow(x - player_.x, 2) + pow(y - player_.y, 2);
                yTexSrc = y - mapY;
                break;
            }

            x += dX;
            y += dY;
        }

        /// choose the closest distance

        if (hHit && vHit)
            distance = hDistance < vDistance
                       ? (tex = hTex, texSrc = xTexSrc, offset = 0, hDistance)
                       : (tex = vTex, texSrc = yTexSrc, offset = texSize, vDistance);
        else if (hHit)
            (tex = hTex, texSrc = xTexSrc, offset = 0, distance = hDistance);
        else if (vHit)
            (tex = vTex, texSrc = yTexSrc, offset = texSize, distance = vDistance);
        else
            continue;

        distance = sqrt(distance) * fishbowl;
        projDistance = viewDistance / distance;

        /// map texture to projection plane

        hdcTex = textures_[tex - 1];
        StretchBlt(hdc, i, (projPlaneHeight - projDistance) / 2.0, 1, projDistance,
                   hdcTex, offset + texSrc * texSize, 0, 1, texSize, SRCCOPY);
    }
}
