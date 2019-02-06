#include <algorithm>
#include <iostream>
#include <fstream>

#include "Engine.hpp"

#define PRESSED(key) GetAsyncKeyState(key) & 0x8000

#define TEXTURE(n)    ("Textures/Texture" + std::to_string(n) + ".bmp").c_str()
#define IMAGE(n)      ("Images/Image" + std::to_string(n) + ".bmp").c_str()
#define SPRITE(n)     ("Sprites/Sprite" + std::to_string(n) + ".bmp").c_str()
#define SPRITEMASK(n) ("Sprites/Sprite" + std::to_string(n) + "Mask.bmp").c_str()

#define MS_PER_UPDATE 16
#define DOORSPEED 0.03

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

    // background
    DeleteDC(background_);

    // menus

    for (auto bitmap : menuBitmaps_)
        DeleteObject(bitmap);

    for (auto hdc : menus_)
        DeleteDC(hdc);

    DeleteDC(menu_);
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

    viewDistance_ = (projPlaneWidth >> 1) / tan(fov / 2);
    angleIncrement_ = fov / projPlaneWidth;

    // projection columns
    projCols_.resize(projPlaneWidth);

    // menu items

    int x1 = 4 * 72;
    int y1 = 276 / 2;
    RECT item;
    item.left = x1;
    item.right = x1 + 6 * 72;
    pauseMenuItems_[0] = item;
    for(auto i = 0; i < 3; i++)
    {
        item.top = y1 + i * 100;
        item.bottom = y1 + (i + 1) * 100;
        pauseMenuItems_[i] = item;
    }

    int x2 = 0;
    int y2 = 380;
    item.left = x2;
    item.right = x2 + 4 * 72;
    for(auto i = 0; i < 3; i++)
    {
        item.top = y2 + i * 50;
        item.bottom = y2 + (i + 1) * 50;
        mainMenuItems_[i] = item;
    }

    for(auto i = 0; i < 8; i++)
    {
        auto bitmap = (HBITMAP) LoadImage(NULL, IMAGE(i+1), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        menuBitmaps_[i] = bitmap;

        auto hdc = CreateCompatibleDC(screenDC);
        SelectObject(hdc, bitmap);
        menus_[i] = hdc;
    }

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

    // load background
    {
        auto bitmap = (HBITMAP) LoadImage(NULL, "background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        bitmaps_[bitmaps_.size() - 1] = bitmap;

        auto hdc = CreateCompatibleDC(screenDC);
        SelectObject(hdc, bitmap);
        background_ = hdc;
    }

    gameState_ = GameState::MAIN_MENU;
    menu_ = menus_[4];
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
    switch (gameState_)
    {
        case GameState::RUNNING:
            break;

        case GameState::MAIN_MENU:
            switch(findMenuItem(x, y, GameState::MAIN_MENU))
            {
                case Menu::NEW_GAME:
                    menu_ = menus_[5];
                    break;
                case Menu::LOAD_GAME:
                    menu_ = menus_[6];
                    break;
                case Menu::QUIT:
                    menu_ = menus_[7];
                    break;
                case Menu::NONE:
                    menu_ = menus_[4];
                    break;
                default:
                    break;
            }
            break;

        case GameState::PAUSE_MENU:
            switch(findMenuItem(x, y, GameState::PAUSE_MENU))
            {
                case Menu::RESUME:
                    menu_ = menus_[1];
                    break;
                case Menu::SAVE_GAME:
                    menu_ = menus_[2];
                    break;
                case Menu::MAIN_MENU:
                    menu_ = menus_[3];
                    break;
                default:
                    menu_ = menus_[0];
                    break;
            }
            break;

    }
}

void Engine::handleLButtonDown(int x, int y)
{
    switch (gameState_)
    {
        case GameState::RUNNING:
            break;

        case GameState::MAIN_MENU:
            PlaySound("Sounds/click.wav", NULL, SND_ASYNC | SND_FILENAME);
            switch(findMenuItem(x, y, GameState::MAIN_MENU))
            {
                case Menu::NEW_GAME:
                    loadLevel(1);
                    gameState_ = GameState::RUNNING;
                    break;
                case Menu::LOAD_GAME:
                    loadGame();
                    break;
                case Menu::QUIT:
                    PostQuitMessage(0);
                    break;
                default:
                    break;
            }
            break;

        case GameState::PAUSE_MENU:
            PlaySound("Sounds/click.wav", NULL, SND_ASYNC | SND_FILENAME);
            switch(findMenuItem(x, y, GameState::PAUSE_MENU))
            {
                case Menu::RESUME:
                    gameState_ = GameState::RUNNING;
                    break;
                case Menu::SAVE_GAME:
                    saveGame();
                    gameState_ = GameState::RUNNING;
                    break;
                case Menu::MAIN_MENU:
                {
                    auto choice = MessageBox(
                        hwnd_,
                        "All unsaved progress will be lost",
                        "Are you sure?",
                        MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2
                    );

                    if (choice == IDYES)
                    {
                        menu_ = menus_[4];
                        gameState_ = GameState::MAIN_MENU;
                    }

                    break;
                }
                default:
                    break;
            }
            break;

        default:
            break;

    }
}

void Engine::handleKeyDown(int key)
{
    switch (gameState_)
    {
        case GameState::RUNNING:
            switch (key)
            {
                case 'F': // toggle fps indicator
                    fps_ = !fps_;
                    break;

                case 'M': // toggle mini map
                    miniMap_ = !miniMap_;
                    break;

                case VK_ESCAPE: // pause the game
                    PlaySound("Sounds/click.wav", NULL, SND_ASYNC | SND_FILENAME);
                    gameState_ = GameState::PAUSE_MENU;
                    menu_ = menus_[0];
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
                            PlaySound("Sounds/door.wav", NULL, SND_ASYNC | SND_FILENAME);

                            SetTimer(hwnd_, (int) mapX | ((int) mapY << 16), 5000, NULL);

                            Door& dr = getDoor(mapX, mapY);
                            dr.state = opening;

                            break;
                        }
                        case openDoor:
                        {
                            PlaySound("Sounds/door.wav", NULL, SND_ASYNC | SND_FILENAME);

                            level_.levelMap[level_.height * mapY + mapX] = door;

                            Door& dr = getDoor(mapX, mapY);
                            dr.state = closing;

                            break;
                        }
                        case exit:

                            PlaySound("Sounds/door.wav", NULL, SND_ASYNC | SND_FILENAME);

                            if (level_.number != levelNum)
                                loadLevel(level_.number + 1);
                            else
                                // game finished

                            break;

                    }
                    break;
                }
            }
            break;

        case GameState::PAUSE_MENU:
            if (key == VK_ESCAPE)
            {
                PlaySound("Sounds/click.wav", NULL, SND_ASYNC | SND_FILENAME);
                gameState_ = GameState::RUNNING;
            }

            break;

        default:
            break;

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

    try {

        Door& dr = getDoor(mapX, mapY);
        dr.state = closing;

        auto& block = level_.levelMap[level_.height * mapY + mapX];
        if (block != door)
        {
            PlaySound("Sounds/door.wav", NULL, SND_ASYNC | SND_FILENAME);
            block = door;
        }

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
    static bool firstTime = true;

    static size_t animationCounter = 0;
    ++animationCounter;

    if (gameState_ != GameState::RUNNING)
        return;

    // stamina
    if (player_.running && player_.moveSpeed != 0.0)
    {
        player_.stamina -= 0.8;
        if (player_.stamina < 0.0)
        {
            player_.running = false;
            player_.stamina = 0.0;
        }
        else if (player_.stamina < 1.0)
        {
            if (firstTime)
            {
                PlaySound("Sounds/sigh.wav", NULL, SND_ASYNC | SND_FILENAME);
                firstTime = false;
            }
        }
    }
    else
    {
        firstTime = true;
        player_.stamina += 0.5;
        if (player_.stamina > Player::fullStamina)
            player_.stamina = Player::fullStamina;

    }

    player_.rotSpeed *= player_.running ? Player::rotSpeedRun : Player::rotSpeedWalk;
    player_.moveSpeed *= player_.running ? Player::moveSpeedRun : Player::moveSpeedWalk;

    player_.rot = fmod(player_.rot + player_.rotSpeed, rot360);

    auto angle = player_.rot + rot45 * player_.dir;
    auto cosine = cos(angle);
    auto sine = sin(angle);

    auto newX = player_.x + player_.moveSpeed * cosine;
    auto newY = player_.y + player_.moveSpeed * sine;

    // collision detection

    constexpr double collisionRadius = 0.05;

    auto block = level_.height * (int) player_.y + (int) player_.x;

    auto C  = level_.levelMap[block]                    <= 0; // current block - free space
    auto L  = level_.levelMap[block - 1]                 > 0; // left block - wall
    auto R  = level_.levelMap[block + 1]                 > 0; // right block - wall
    auto T  = level_.levelMap[block - level_.height]     > 0; // ...
    auto D  = level_.levelMap[block + level_.height]     > 0;
    auto LT = level_.levelMap[block - level_.height - 1] > 0;
    auto RT = level_.levelMap[block - level_.height + 1] > 0;
    auto LD = level_.levelMap[block + level_.height - 1] > 0;
    auto RD = level_.levelMap[block + level_.height + 1] > 0;
    auto blockNumberH = block%level_.height;
    auto blockNumberV = block/level_.height;
    auto newXRight = newX + collisionRadius;
    auto newXLeft = newX - collisionRadius;
    auto newYTop = newY - collisionRadius;
    auto newYDown = newY + collisionRadius;
    auto RCol = newXRight >= (blockNumberH + 1);
    auto LCol = newXLeft <= blockNumberH;
    auto TCol = newYTop <= blockNumberV;
    auto DCol = newYDown >= (blockNumberV + 1);

    if ((LT && LCol && TCol && !L && !T) || (RT && RCol && TCol && !R && !T)
        || (LD && LCol && DCol && !L && !D) || (RD && RCol && DCol && !R && !D)) {}
    else
    {
        if ((R && RCol) || (L && LCol)) {}
        else player_.x = newX;
        if ((T && TCol) || (D && DCol)) {}
        else player_.y = newY;
    }

    /*
    double colRad = player_.moveSpeed < 0.0 ? -collisionRadius : collisionRadius;

    bool up = (angle < 0.0 && angle > -PI) || (angle > PI && angle < rot360);
    bool left = (angle > rot90 && angle < rot270) || (angle < -rot90 && angle > -rot270);

    int collisionX = newX + (left ? -colRad : colRad);
    int collisionY = player_.y + (up ? colRad : -colRad);

    if (level_.levelMap[level_.height * collisionY + collisionX] <= 0)
        player_.x = newX;

    collisionY = newY + (up ? -colRad : colRad);
    collisionX = player_.x + (left ? colRad : -colRad);

    if (level_.levelMap[level_.height * collisionY + collisionX] <= 0)
        player_.y = newY;
    */

    // door animation fsm
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

    // enemy AI
    for_each(enemies_.begin(), enemies_.end(),
        [this](Enemy& enemy)
        {
            static bool firstDetect = true;

            constexpr double detectionRadius = pow(5.0, 2);
            constexpr double contactRadius = pow(0.5, 2);
            constexpr size_t animationSpeed = 8;

            double newX, newY;
            double distance = pow(enemy.x - player_.x, 2) + pow(enemy.y - player_.y, 2);

            if (distance < contactRadius)
            {
                PlaySound("Sounds/grunt.wav", NULL, SND_ASYNC | SND_FILENAME);
                firstDetect = true;

                if (--player_.lives == 0)
                {
                    /// TODO: game over
                }

                level_.load(level_.number);
                player_.stamina = Player::fullStamina;

                return;
            }
            else if (distance < detectionRadius)
            {
                if (firstDetect)
                {
                    PlaySound("Sounds/stop.wav", NULL, SND_ASYNC | SND_FILENAME);
                    firstDetect = false;
                }

                if (animationCounter % animationSpeed == 0)
                    enemy.offset = enemy.offset % 4 + 1;

                enemy.rot = atan2(player_.y - enemy.y, player_.x - enemy.x);
                newX = enemy.x + Enemy::moveSpeed * cos(enemy.rot);
                newY = enemy.y + Enemy::moveSpeed * sin(enemy.rot);
            }
            else
            {
                firstDetect = true;
                distance = pow(enemy.x - enemy.startX, 2) + pow(enemy.y - enemy.startY, 2);
                if (distance > 0.01)
                {
                    if (animationCounter % animationSpeed == 0)
                        enemy.offset = enemy.offset % 4 + 1;

                    // return to start position
                    enemy.rot = atan2(enemy.startY - enemy.y, enemy.startX - enemy.x);
                    newX = enemy.x + Enemy::moveSpeed * cos(enemy.rot);
                    newY = enemy.y + Enemy::moveSpeed * sin(enemy.rot);
                }
                else
                {
                    enemy.offset = 0;
                    enemy.rot = enemy.startRot;
                    return;
                }

            }

            // collision detection

            if (level_.levelMap[level_.height * (int) newY + (int) newX] == door)
            {
                // open door if it's in the way

                SetTimer(hwnd_, (int) newX | ((int) newY << 16), 5000, NULL);

                Door& dr = getDoor(newX, newY);
                dr.state = opening;
            }

            if (level_.levelMap[level_.height * (int) enemy.y + (int) newX] <= 0)
                enemy.x = newX;

            if (level_.levelMap[level_.height * (int) newY + (int) enemy.x] <= 0)
                enemy.y = newY;

        });

    // lives
    for_each(lives_.begin(), lives_.end(),
        [this](Life& life)
        {
            constexpr double contactRadius = pow(1.0, 2);

            if (player_.lives == Player::maxLives)
                return;

            if (!(life.visible))
                return;

            double distance = pow(life.x - player_.x, 2) + pow(life.y - player_.y, 2);
            if (distance < contactRadius)
            {
                PlaySound("Sounds/life.wav", NULL, SND_ASYNC | SND_FILENAME);
                life.visible = false;
                player_.lives += 1;
            }
        });
}

