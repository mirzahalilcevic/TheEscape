#include <algorithm>
#include <iostream>

#include "Engine.hpp"

#define PRESSED(key) GetAsyncKeyState(key) & 0x8000

#define TEXTURE(n)    ("Textures/Texture" + std::to_string(n) + ".bmp").c_str()
#define SPRITE(n)     ("Sprites/Sprite" + std::to_string(n) + ".bmp").c_str()
#define SPRITEMASK(n) ("Sprites/Sprite" + std::to_string(n) + "Mask.bmp").c_str()

#define MS_PER_UPDATE 16
#define DOORSPEED 0.02

using namespace std;

Engine::Engine()
{
    // pens
    blackPen_ = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    whitePen_ = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    darkGrayPen_ = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
    lightGrayPen_ = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    redPen_ = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    greenPen_ = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    bluePen_ = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

    // brushes
    blackBrush_ = CreateSolidBrush(RGB(0, 0, 0));
    whiteBrush_ = CreateSolidBrush(RGB(255, 255, 255));
    darkGrayBrush_ = CreateSolidBrush(RGB(50, 50, 50));
    lightGrayBrush_ = CreateSolidBrush(RGB(100, 100, 100));
    redBrush_ = CreateSolidBrush(RGB(255, 0, 0));
    greenBrush_ = CreateSolidBrush(RGB(0, 255, 0));
    blueBrush_ = CreateSolidBrush(RGB(0, 0, 255));
}

Engine::~Engine()
{
    // pens
    DeleteObject(blackPen_);
    DeleteObject(whitePen_);
    DeleteObject(lightGrayPen_);
    DeleteObject(darkGrayPen_);
    DeleteObject(redPen_);
    DeleteObject(greenPen_);
    DeleteObject(bluePen_);

    // brushes
    DeleteObject(blackBrush_);
    DeleteObject(whiteBrush_);
    DeleteObject(darkGrayBrush_);
    DeleteObject(lightGrayBrush_);
    DeleteObject(redBrush_);
    DeleteObject(greenBrush_);
    DeleteObject(blueBrush_);

    // buffer
    DeleteDC(memoryDC_);
    DeleteObject(memoryBitmap_);

    // bitmaps
    for (auto bitmap : bitmaps_)
        DeleteObject(bitmap);

    // textures
    for (auto hdc : textures_)
        DeleteDC(hdc);

    // sprites
    for (auto hdc : sprites_)
        DeleteDC(hdc);
    for (auto hdc : spriteMasks_)
        DeleteDC(hdc);

}

