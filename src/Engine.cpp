#include <cmath>

#include "Engine.hpp"

#define MINI_MAP_SCALE 16

Engine::Engine(HWND hwnd, HBRUSH backgroundBrush)
    : hwnd_(hwnd), backgroundBrush_(backgroundBrush)
{
    blackBrush_ = CreateSolidBrush(RGB(0, 0, 0));
    blackPen_ = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    whitePen_ = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

    level_.load(1);
}

Engine::~Engine()
{
    DeleteObject(blackBrush_);
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

        render();

        frameTime = GetTickCount() - startTime;
        if (frameTime < frameDelay)
            Sleep(frameDelay - frameTime);

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

    SelectObject(hdcMem, backgroundBrush_);
    SelectObject(hdcMem, whitePen_);
    Rectangle(hdcMem, 0, 0, cRect.right, cRect.bottom);

    drawMiniMap(hdcMem);

    BitBlt(hdc, 0, 0, cRect.right, cRect.bottom, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmpOld);
    DeleteDC(hdcMem);
    DeleteObject(hbmp);

    ReleaseDC(hwnd_, hdc);
}

void Engine::drawMiniMap(HDC hdc)
{
    int x, y;

    SelectObject(hdc, blackBrush_);
    for (int i = 0; i < level_.width; ++i)
    {
        for (int j = 0; j < level_.height; ++j)
        {
            if (level_.levelMap[i][j] > 0)
            {
                x = j * MINI_MAP_SCALE;
                y = i * MINI_MAP_SCALE;
                Rectangle(hdc, x, y, x + MINI_MAP_SCALE, y + MINI_MAP_SCALE);
            }
        }
    }

    Rectangle(hdc, player_.x - (MINI_MAP_SCALE >> 2), player_.y - (MINI_MAP_SCALE >> 2),
              player_.x + (MINI_MAP_SCALE >> 2), player_.y + (MINI_MAP_SCALE >> 2));

    SelectObject(hdc, blackPen_);
    x = (MINI_MAP_SCALE + 4) * cos(player_.angle) + player_.x;
    y = (MINI_MAP_SCALE + 4) * sin(player_.angle) + player_.y;
    MoveToEx(hdc, player_.x, player_.y, NULL);
    LineTo(hdc, x, y);
}
