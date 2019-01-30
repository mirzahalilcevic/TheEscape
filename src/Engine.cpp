#include <iostream>

#include "Engine.hpp"

#define PRESSED(key) GetAsyncKeyState(key) & 0x8000

#define MINI_MAP_SCALE 16
#define PLAYER_SCALE 4

using namespace std;

Engine::Engine()
{
    blackBrush_ = CreateSolidBrush(RGB(0, 0, 0));
    whiteBrush_ = CreateSolidBrush(RGB(255, 255, 255));
    blackPen_ = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    whitePen_ = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    grayPen_ = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
}

Engine::~Engine()
{
    DeleteObject(blackBrush_);
    DeleteObject(whiteBrush_);
    DeleteObject(blackPen_);
    DeleteObject(whitePen_);
    DeleteObject(grayPen_);
}

void Engine::init(HWND hwnd)
{
    hwnd_ = hwnd;
    GetClientRect(hwnd_, &cRect_);

    level_.load(1);
}

void Engine::start()
{
    MSG message;
    DWORD startTime, lastTime;

    lastTime = GetTickCount();
    while (true)
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

        checkInput();
        render();

        ///

        lastTime = startTime;
        // frameTime = GetTickCount() - startTime;
        // if (frameTime < frameDelay)
        //     Sleep(frameDelay - frameTime);

    }
}

void Engine::handleLButtonDown(int x, int y)
{
    ///
}

void Engine::checkInput()
{
    double dt = frameTime_ / 16;

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
        player_.rot = fmod(player_.rot - Player::rotSpeed * dt, rot360);
    }
    if (PRESSED(VK_RIGHT))
    {
        player_.rot = fmod(player_.rot + Player::rotSpeed * dt, rot360);
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
        player_.moveSpeed = Player::runSpeed;
    }
    else
    {
        player_.moveSpeed = Player::walkSpeed;
    }
}

void Engine::updatePlayerPos(double x, double y)
{
    // TODO: implement collision offset

    if (level_.levelMap[level_.height * (int) player_.y + (int) x] == 0)
        player_.x = x;

    if (level_.levelMap[level_.height * (int) y + (int) player_.x] == 0)
        player_.y = y;

}

void Engine::render()
{
    HDC hdc = GetDC(hwnd_);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmp = CreateCompatibleBitmap(hdc, cRect_.right, cRect_.bottom);
    HBITMAP hbmpOld = (HBITMAP) SelectObject(hdcMem, hbmp);

    ///

    // drawMiniMap(hdcMem);
    castRays(hdcMem);

    ///

    BitBlt(hdc, 0, 0, cRect_.right, cRect_.bottom, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmpOld);

    DeleteDC(hdcMem);
    DeleteObject(hbmp);
    DeleteObject(hbmpOld);

    ReleaseDC(hwnd_, hdc);
}

void Engine::drawMiniMap(HDC hdc)
{
    SelectObject(hdc, whiteBrush_);
    SelectObject(hdc, whitePen_);
    Rectangle(hdc, 0, 0, cRect_.right, cRect_.bottom);

    int x, y;

    // mini map
    SelectObject(hdc, blackBrush_);
    for (auto i = 0u; i < level_.height; ++i)
    {
        for (auto j = 0u; j < level_.width; ++j)
        {
            if (level_.levelMap[level_.height * i + j] > 0)
            {
                x = j * MINI_MAP_SCALE;
                y = i * MINI_MAP_SCALE;
                Rectangle(hdc, x, y, x + MINI_MAP_SCALE, y + MINI_MAP_SCALE);
            }
        }
    }

    // player
    x = player_.x * MINI_MAP_SCALE;
    y = player_.y * MINI_MAP_SCALE;
    Rectangle(hdc, x - PLAYER_SCALE, y - PLAYER_SCALE,
              x + PLAYER_SCALE, y + PLAYER_SCALE);

    // rotation indicator
    // SelectObject(hdc, blackPen_);
    // MoveToEx(hdc, x, y, NULL);
    // x = 2 * MINI_MAP_SCALE * cos(player_.rot) + x;
    // y = 2 * MINI_MAP_SCALE * sin(player_.rot) + y;
    // LineTo(hdc, x, y);
}

void Engine::castRays(HDC hdc)
{
    SelectObject(hdc, grayPen_);

    double x, y;
    double dX, dY;
    double sine, cosine, slope;
    double vDistance, hDistance, distance;
    double fishbowl;
    double projDistance;

    bool up, left;
    bool hHit, vHit;
    size_t mapX, mapY;
    size_t y1, y2;

    double angle = player_.rot - Player::fov / 2;
    for (auto i = 0u; i < projPlaneWidth; ++i, angle = fmod(angle + angleIncrement, rot360))
    {
        hHit = false;
        vHit = false;

        up = (angle < 0.0 && angle > -PI) || (angle > PI && angle < rot360);
        left = (angle > rot90 && angle < rot270) || (angle < -rot90 && angle > -rot270);

        sine = sin(angle);
        cosine = cos(angle);
        slope = sine / cosine;
        fishbowl = cos(player_.rot - angle);

        /// horizontal intersection

        dY = up ? -1.0 : 1.0;
        dX = dY / slope;

        y = up ? floor(player_.y) : ceil(player_.y);
        x = player_.x + (y - player_.y) / slope;

        while (y >= 0.0 && y < level_.height && x >= 0.0 && x < level_.width)
        {
            mapY = y + (up ? -1.0 : 0.0);
            mapX = x;

            if (level_.levelMap[level_.height * mapY + mapX])
            {
                hHit = true;
                hDistance = pow(x - player_.x, 2) + pow(y - player_.y, 2);
                break;
            }

            y += dY;
            x += dX;
        }

        /// vertical intersection

        dX = left ? -1.0 : 1.0;
        dY = dX * slope;

        x = left ? floor(player_.x) : ceil(player_.x);
        y = player_.y + (x - player_.x) * slope;

        while (y >= 0.0 && y < level_.height && x >= 0.0 && x < level_.width)
        {
            mapX = x + (left ? -1.0 : 0.0);
            mapY = y;

            if (level_.levelMap[level_.height * mapY + mapX])
            {
                vHit = true;
                vDistance = pow(x - player_.x, 2) + pow(y - player_.y, 2);
                break;
            }

            x += dX;
            y += dY;
        }

        if (hHit && vHit)
            distance = hDistance < vDistance ? hDistance : vDistance;
        else if (hHit)
            distance = hDistance;
        else if (vHit)
            distance = vDistance;
        else
            continue;

        distance = sqrt(distance) * fishbowl;

        /// draw
        /*
        int lineX, lineY;
        int playerX = MINI_MAP_SCALE * player_.x;
        int playerY = MINI_MAP_SCALE * player_.y;
        SelectObject(hdc, grayPen_);
        MoveToEx(hdc, playerX, playerY, NULL);
        lineX = MINI_MAP_SCALE * distance * cosine + playerX;
        lineY = MINI_MAP_SCALE * distance * sine + playerY;
        LineTo(hdc, lineX, lineY);
        */

        projDistance = viewDistance / distance;
        y1 = (projPlaneHeight - projDistance) / 2;
        y2 = y1 + projDistance;

        MoveToEx(hdc, i, y1, NULL);
        LineTo(hdc, i, y2);
    }
}
