#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "definitions.hpp" // Contains constants relevent to program

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

char imgLoc[] = "assets/grass_tile.png"; // Location of isometric tile
Image tileImg;
Texture tileTex;

// Function Declarations
void handleEvents();
void drawGame();
void drawTile(int x, int y, Vector2 startPos, int size, float altitude, bool showOutline = false);
unsigned int prepareAssets(char *fileLoc);
Vector2 transform(Vector2 v);

// Entry Point
using namespace std;
int main()
{
    SetTargetFPS(FPS);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(w, h, SCREEN_TITLE);

    if (prepareAssets(imgLoc)) // Load Image, perform relevent pre-operations and load it as a texture.
        cout << "Loaded texture successfully" << "\n";
    else
    {
        cout << "Texture loading failed" << "\n";
        return -1;
    }

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
        gridSize += 1;
    if (IsKeyPressed(KEY_L))
        gridSize -= 1;

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

            drawTile(colIndex,
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

void drawTile(int x, int y, Vector2 startPos, int size, float altitude, bool showOutline)
{

    Vector2 isoCoords = transform({float(x * tileTex.width), float(y * tileTex.height)}); // isometric transformation
    isoCoords.x = startPos.x + (isoCoords.x / 2.f) - (float)(tileTex.width / 2);
    isoCoords.y = startPos.y + (isoCoords.y / 2.f) - (float)(tileTex.height * size / 4);

    isoCoords.y -= altitude; // Makes the tile appear elevated
    if (showOutline)
        DrawRectangleLines((int)isoCoords.x, (int)isoCoords.y, tileTex.width, tileImg.height, RED); // Show outline of tiles

    DrawTexture(tileTex, (int)isoCoords.x, (int)isoCoords.y, fgColor);
}

unsigned int prepareAssets(char *fileLoc)
{
    tileImg = LoadImage(fileLoc);                                   // upload to RAM
    ImageResizeNN(&tileImg, tileImg.width * 2, tileImg.height * 2); // 32x32 -> 64x64 (w/ nearest neighbour)
    tileTex = LoadTextureFromImage(tileImg);                        // upload to VRAM
    UnloadImage(tileImg);                                           // unload from RAM
    cout << "Loaded Texture with width: " << tileTex.width << " and height: " << tileTex.height << "\n";
    return tileTex.id; // 0 if texture loading failed
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