void Engine::render()
{
    HDC hdc = GetDC(hwnd_);

    switch (gameState_)
    {
        case GameState::RUNNING:

            SelectObject(memoryDC_, blackPen_);

            BitBlt(memoryDC_, 0, 0, cRect_.right, cRect_.bottom, background_, 0, 0, SRCCOPY);

            drawScene(memoryDC_);

            drawHud(memoryDC_);

            if (fps_) displayFps(memoryDC_);

            if (miniMap_) drawMiniMap(memoryDC_);

            break;

        case GameState::MAIN_MENU:
        case GameState::PAUSE_MENU:
            BitBlt(memoryDC_, 0, 0, cRect_.right, cRect_.bottom, menu_, 0, 0, SRCCOPY);
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

    // enemies
    SelectObject(hdc, redBrush_);
    for_each(enemies_.cbegin(), enemies_.cend(),
        [&](const Enemy& enemy)
        {
            x = enemy.x * miniMapScale;
            y = enemy.y * miniMapScale;
            Rectangle(hdc, x - miniMapScale / 2, y - miniMapScale / 2,
                      x + miniMapScale / 2, y + miniMapScale / 2);

        });

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

void Engine::castRays() // ray casting algorithm
{
    double x, y;
    double dX, dY;
    double sine, cosine, slope;
    double yTexSrc, xTexSrc, texSrc;
    double vDistance, hDistance, distance;
    double fishbowl;
    double intensity;
    bool up, left;
    size_t mapY, mapX;
    int hTex, vTex, tex;
    int offset;

    double angle = player_.rot - fov / 2;
    for (auto i = 0u; i < projPlaneWidth; ++i, angle = fmod(angle + angleIncrement_, rot360))
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
                        hDistance = INFINITE;
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
                        vDistance = INFINITE;
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
        }
        else
        {
            distance = vDistance;
            tex = vTex;
            texSrc = yTexSrc;
        }
        distance = sqrt(distance);

        // lighting
        intensity = 1.0 / distance * 32;
        offset = 20 - intensity;
        if (offset < 0) offset = 0;

        projCols_[i] = ProjInfo{i, distance, fishbowl, texSrc, tex, 0, offset, 0};
    }
}

