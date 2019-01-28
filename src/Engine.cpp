#include <cmath>
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
    blackPen_ = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    whitePen_ = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    grayPen_ = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));

    level_.load(1);
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
}

void Engine::start()
{
    MSG message;
    DWORD startTime, frameTime;

    while (true)
    {
        startTime = GetTickCount();

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

        frameTime = GetTickCount() - startTime;
        if (frameTime < frameDelay)
            Sleep(frameDelay - frameTime);

    }
}

void Engine::handleLButtonDown(int x, int y)
{
    ///
}

void Engine::checkInput()
{
    constexpr double rot90 = PI / 2;
    constexpr double rot360 = 2 * PI;

    // walk forward/backwards
    if (PRESSED(VK_UP) || PRESSED('W'))
    {
        auto newX = player_.x + player_.moveSpeed * cos(player_.rot);
        auto newY = player_.y + player_.moveSpeed * sin(player_.rot);
        updatePlayerPos(newX, newY);
    }
    if (PRESSED(VK_DOWN) || PRESSED('S'))
    {
        auto newX = player_.x - player_.moveSpeed * cos(player_.rot);
        auto newY = player_.y - player_.moveSpeed * sin(player_.rot);
        updatePlayerPos(newX, newY);
    }

    // turn left/right
    if (PRESSED(VK_LEFT))
    {
        player_.rot = fmod(player_.rot - Player::rotSpeed, rot360);
    }
    if (PRESSED(VK_RIGHT))
    {
        player_.rot = fmod(player_.rot + Player::rotSpeed, rot360);
    }

    // strafe left/right
    if (PRESSED('A'))
    {
        auto angle = player_.rot - rot90;
        auto newX = player_.x + player_.moveSpeed * cos(angle);
        auto newY = player_.y + player_.moveSpeed * sin(angle);
        updatePlayerPos(newX, newY);
    }
    if (PRESSED('D'))
    {
        auto angle = player_.rot + rot90;
        auto newX = player_.x + player_.moveSpeed * cos(angle);
        auto newY = player_.y + player_.moveSpeed * sin(angle);
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

    if (level_.levelMap[(int) player_.y][(int) x] == 0)
        player_.x = x;

    if (level_.levelMap[(int) y][(int) player_.x] == 0)
        player_.y = y;

}

void Engine::render()
{
    HDC hdc = GetDC(hwnd_);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmp = CreateCompatibleBitmap(hdc, cRect_.right, cRect_.bottom);
    HBITMAP hbmpOld = (HBITMAP) SelectObject(hdcMem, hbmp);

    SelectObject(hdcMem, whiteBrush_);
    SelectObject(hdcMem, whitePen_);
    Rectangle(hdcMem, 0, 0, cRect_.right, cRect_.bottom);

    ///

    drawMiniMap(hdcMem);
    castRays();

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
    int x, y;

    // mini map
    SelectObject(hdc, blackBrush_);
    for (auto i = 0u; i < level_.height; ++i)
    {
        for (auto j = 0u; j < level_.width; ++j)
        {
            if (level_.levelMap[i][j] > 0)
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
    SelectObject(hdc, blackPen_);
    MoveToEx(hdc, x, y, NULL);
    x = 2 * MINI_MAP_SCALE * cos(player_.rot) + x;
    y = 2 * MINI_MAP_SCALE * sin(player_.rot) + y;
    LineTo(hdc, x, y);
}

void Engine::castRays()
{
    ///
}
