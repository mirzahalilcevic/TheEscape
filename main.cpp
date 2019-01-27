#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#ifdef __MINGW32__
#   define _WIN32_WINNT 0x0501
#endif // __MINGW32__

#include <tchar.h>
#include <windows.h>

#include <iostream>
#include <stdexcept>

#include "Engine.hpp"
Engine * engine = nullptr;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

TCHAR szClassName[ ] = _T("MainClass");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    ::engine = new Engine();

    HWND hwnd;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (
           0,
           szClassName,
           _T("The Escape"),
           WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           800,
           600,
           HWND_DESKTOP,
           NULL,
           hThisInstance,
           NULL
           );

    ShowWindow (hwnd, nCmdShow);

    ::engine->init(hwnd);
    ::engine->start();

    delete ::engine;
    return 0;
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // static RAWINPUTDEVICE Rid[2];
    // static UINT bufferSize;
    // static BYTE * buffer = nullptr;

    switch (message)
    {
        /*
        case WM_CREATE:

            // keyboard
            Rid[0].usUsagePage = 1;
            Rid[0].usUsage = 6;
            Rid[0].dwFlags = 0;
            Rid[0].hwndTarget = NULL;

            // mouse
            Rid[1].usUsagePage = 1;
            Rid[1].usUsage = 2;
            Rid[1].dwFlags = 0;
            Rid[1].hwndTarget = NULL;

            if (RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
                throw std::invalid_argument("unable to register raw input devices");

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof (RAWINPUTHEADER));
            buffer = new BYTE[bufferSize];

            break;

        case WM_INPUT:
        {
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)buffer, &bufferSize, sizeof (RAWINPUTHEADER));

            RAWINPUT * raw = (RAWINPUT *) buffer;
            ::engine->handleInput(raw);

            break;
        }
        */
        case WM_DESTROY:
            PostQuitMessage (0);
            // delete [] buffer;
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