void Engine::drawScene(HDC hdc)
{
    castRays();

    vector<ProjInfo> graphics;
    copy(projCols_.cbegin(), projCols_.cend(), back_inserter(graphics));

    for_each(enemies_.cbegin(), enemies_.cend(),
        [this, &graphics](const Enemy& enemy)
        {
            double angle = atan2(enemy.y - player_.y, enemy.x - player_.x);
            double rot = player_.rot;

            if (angle < 0.0)
                angle = angle + rot360;

            if (rot < 0.0)
                rot = rot + rot360;

            int col = (angle - (player_.rot - fov / 2)) / angleIncrement_;
            if (col < 0 || col >= projPlaneWidth)
                return;

            double distance = pow(enemy.x - player_.x, 2) + pow(enemy.y - player_.y, 2);
            distance = sqrt(distance);

            if (distance < 1.0)
                return;

            double xOffset = fmod((enemy.rot - angle) + PI, rot360);
            xOffset = fmod(xOffset / rot45 + 0.5, 8);

            auto&& info = ProjInfo{col, distance, cos(player_.rot - angle), 0.0, 0, 0, (int) xOffset, enemy.offset};
            graphics.push_back(std::move(info));
        });

    for_each(lives_.cbegin(), lives_.cend(),
        [this, &graphics](const Life& life)
        {
            if (!(life.visible))
                return;

            double angle = atan2(life.y - player_.y, life.x - player_.x);
            double rot = player_.rot;

            if (angle < 0.0)
                angle = angle + rot360;

            if (rot < 0.0)
                rot = rot + rot360;

            int col = (angle - (rot - fov / 2)) / angleIncrement_;
            if (col < 0 || col >= projPlaneWidth)
                return;

            double distance = pow(life.x - player_.x, 2) + pow(life.y - player_.y, 2);
            distance = sqrt(distance);

            if (distance < 1.5)
                return;

            auto&& info = ProjInfo{col, distance, cos(player_.rot - angle), 0.0, 0, 1, 0, 0};
            graphics.push_back(std::move(info));
        });

    // sort by distance
    sort(graphics.begin(), graphics.end(),
        [](const ProjInfo& info1, const ProjInfo& info2)
        {
            return info1.distance > info2.distance;
        });

    SelectObject(hdc, blackBrush_);

    double projDistance, top;
    for (const auto& info : graphics)
    {
        projDistance = viewDistance_ / (info.distance * info.fishbowl);
        top = (projPlaneHeight - projDistance) / 2.0;

        // draw black if wall is too far away
        if (info.distance > 16.0)
        {
            if (info.tex)
            {
                MoveToEx(hdc, info.col, top, NULL);
                LineTo(hdc, info.col, top + projDistance);
            }
            continue;
        }

        if (info.tex) // wall
        {
            StretchBlt(hdc, info.col, top, 1, projDistance, textures_[info.tex - 1],
                       (info.xOffset + info.texSrc) * texSize, 0, 1, texSize, SRCCOPY);

        }
        else // sprite
        {
            StretchBlt(hdc, info.col - projDistance / 2, top, projDistance, projDistance,
                       spriteMasks_[info.sprite], info.xOffset * texSize, info.yOffset * texSize,
                       texSize, texSize, SRCAND);

            StretchBlt(hdc, info.col - projDistance / 2, top, projDistance, projDistance,
                       sprites_[info.sprite], info.xOffset * texSize, info.yOffset * texSize,
                       texSize, texSize, SRCPAINT);

        }
    }
}

