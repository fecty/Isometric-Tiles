#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "definitions.hpp"

int w = SCREEN_WIDTH;
int h = SCREEN_HEIGHT;
Color bgColor = BG_COLOR;

char imgLoc[] = "assets/grass_tile.png";
Image tileImg;
Texture tileTex;

void HandleEvents();
void DrawGame();
unsigned int prepareAssets(char *fileLoc);
using namespace std;

int main()
{
    SetTargetFPS(FPS);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(w, h, SCREEN_TITLE);

    if (prepareAssets(imgLoc))
        cout << "Loaded texture successfully" << "\n";
    else
        cout << "Texture loading failed" << "\n";

    while (!WindowShouldClose())
    {
        if (IsWindowFocused())
            HandleEvents();
        DrawGame();
    };
    return 0;
}

void HandleEvents() {

};

Vector2 transform(Vector2 v)
{
    float x = v.x;
    float y = v.y;

    return {1.0 * x - 1.0 * y,
            0.5 * x + 0.5 * y};
}

void drawTile(int x, int y, Vector2 startPos, int gridSize, float altitude)
{

    Vector2 isoCoords = transform({x * tileTex.width, y * tileTex.height});
    isoCoords.x = startPos.x + (isoCoords.x / 2.f) - (tileTex.width / 2.f);
    isoCoords.y = startPos.y + (isoCoords.y / 2.f) - (tileTex.height * gridSize / 4.f);

    isoCoords.y -= altitude;
    // DrawRectangleLines(isoCoords.x, isoCoords.y, tileTex.width, tileImg.height, RED);
    DrawTexture(tileTex, isoCoords.x, isoCoords.y, WHITE);
}

void DrawGame()
{
    BeginDrawing();
    ClearBackground(bgColor);

    int gridSize = 15;
    Vector2 mousePos = GetMousePosition();
    Vector2 startPos = {(w - tileTex.width) / 2.f, h / 2.f};

    // DrawTexture(tileTex, startPos.x, startPos.y, WHITE);

    for (int rowIndex = 0; rowIndex < gridSize; rowIndex++)
    {
        for (int colIndex = 0; colIndex < gridSize; colIndex++)
        {
            int i = (gridSize * rowIndex) + colIndex;
            Vector2 isoCoords = transform({colIndex * tileTex.width, rowIndex * tileTex.height});

            auto getAlt = [&](float speed, float maxAlt)
            {
                // return sinf(rowIndex + GetTime() * speed) * maxAlt; // along row
                // return sinf(colIndex + GetTime() * speed) * maxAlt; // along col

                return sinf(rowIndex + GetTime() * speed) * sinf(colIndex + GetTime() * speed) * maxAlt; // along both
            };

            drawTile(colIndex, rowIndex, startPos, gridSize, getAlt(3, 30));
        }
    }

    DrawCircleV({startPos.x, startPos.y}, 5, RED);

    EndDrawing();
};

unsigned int prepareAssets(char *fileLoc)
{
    tileImg = LoadImage(fileLoc);
    ImageResizeNN(&tileImg, tileImg.width * 2, tileImg.height * 2);
    tileTex = LoadTextureFromImage(tileImg);
    cout << "Loaded Texture with width: " << tileTex.width << " and height: " << tileTex.height << "\n";
    return tileTex.id;
}