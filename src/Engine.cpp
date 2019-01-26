#include "Engine.hpp"

Engine::Engine(HWND hwnd)
    : hwnd_(hwnd)
{
    level_.load(1);
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

        frameTime = GetTickCount() - startTime;
        if (frameTime < frameDelay)
            Sleep(frameDelay - frameTime);

    }
}