void Engine::init(HWND hwnd)
{
    HDC screenDC = GetWindowDC(hwnd);

    // window related
    hwnd_ = hwnd;
    GetClientRect(hwnd, &cRect_);

    // buffer
    memoryDC_ = CreateCompatibleDC(screenDC);
    memoryBitmap_ = CreateCompatibleBitmap(screenDC, cRect_.right, cRect_.bottom);
    SelectObject(memoryDC_, memoryBitmap_);

    // projection plane
    projPlaneWidth = cRect_.right;
    projPlaneHeight = cRect_.bottom;

    // load textures
    for (auto i = 0u; i < texNum; ++i)
    {
        auto bitmap = (HBITMAP) LoadImage(NULL, TEXTURE(i + 1), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        bitmaps_[i] = bitmap;

        auto hdc = CreateCompatibleDC(screenDC);
        SelectObject(hdc, bitmap);
        textures_[i] = hdc;
    }

    // load sprites
    for (auto i = 0u; i < spriteNum; ++i)
    {
        auto bitmap = (HBITMAP) LoadImage(NULL, SPRITE(i + 1), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        bitmaps_[i] = bitmap;

        auto hdc = CreateCompatibleDC(screenDC);
        SelectObject(hdc, bitmap);
        sprites_[i] = hdc;

        bitmap = (HBITMAP) LoadImage(NULL, SPRITEMASK(i + 1), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        bitmaps_[i] = bitmap;

        hdc = CreateCompatibleDC(screenDC);
        SelectObject(hdc, bitmap);
        spriteMasks_[i] = hdc;
    }

    // load first level
    loadLevel(1);
}

void Engine::start()
{
    MSG message;

    DWORD currentTime;
    DWORD previousTime = GetTickCount();
    DWORD lag = 0;

    /// game loop
    while (true)
    {
        currentTime = GetTickCount();
        frameTime = currentTime - previousTime;
        previousTime = currentTime;
        lag += frameTime;

        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
                break;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        checkInput();

        while (lag >= MS_PER_UPDATE)
        {
            update();
            lag -= MS_PER_UPDATE;
        }

        render();
    }
}

void Engine::handleMouseMove(int x, int y)
{
    /// TODO: handle mouse move
}
void Engine::handleLButtonDown(int x, int y)
{
    /// TODO: handle left click
}

void Engine::handleKeyDown(int key)
{
    switch (key)
    {
        case 'F': // toggle fps indicator
            fps_ = !fps_;
            break;

        case 'M': // toggle mini map
            miniMap_ = !miniMap_;
            break;

        case VK_ESCAPE: // pause the game
            gameState_ = (gameState_ == GameState::MAINMENU) ? GameState::RUNNING
                                                             : GameState::MAINMENU;
            break;

        case VK_SPACE: // action key
        {
            size_t mapX = player_.x + 1.0 * cos(player_.rot);
            size_t mapY = player_.y + 1.0 * sin(player_.rot);

            auto& block = level_.levelMap[level_.height * mapY + mapX];
            switch (block)
            {
                case door:
                {
                    SetTimer(hwnd_, (int) mapX | ((int) mapY << 16), 5000, NULL);

                    Door& dr = getDoor(mapX, mapY);
                    dr.state = opening;

                    break;
                }
                case openDoor:
                {
                    level_.levelMap[level_.height * mapY + mapX] = door;

                    Door& dr = getDoor(mapX, mapY);
                    dr.state = closing;

                    break;
                }
                case exit:

                    if (level_.number != levelNum)
                        loadLevel(level_.number + 1);
                    else
                        // game finished

                    break;

            }
            break;
        }
    }
}

void Engine::closeDoor(int timerId)
{
    KillTimer(hwnd_, timerId);

    auto mapX = LOWORD(timerId);
    auto mapY = HIWORD(timerId);

    if (mapX == (int) player_.x && mapY == (int) player_.y)
    {
        SetTimer(hwnd_, timerId, 500, NULL);
        return;
    }

    level_.levelMap[level_.height * mapY + mapX] = door;

    try {
        Door& dr = getDoor(mapX, mapY);
        dr.state = closing;
    } catch (...) {}
}

void Engine::checkInput()
{
    if (gameState_ != GameState::RUNNING)
        return;

    player_.rotSpeed = 0.0;
    player_.moveSpeed = 0.0;
    player_.dir = 0;

    // turn right/left
    if (PRESSED(VK_RIGHT))
    {
        player_.rotSpeed += 1.0;
    }
    if (PRESSED(VK_LEFT))
    {
        player_.rotSpeed -= 1.0;
    }

    // walk forward/backwards
    if (PRESSED(VK_UP) || PRESSED('W'))
    {
        player_.moveSpeed += 1.0;
    }
    if (PRESSED(VK_DOWN) || PRESSED('S'))
    {
        player_.moveSpeed -= 1.0;
    }

    // strafe right/left
    if (PRESSED('D'))
    {
        player_.dir += player_.moveSpeed ? player_.moveSpeed
                                         : (player_.moveSpeed = 1.0, 2.0);
    }
    if (PRESSED('A'))
    {
        player_.dir -= player_.moveSpeed ? player_.moveSpeed
                                         : (player_.moveSpeed = 1.0, 2.0);
    }

    // run while shift is pressed
    player_.running = PRESSED(VK_SHIFT);
}

void Engine::update()
{
    if (gameState_ != GameState::RUNNING)
        return;

    player_.rotSpeed *= player_.running ? Player::rotSpeedRun : Player::rotSpeedWalk;
    player_.moveSpeed *= player_.running ? Player::moveSpeedRun : Player::moveSpeedWalk;

    player_.rot = fmod(player_.rot + player_.rotSpeed, rot360);

    auto newX = player_.x + player_.moveSpeed * cos(player_.rot + rot45 * player_.dir);
    auto newY = player_.y + player_.moveSpeed * sin(player_.rot + rot45 * player_.dir);

    // collision detection

    constexpr double collisionRadius = 0.2;

    /// TODO: keep distance of 'collisionRadius' between player and walls

    if (level_.levelMap[level_.height * (int) player_.y + (int) newX] <= 0)
        player_.x = newX;

    if (level_.levelMap[level_.height * (int) newY + (int) player_.x] <= 0)
        player_.y = newY;

    // door animations
    for (auto& dr : doors_)
    {
        switch (dr.state)
        {
            case opening:
                dr.length -= DOORSPEED;
                if (dr.length < 0.0)
                {
                    dr.length = 0.0;
                    dr.state = open;
                    level_.levelMap[level_.height * dr.y + dr.x] = openDoor;
                }

                break;

            case closing:
                dr.length += DOORSPEED;
                if (dr.length > 1.0)
                {
                    dr.length = 1.0;
                    dr.state = closed;
                }

                break;

        }
    }
}

void Engine::render()
{
    HDC hdc = GetDC(hwnd_);

    switch (gameState_)
    {
        case GameState::RUNNING:

            SelectObject(memoryDC_, blackPen_);

            // ceiling
            SelectObject(memoryDC_, darkGrayBrush_);
            Rectangle(memoryDC_, 0, 0, cRect_.right, cRect_.bottom / 2);

            // floor
            SelectObject(memoryDC_, lightGrayBrush_);
            Rectangle(memoryDC_, 0, cRect_.bottom / 2, cRect_.right, cRect_.bottom);

            castRays(memoryDC_);

            if (fps_) displayFps(memoryDC_);

            if (miniMap_) drawMiniMap(memoryDC_);

            break;

        case GameState::MAINMENU:
            /// TODO: main menu
            break;

        case GameState::LEVELEND:
            /// TODO: pause screen
            break;

    }

    BitBlt(hdc, 0, 0, cRect_.right, cRect_.bottom, memoryDC_, 0, 0, SRCCOPY);
    ReleaseDC(hwnd_, hdc);
}

void Engine::drawMiniMap(HDC hdc)
{
    constexpr size_t miniMapScale = 8;

    size_t x, y;

    // mini map
    SelectObject(hdc, blackPen_);
    SelectObject(hdc, whiteBrush_);
    for (auto i = 0u; i < level_.height; ++i)
    {
        for (auto j = 0u; j < level_.width; ++j)
        {
            if (level_.levelMap[level_.height * i + j] > 0)
            {
                x = j * miniMapScale;
                y = i * miniMapScale;
                Rectangle(hdc, x, y, x + miniMapScale, y + miniMapScale);
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

void Engine::castRays(HDC hdc) // ray casting algorithm
{
    SelectObject(hdc, blackPen_);

    double viewDistance, angleIncrement;

    double x, y;
    double dX, dY;

    double sine, cosine, slope;

    double vDistance, hDistance, distance;

    double fishbowl;
    double projDistance;

    double yTexSrc, xTexSrc, texSrc;

    double top;
    double intensity;

    bool up, left;

    size_t mapY, mapX;

    int hTex, vTex, tex;
    int offset;

    viewDistance = (projPlaneWidth >> 1) / tan(fov / 2);
    angleIncrement = fov / projPlaneWidth;

    double angle = player_.rot - fov / 2;
    for (auto i = 0u; i < projPlaneWidth; ++i, angle = fmod(angle + angleIncrement, rot360))
    {
        hDistance = INFINITE;
        vDistance = INFINITE;

        // determine in which direction the angle is facing
        up = (angle < 0.0 && angle > -PI) || (angle > PI && angle < rot360);
        left = (angle > rot90 && angle < rot270) || (angle < -rot90 && angle > -rot270);

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

            if((hTex = level_.levelMap[level_.height * mapY + mapX]) > 0)
            {
                switch (hTex)
                {
                    case door:
                    case exit:
                    case entrance:
                        y += up ? -0.5 : 0.5;
                        x += dX / 2;

                    default:
                        hDistance = pow(x - player_.x, 2) + pow(y - player_.y, 2);
                        xTexSrc = x - mapX;

                }

                if (hTex == door)
                {
                    Door& dr = getDoor(mapX, mapY);
                    if (xTexSrc > dr.length)
                    {
                        // backtrack
                        y -= up ? -0.5 : 0.5;
                        x -= dX / 2;
                    }
                    else
                    {
                        xTexSrc += 1.0 - dr.length;
                        break;
                    }
                }
                else
                {
                    // check if door side
                    mapY += up ? 1 : -1;
                    tex = level_.levelMap[level_.height * mapY + mapX];
                    if (tex == door || tex == exit || tex == entrance || tex < 0)
                        hTex = doorSide;

                    break;
                }
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

            if ((vTex = level_.levelMap[level_.height * mapY + mapX]) > 0)
            {
                switch (vTex)
                {
                    case door:
                    case exit:
                    case entrance:
                        x += left ? -0.5 : 0.5;
                        y += dY / 2;

                    default:
                        vDistance = pow(x - player_.x, 2) + pow(y - player_.y, 2);
                        yTexSrc = y - mapY;

                }

                if (vTex == door)
                {
                    Door& dr = getDoor(mapX, mapY);
                    if (yTexSrc > dr.length)
                    {
                        // backtrack
                        x -= left ? -0.5 : 0.5;
                        y -= dY / 2;
                    }
                    else
                    {
                        yTexSrc += 1.0 - dr.length;
                        break;
                    }
                }
                else
                {
                    // check if door side
                    mapX += left ? 1 : -1;
                    tex = level_.levelMap[level_.height * mapY + mapX];
                    if (tex == door || tex == exit || tex == entrance || tex < 0)
                        vTex = doorSide;

                    break;
                }
            }

            x += dX;
            y += dY;
        }

        /// choose the closest distance

        if (hDistance < vDistance)
        {
            distance = hDistance;
            tex = hTex;
            texSrc = xTexSrc;
            offset = 0;
        }
        else
        {
            distance = vDistance;
            tex = vTex;
            texSrc = yTexSrc;
            offset = 1;
        }

        distance = sqrt(distance) * fishbowl;

        /// map texture onto the projection plane

        projDistance = viewDistance / distance;
        top = (projPlaneHeight - projDistance) / 2.0;

        // draw black if wall is too far away
        if (distance > 16.0)
        {
            MoveToEx(hdc, i, top, NULL);
            LineTo(hdc, i, top + projDistance);
            continue;
        }

        /*
        // lighting intensity
        intensity = 1.0 / distance * 4.0;
        offset = 9 - (int) (intensity * 10);
        if (offset < 0) offset = 0;
        */

        StretchBlt(hdc, i, top, 1, projDistance, textures_[tex - 1],
                   (offset + texSrc) * texSize, 0, 1, texSize, SRCCOPY);

    }
}

void Engine::loadLevel(size_t number)
{
    level_.load(number);

    // load doors
    doors_.clear();
    for (auto i = 0u; i < level_.height; ++i)
    {
        for (auto j = 0u; j < level_.width; ++j)
        {
            if (level_.levelMap[level_.height * i + j] == door)
                doors_.push_back(Door{i, j});

        }
    }
}

Door& Engine::getDoor(size_t x, size_t y)
{
    auto it = find_if(doors_.begin(), doors_.end(),
        [=](const Door& door)
        {
            return x == door.x && y == door.y;
        });

    if (it == doors_.end())
        throw invalid_argument("door not found");

    return *it;
}

void Engine::displayFps(HDC hdc)
{
    static int counter = 0;
    static string fps;

    if (counter == 0)
    {
        string out = to_string(1000.0 / frameTime);
        if (out.find("inf") == string::npos)
            fps = "FPS: " + out;

    }

    TextOut(hdc, cRect_.right - 100, 20, fps.c_str(), 10);

    counter = (counter + 1) % 20;
}
