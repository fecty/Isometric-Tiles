#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include <string>
#include <vector>
#include <random>

#include "definitions.hpp" // Contains constants relevent to program
using namespace std;

// Globals
int w = SCREEN_WIDTH;
int h = SCREEN_HEIGHT;
Color bgColor = BG_COLOR;
Color fgColor = {static_cast<unsigned char>(255 - bgColor.r),
                 static_cast<unsigned char>(255 - bgColor.g),
                 static_cast<unsigned char>(255 - bgColor.b),
                 255}; // Opposite of background color

int gridSize = GRID_SIZE; // square grid
float amplitude = AMPLITUDE;
float oscilSpeed = OSCIl_SPEED;
unsigned short oscilOption = OSCIL_OPTION; // for different altitude functions

string imgFiles[IMG_ARRAY_SIZE] = {
    "assets/tile_1.png",
    "assets/tile_2.png",
    "assets/tile_3.png",
    "assets/tile_4.png",
    "assets/tile_5.png",
};
size_t imgFilesSize = IMG_ARRAY_SIZE;
Image tileImg;
Texture tileTexArray[IMG_ARRAY_SIZE];
Texture tileTex;

vector<int> tileMap;

// Function Declarations
void handleEvents();
void drawGame();
void drawTile(Texture &tile, int x, int y, Vector2 startPos, int size, float altitude, bool showOutline = false);
unsigned int prepareAssets(string files[], size_t limit);
Vector2 transform(Vector2 v);
void arrangeRandomTiles();

// Entry Point
int main()
{

    SetTargetFPS(FPS);
    SetTraceLogLevel(LOG_ERROR);

    if (FULLSCREEN) // use full screen
    {
        int monitor = GetCurrentMonitor();
        InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), SCREEN_TITLE);
        w = GetScreenWidth();
        h = GetScreenHeight();

        ToggleFullscreen();
    }
    else
    {
        InitWindow(w, h, SCREEN_TITLE);
    }

    if (prepareAssets(imgFiles, imgFilesSize)) // Load Image, perform relevent pre-operations and load it as a texture.
        cout << "Loaded texture successfully" << "\n";
    else
    {
        cout << "Texture loading failed" << "\n";
        return -1;
    }

    tileMap.resize(gridSize * gridSize, 3); // initialize vector with a default value
    arrangeRandomTiles();                   // allocate a normal distribution biased random index to each tile position
    while (!WindowShouldClose())
    {
        if (IsWindowFocused())
            handleEvents();

        drawGame();
    };

    return 0;
}

void handleEvents()
{

    // Amplitude
    if (IsKeyDown(KEY_U))
        amplitude += .5f;
    if (IsKeyDown(KEY_J))
        amplitude -= .5f;

    // Oscillation Speed
    if (IsKeyDown(KEY_I))
        oscilSpeed += .1f;
    if (IsKeyDown(KEY_K))
        oscilSpeed -= .1f;

    // Grid Size
    if (IsKeyPressed(KEY_O))
    {
        gridSize += 1;
        arrangeRandomTiles();
    }

    if (IsKeyPressed(KEY_L))
    {
        gridSize -= 1;
        arrangeRandomTiles();
    }

    if (IsKeyPressed(KEY_ONE))
        oscilOption = 1;
    if (IsKeyPressed(KEY_TWO))
        oscilOption = 2;
    if (IsKeyPressed(KEY_THREE))
        oscilOption = 3;

    // Revert to original values
    if (IsKeyPressed(KEY_SPACE))
    {
        gridSize = GRID_SIZE;
        amplitude = AMPLITUDE;
        oscilSpeed = OSCIl_SPEED;
    }

    amplitude = Clamp(amplitude, 0.f, MAX_AMPLITUDE);
    oscilSpeed = Clamp(oscilSpeed, 0.f, MAX_OSCIL_SPEED);
    gridSize = static_cast<int>(Clamp((float)gridSize, 1.f, float(MAX_GRID_SIZE)));

    // cout << "Grid: " << gridSize << "x" << gridSize << "  ";
    // cout << "Oscillation Speed: " << oscilSpeed << "  ";
    // cout << "Amplitude: " << amplitude << "\n";
};