void Engine::drawHud(HDC hdc)
{
    string text;

    // level number
    text = string("LEVEL ") + to_string(level_.number);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOut(hdc, cRect_.right - 80, cRect_.bottom - 35, text.c_str(), text.length());

    // stamina bar
    SelectObject(hdc, greenBrush_);
    Rectangle(hdc, 20, cRect_.bottom - 40, 20 + (player_.stamina * 2), cRect_.bottom - 20);

    // lives
    for (auto i = 1u; i <= player_.lives; ++i)
    {
        BitBlt(hdc, i * 25, cRect_.bottom - 65, 16, 16,
                   spriteMasks_[2], 0, 0, SRCAND);

        BitBlt(hdc, i * 25, cRect_.bottom - 65, 16, 16,
                   spriteMasks_[2], 0, 0, SRCAND);

    }
}

void Engine::saveGame()
{
    CreateDirectory("Saves", NULL);

    time_t now = time(0);
    tm *ltm = localtime(&now);

    string name = to_string(1900 + ltm->tm_year) + "-"
        + to_string(1 + ltm->tm_mon) + "-"
        + to_string(ltm->tm_mday) + "_"
        + to_string(ltm->tm_hour)
        + to_string(ltm->tm_min)
        + to_string(ltm->tm_sec)
        + ".save";

    ofstream saveFile("Saves/" + name);

    saveFile << level_.number << " ";

    // player
    saveFile << player_.lives << " ";
    saveFile << player_.stamina << " ";
    saveFile << player_.x << " ";
    saveFile << player_.y << " ";
    saveFile << player_.rot << " ";

    // enemies
    for (const auto& enemy : enemies_)
    {
        saveFile << enemy.x << " ";
        saveFile << enemy.y << " ";
        saveFile << enemy.rot << " ";
    }

    // lives
    for (const auto& life : lives_)
        saveFile << life.visible << " ";

    saveFile.close();
}

