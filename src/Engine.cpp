#include <cmath>

#include "Engine.hpp"

#define PRESSED(key) GetAsyncKeyState(key) & 0x8000
#define MINI_MAP_SCALE 16

using namespace std;

Engine::Engine()
{
    blackBrush_ = CreateSolidBrush(RGB(0, 0, 0));
    whiteBrush_ = CreateSolidBrush(RGB(255, 255, 255));
    blackPen_ = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    whitePen_ = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

    level_.load(1);
}

Engine::~Engine()
{
    DeleteObject(blackBrush_);
    DeleteObject(whiteBrush_);
    DeleteObject(blackPen_);
    DeleteObject(whitePen_);
}

void Engine::start()
{
    MSG message;
    DWORD startTime, frameTime;

    while (true)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
                break;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        startTime = GetTickCount();

        ///

        checkInput();
        render();

        ///

        frameTime = GetTickCount() - startTime;
        if (frameTime < frameDelay)
            Sleep(frameDelay - frameTime);

    }
}

void Engine::handleInput(RAWINPUT * raw)
{
    // TODO
}

void Engine::checkInput()
{
    if (PRESSED(VK_UP) || PRESSED('W'))
    {
        player_.x += player_.moveSpeed * cos(player_.rot);
        player_.y += player_.moveSpeed * sin(player_.rot);
    }
    if (PRESSED(VK_DOWN) || PRESSED('S'))
    {
        player_.x -= player_.moveSpeed * cos(player_.rot);
        player_.y -= player_.moveSpeed * sin(player_.rot);
    }
    if (PRESSED(VK_LEFT))
    {
        player_.rot -= player_.rotSpeed;
    }
    if (PRESSED(VK_RIGHT))
    {
        player_.rot += player_.rotSpeed;
    }
    if (PRESSED('A'))
    {
        player_.x += player_.moveSpeed * cos(player_.rot - 90.0);
        player_.y += player_.moveSpeed * sin(player_.rot - 90.0);
    }
    if (PRESSED('D'))
    {
        player_.x += player_.moveSpeed * cos(player_.rot + 90.0);
        player_.y += player_.moveSpeed * sin(player_.rot + 90.0);
    }

    if (PRESSED(VK_SHIFT))
    {
        player_.moveSpeed = 2.0;
    }
    else
    {
        player_.moveSpeed = 1.0;
    }
}

void Engine::render()
{
    RECT cRect;
    GetClientRect(hwnd_, &cRect);

    HDC hdc = GetDC(hwnd_);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmp = CreateCompatibleBitmap(hdc, cRect.right, cRect.bottom);
    HBITMAP hbmpOld = (HBITMAP) SelectObject(hdcMem, hbmp);

    SelectObject(hdcMem, whiteBrush_);
    SelectObject(hdcMem, whitePen_);
    Rectangle(hdcMem, 0, 0, cRect.right, cRect.bottom);

    ///

    drawMiniMap(hdcMem);

    ///

    BitBlt(hdc, 0, 0, cRect.right, cRect.bottom, hdcMem, 0, 0, SRCCOPY);
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
    for (auto i = 0u; i < level_.width; ++i)
    {
        for (auto j = 0u; j < level_.height; ++j)
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
    Rectangle(hdc, round(player_.x) - (MINI_MAP_SCALE >> 2), round(player_.y) - (MINI_MAP_SCALE >> 2),
              round(player_.x) + (MINI_MAP_SCALE >> 2), round(player_.y) + (MINI_MAP_SCALE >> 2));

    // rotation indicator
    SelectObject(hdc, blackPen_);
    x = round((MINI_MAP_SCALE + 4) * cos(player_.rot) + player_.x);
    y = round((MINI_MAP_SCALE + 4) * sin(player_.rot) + player_.y);
    MoveToEx(hdc, round(player_.x), round(player_.y), NULL);
    LineTo(hdc, x, y);

    // TODO: cast rays
}