void drawGame()
{
    BeginDrawing();
    ClearBackground(bgColor);

    Vector2 startPos = {((float)w - (float)tileTex.width) / 2.f, // to center a unit tile to its center
                        (float)h / 2.f};

    for (int rowIndex = 0; rowIndex < gridSize; rowIndex++)
    {
        for (int colIndex = 0; colIndex < gridSize; colIndex++)
        {
            int i = (rowIndex * gridSize) + colIndex;
            tileTex = tileTexArray[tileMap[i]];

            auto getAlt = [&](float speed, float maxAlt, unsigned short option)
            {
                switch (option)
                {
                case 1:
                    return sinf((float)rowIndex + (float)GetTime() * speed) * maxAlt; // along row
                    break;
                case 2:
                    return sinf((float)colIndex + (float)GetTime() * speed) * maxAlt; // along col
                    break;
                case 3:
                default:
                    return sinf((float)rowIndex + (float)GetTime() * speed) * sinf((float)colIndex + (float)GetTime() * speed) * maxAlt; // along both
                }
            };

            drawTile(tileTex,
                     colIndex,
                     rowIndex,
                     startPos,
                     gridSize,
                     getAlt(oscilSpeed, amplitude, oscilOption),
                     false);

            // cout << "Grid: " << gridSize << "x" << gridSize << "  ";
            // cout << "Oscillation Speed: " << oscilSpeed << "  ";
            // cout << "Amplitude: " << amplitude << "\n";
        }
    }
    DrawText(TextFormat("Grid: %dx%d", gridSize, gridSize), 5, 5, 20, fgColor);
    DrawText(TextFormat("Oscillation Speed: %.1f", oscilSpeed), 5, 25, 20, fgColor);
    DrawText(TextFormat("Amplitude: %.1f", amplitude), 5, 45, 20, fgColor);

    DrawText("( I/K ) for Oscillation speed", 5, h - 25, 10, fgColor);
    DrawText("( U/J ) for Amplitude", 5, h - 15, 10, fgColor);
    DrawText("( O/L ) for Grid Size", 5, h - 35, 10, fgColor);
    DrawText("( SPACE ) to Reset", 5, h - 45, 10, fgColor);
    DrawText("( 1, 2, 3... ) for Options", 5, h - 55, 10, fgColor);

    EndDrawing();
};

void drawTile(Texture &tile, int x, int y, Vector2 startPos, int size, float altitude, bool showOutline)
{

    Vector2 isoCoords = transform({float(x * tile.width), float(y * tile.height)}); // isometric transformation
    isoCoords.x = startPos.x + (isoCoords.x / 2.f) - (float)(tile.width / 2);
    isoCoords.y = startPos.y + (isoCoords.y / 2.f) - (float)(tile.height * size / 4);

    isoCoords.y -= altitude; // Makes the tile appear elevated
    if (showOutline)
        DrawRectangleLines((int)isoCoords.x, (int)isoCoords.y, tile.width, tile.height, RED); // Show outline of tiles

    DrawTexture(tile, (int)isoCoords.x, (int)isoCoords.y, fgColor);
}

unsigned int prepareAssets(string files[], size_t limit)
{

    for (int i = 0; i < limit; i++)
    {
        tileImg = LoadImage(files[i].c_str());                          // upload to RAM
        ImageResizeNN(&tileImg, tileImg.width * 2, tileImg.height * 2); // 32x32 -> 64x64 (w/ nearest neighbour)
        tileTexArray[i] = LoadTextureFromImage(tileImg);                // upload to VRAM
        UnloadImage(tileImg);                                           // unload from RAM
        cout << "Loaded Texture (" << files[i] << ") with width: " << tileTexArray[i].width << " and height: " << tileTexArray[i].height << "\n";
        if (!tileTexArray[i].id)
        {
            return tileTexArray[i].id;
        }
    }
    return 1;
}

Vector2 transform(Vector2 v)
{
    /**
    * Apply matrix transformation
    * M = [+1.0 -1.0]
    *     [+0.5 +0.5]

    * v = <x, y>

    * v' = Mv
     */

    return {1.0f * v.x - 1.0f * v.y,
            0.5f * v.x + 0.5f * v.y};
}

void arrangeRandomTiles()
{
    random_device rd;
    mt19937 gen(rd());

    double mean = floor(imgFilesSize / 2.f);
    double stddev = 2;
    normal_distribution<float> dist(mean, stddev);
    for (int i = 0; i < gridSize * gridSize; i++)
    {

        double x;
        do
        {
            x = dist(gen);
        } while (x < 0.0f || x > (float)(imgFilesSize)-1);

        // cout << round(x) << "\n";
        tileMap[i] = int(round(x));
        // tileMap.push_back(GetRandomValue(0, imgFilesSize - 1));
        // tileMap.push_back(2);
    }
}