void Engine::loadGame()
{
    char fileName[255] = "\0";
    char filter[] = "Save files (*.save)\0*.save\0\0";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;

    bool isOpen = GetOpenFileName(&ofn);
    if (isOpen)
    {
        ifstream saveFile{string(fileName)};
        if (saveFile.is_open())
        {
            int levelNum;
            saveFile >> levelNum;
            loadLevel(levelNum);

            // player
            saveFile >> player_.lives;
            saveFile >> player_.stamina;
            saveFile >> player_.x;
            saveFile >> player_.y;
            saveFile >> player_.rot;

            // enemies
            for (auto& enemy : enemies_)
            {
                saveFile >> enemy.x;
                saveFile >> enemy.y;
                saveFile >> enemy.rot;
            }

            // lives
            for (auto& life : lives_)
                saveFile >> life.visible;

            gameState_ = GameState::RUNNING;
        }

        saveFile.close();
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

    TextOut(hdc, cRect_.right - 95, 20, fps.c_str(), 10);

    counter = (counter + 1) % 20;
}

Menu Engine::findMenuItem(int x, int y, GameState state)
{
    switch(state)
    {
        case GameState::MAIN_MENU:
            if((x >= mainMenuItems_[0].left) && (x <= mainMenuItems_[0].right) && (y >= mainMenuItems_[0].top) && (y <= mainMenuItems_[0].bottom ))
                return Menu::NEW_GAME;
            else if((x >= mainMenuItems_[1].left && x <= mainMenuItems_[1].right && y >= mainMenuItems_[1].top && y <= mainMenuItems_[1].bottom ))
                return Menu::LOAD_GAME;
            else if((x >= mainMenuItems_[2].left && x <= mainMenuItems_[2].right && y >= mainMenuItems_[2].top && y <= mainMenuItems_[2].bottom ))
                return Menu::QUIT;
            else
                return Menu::NONE;

            break;

        case GameState::PAUSE_MENU:
            if((x >= pauseMenuItems_[0].left && x <= pauseMenuItems_[0].right && y >= pauseMenuItems_[0].top && y <= pauseMenuItems_[0].bottom ))
                return Menu::RESUME;
            else if((x >= pauseMenuItems_[1].left && x <= pauseMenuItems_[1].right && y >= pauseMenuItems_[1].top && y <= pauseMenuItems_[1].bottom ))
                return Menu::SAVE_GAME;
            else if((x >= pauseMenuItems_[2].left && x <= pauseMenuItems_[2].right && y >= pauseMenuItems_[2].top && y <= pauseMenuItems_[2].bottom ))
                return Menu::MAIN_MENU;
            else
                return Menu::NONE;
            break;

        default:
            return Menu::NONE;

    }
}
