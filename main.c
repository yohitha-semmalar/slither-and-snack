#include "raylib.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
typedef enum {
    SCREEN_INTRO,
    SCREEN_MAIN_MENU,
    SCREEN_GOBBLE,
    SCREEN_CUSTOM_SNAKE,
    SCREEN_CUSTOM_BG,
    SCREEN_DIFFICULTY,
    SCREEN_HIGHSCORE
} Screen;

typedef enum{
    STATE_NONE,
    STATE_INTRO,
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAMEOVER
} GameState;

typedef enum {
    CREATURE_HARMFUL,
    CREATURE_NONE
} CreatureType;


#define WIN_WIDTH 1200
#define WIN_HEIGHT 700
#define SQUARE_SIZE 32
#define MAX_SNAKE_LENGTH 400
#define MAX_ENEMIES 5
#define HIGHSCORE_FILE "highscore.txt"

typedef struct Snake {
    Vector2 position[MAX_SNAKE_LENGTH]; // Snake body segments positions
    int size;                           // Current length of the snake
    Vector2 direction;                  // Direction of the snake's movement
} Snake;

typedef struct food{
    Vector2 position;
    bool active;
} food;

// Function Declarations
bool CheckCollision(Vector2 a, Vector2 b);
void InitSnake(Snake *snake);
void UpdateSnake(Snake *snake);
void DrawSnake(Snake snake);
void InitFood(food *food, Snake snake);
void DrawFood(food Food);

Texture2D normalFood;
Texture2D specialFood;
Texture2D harmfulCreature;
Screen currentScreen;

Texture2D currentTheme;
Texture2D ForestTheme;
Texture2D UnderwaterTheme;
Texture2D SpaceTheme;

Texture2D ForestFood;
Texture2D UnderwatrerFood;
Texture2D spaceFood;
Texture2D currentFood;

Texture2D scalySkin;
Texture2D slimySkin;
Texture2D lavaSkin;
Texture2D currentSkin;

Texture2D ForestEnemy;
Texture2D UnderWaterEnemy;
Texture2D SpaceEnemy;
Texture2D currentEnemy;

CreatureType currentCreature;
Music BGMusic;
GameState currentState;
int score;

float waveFrequency = 0.2f;
float waveAmplitude = 5.0f;

int main(void) {
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Slither & Snack");
    InitAudioDevice();
    SetTargetFPS(60);

    FILE *f = fopen(HIGHSCORE_FILE, "r");
    int highScore = 0;
    if (f != NULL){
        fscanf(f, "%d", &highScore);
    }
    fclose(f);

    ForestTheme = LoadTexture("startBG.png");
    UnderwaterTheme = LoadTexture("uwBG3.jpg");
    SpaceTheme = LoadTexture("space (2).png");
    currentTheme = ForestTheme;

    ForestFood = LoadTexture("rat.png");
    UnderwatrerFood = LoadTexture("clown-fish.png");
    spaceFood = LoadTexture("star.png");
    currentFood = ForestFood;

    ForestEnemy = LoadTexture("fox.png");
    UnderWaterEnemy = LoadTexture("shark (1).png");
    SpaceEnemy = LoadTexture("angry.png");
    currentEnemy = ForestEnemy;

    float centerX = WIN_WIDTH/2;
    float centerY = WIN_HEIGHT/3;

    float buttonwidth = 300;
    float buttonheight = 60;

    Texture2D black_circle = LoadTexture("dot (1).png");
    Texture2D gobblesnake = LoadTexture("snake (4).png");
    Texture2D laughingSnake = LoadTexture("C:/Users/skysm/Downloads/snake (9).png");
    Font myfont = LoadFont("Starbim.ttf");

    Music introMusic = LoadMusicStream("magic-forest-318165.mp3");
    PlayMusicStream(introMusic);

    Sound eatingSound = LoadSound("E:/myproject/Raylib-CPP-Starter-Template-for-VSCODE-main/eating.mp3");
    Sound collisionSound = LoadSound("C:/Users/skysm/Downloads/bellding-254774.mp3");

    Rectangle BackButton = {20,20, 150, 50};
    Rectangle StartButton = {centerX - (buttonwidth/2), centerY - 110, buttonwidth, buttonheight};
    Rectangle CustSnakeBody = {centerX - (buttonwidth/2), centerY - 20, buttonwidth, buttonheight};
    Rectangle CustBG = {centerX - (buttonwidth/2), centerY + 70, buttonwidth, buttonheight};
   // Rectangle difficulty = {centerX - (buttonwidth/2), centerY + 150, buttonwidth, buttonheight};
    Rectangle HighScore = {centerX - (buttonwidth/2), centerY + 150, buttonwidth, buttonheight};
    Rectangle RestartButton = {centerX - (buttonwidth/2), centerY + 150, buttonwidth, buttonheight};

    Texture2D logo = LoadTexture("snake (7).png"); 
    Texture2D HSbg = LoadTexture("HSbg.jpg");

    // Skin textures
    scalySkin = LoadTexture("image (3).png");
    slimySkin = LoadTexture("image (2).png");
    lavaSkin = LoadTexture("image (1).png");
    currentSkin = lavaSkin;

    float timer = 0.0f;
    float waittime = 7.0f;
    const char *text = "Slither & Snack";
    int textlen = strlen(text);
    int currentletter = 0;

    currentState = STATE_INTRO;
    currentScreen = SCREEN_INTRO;    

    Snake snake;
    food Food;
    Vector2 enemy[MAX_ENEMIES];
    Sound enemyCollisionSound = LoadSound("enemyCollisionSound.mp3");

    for (int i = 0; i < MAX_ENEMIES; i++){
        enemy[i].x = (float)GetRandomValue(0, WIN_WIDTH / SQUARE_SIZE - 1);
        enemy[i].y = (float)GetRandomValue(0, WIN_HEIGHT / SQUARE_SIZE - 1);
    }
    
    bool gameOver = false;
    score = 0;

    while (!WindowShouldClose()) {
        UpdateMusicStream(introMusic); 
        timer += GetFrameTime();

        if (currentletter < textlen && timer >= 0.2f * currentletter) {
            currentletter++;
        }


        Vector2 mousePoint = GetMousePosition();
        bool startButtonPressed = CheckCollisionPointRec(mousePoint, StartButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool CustBodyPressed = CheckCollisionPointRec(mousePoint, CustSnakeBody) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool CustBGPressed = CheckCollisionPointRec(mousePoint, CustBG) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
       // bool DiffPressed = CheckCollisionPointRec(mousePoint, difficulty) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool HighScorePressed = CheckCollisionPointRec(mousePoint, HighScore) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool BackButtonPressed = CheckCollisionPointRec(mousePoint, BackButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(currentTheme, 0, 0, WHITE);
       /* if (currentState == STATE_MENU || currentState == STATE_PLAYING || currentState == STATE_GAMEOVER)
       { */
        switch (currentState) {
            case STATE_MENU:
                currentScreen = SCREEN_MAIN_MENU;
                break;
            case STATE_PLAYING:
                //DrawTexture(currentTheme, 0, 0, WHITE);
                DrawText(TextFormat("SCORE : %d", score), 1000, 20, 30, WHITE);
                if (!gameOver) {   
                    if (IsKeyPressed(KEY_UP) && snake.direction.y == 0)  snake.direction = (Vector2){ 0, -1 };
                    if (IsKeyPressed(KEY_DOWN) && snake.direction.y == 0) snake.direction = (Vector2){ 0,  1 };
                    if (IsKeyPressed(KEY_LEFT) && snake.direction.x == 0) snake.direction = (Vector2){ -1, 0 };
                    if (IsKeyPressed(KEY_RIGHT) && snake.direction.x == 0) snake.direction = (Vector2){ 1, 0 };

                    for (int i = 0; i < MAX_ENEMIES; i++){
                        DrawTexture(currentEnemy, enemy[i].x * SQUARE_SIZE, enemy[i].y * SQUARE_SIZE, WHITE);
                    }

                    UpdateSnake(&snake);
                    if (CheckCollision(snake.position[0], Food.position)) {
                        score++;
                        PlaySound(eatingSound);
                        snake.size++;
                        InitFood(&Food, snake);
                    }

                    if (score > highScore){
                        highScore = score;
                        FILE *f = fopen(HIGHSCORE_FILE, "w");
                        fprintf(f, "%d", highScore);
                        fclose(f);
                    }

                    for (int i = 0; i < MAX_ENEMIES; i++){
                        if (snake.position[0].x == enemy[i].x && snake.position[0].y == enemy[i].y){
                            PlaySound(enemyCollisionSound);
                            gameOver = true;
                            break;
                        }
                    }
        
                    // Check wall collision
                    if (snake.position[0].x < 0 || snake.position[0].x >= WIN_WIDTH / SQUARE_SIZE ||
                        snake.position[0].y < 0 || snake.position[0].y >= WIN_HEIGHT / SQUARE_SIZE)
                    {   
                        PlaySound(collisionSound);
                        gameOver = true;
                    }
                    for (int i = 1; i < snake.size; i++)
            {
                if (CheckCollision(snake.position[0], snake.position[i]))
                {   
                    PlaySound(collisionSound);
                    gameOver = true;
                    break;
                }
            }
        }
        if (!gameOver)
        {
            DrawSnake(snake);
            DrawFood(Food);
        }
        else
        {   
            currentState = STATE_GAMEOVER;
        }

                break;
            case STATE_GAMEOVER:
                // DrawTexture(currentTheme, 0, 0, WHITE);
                DrawText(TextFormat("YOUR SCORE : %d", score), 470, WIN_HEIGHT/2 + 150, 30, WHITE);
                DrawText(TextFormat("HIGH SCORE : %d", highScore), 470, WIN_HEIGHT/2 + 200, 30, WHITE);
                DrawTexture(black_circle, centerX - 128, centerY - 128, WHITE);
                DrawTexture(laughingSnake, centerX - 128, centerY - 128, WHITE);
                DrawText("Ha Ha Ha...!!! You died",WIN_WIDTH/2 - 200, WIN_HEIGHT/2 + 100, 40, WHITE);
                DrawRectangleRounded(RestartButton, 0.8f, 15, DARKGREEN);
                DrawText("RESTART", RestartButton.x + 55, RestartButton.y + 14, 30, WHITE);
                bool RestartButtonPressed = CheckCollisionPointRec(mousePoint, RestartButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
                if (RestartButtonPressed){
                    score = 0;
                    currentState = STATE_PLAYING;
                    InitSnake(&snake);
                    InitFood(&Food, snake);
                    gameOver = false;
                }
                break;
        }
    //}
        if (currentScreen == SCREEN_INTRO) {
            if (timer < waittime) {
                //DrawTexture(currentTheme, 0, 0, WHITE);
                DrawTexture(black_circle, centerX - 128, centerY - 128, WHITE);
                DrawTexture(logo, centerX - 128, centerY - 128, WHITE);
                DrawTextEx(myfont, TextSubtext(text, 0, currentletter), (Vector2){WIN_WIDTH / 3 - 130, WIN_HEIGHT / 2 + 5}, 75, 2, BLACK);
            } else {
                SetTargetFPS(10);
                currentScreen = SCREEN_MAIN_MENU;
                currentState = STATE_NONE;
            }
        }

      

        else if (currentScreen == SCREEN_MAIN_MENU) {
           // DrawTexture(currentTheme, 0, 0, WHITE);
            DrawTexture(gobblesnake, 20, 500, WHITE);

            DrawRectangleRounded(StartButton, 0.6f, 15, DARKGREEN);
            DrawText("GOBBLE", StartButton.x + 75, StartButton.y + 14, 30, WHITE);

            DrawRectangleRounded(CustSnakeBody, 0.6f, 15, DARKGREEN);
            DrawText("CUSTOM SNAKE", CustSnakeBody.x + 30, CustSnakeBody.y + 14, 30, WHITE);

            DrawRectangleRounded(CustBG, 0.6f, 15, DARKGREEN);
            DrawText("CUSTOM BG", CustBG.x + 65, CustBG.y + 14, 30, WHITE);

           // DrawRectangleRounded(difficulty, 0.6f, 15, DARKGREEN);
        //    DrawText("DIFFICULTY", difficulty.x + 55, difficulty.y + 14, 30, WHITE);

            DrawRectangleRounded(HighScore, 0.6f, 15, DARKGREEN);
            DrawText("HIGH SCORE", HighScore.x + 55, HighScore.y + 14, 30, WHITE);

            if (startButtonPressed) {
                currentScreen = SCREEN_GOBBLE;
                currentState = STATE_PLAYING;
                InitSnake(&snake);
                InitFood(&Food, snake);
            }
            if (CustBodyPressed) currentScreen = SCREEN_CUSTOM_SNAKE;
            if (CustBGPressed) currentScreen = SCREEN_CUSTOM_BG;
            //if (DiffPressed) currentScreen = SCREEN_DIFFICULTY;
            if (HighScorePressed) currentScreen = SCREEN_HIGHSCORE;
        }

        else if (currentScreen == SCREEN_GOBBLE) {
            //DrawTexture(currentTheme, 0, 0, WHITE);
            DrawRectangleRounded(BackButton, 1.0f, 15, DARKGREEN);
            DrawText("BACK", BackButton.x + 30, BackButton.y + 10, 30, WHITE);

            if (BackButtonPressed) {
                currentScreen = SCREEN_MAIN_MENU;
                currentState = STATE_NONE;
            }
        }

        else if (currentScreen == SCREEN_CUSTOM_SNAKE) {
            ClearBackground(RAYWHITE);
            DrawRectangleRounded(BackButton, 1.0f, 15, DARKGREEN);
            DrawText("BACK", BackButton.x + 30, BackButton.y + 10, 30, WHITE);
            Rectangle lavaBtn = {centerX - 160, centerY - 60, 300, 50};
            Rectangle slimyBtn = {centerX - 160, centerY + 10, 300, 50};
            Rectangle scalyBtn = {centerX - 160, centerY + 80, 300, 50};

            DrawRectangleRounded(lavaBtn, 0.6, 10, RED);
            DrawText("HOT LAVA", lavaBtn.x + 70, lavaBtn.y + 10, 25, WHITE);
            DrawRectangleRounded(slimyBtn, 0.6f, 10, GREEN);
            DrawText("SLIMY", slimyBtn.x + 110, slimyBtn.y + 10, 25, WHITE);
            DrawRectangleRounded(scalyBtn, 0.6f, 10, DARKGREEN);
            DrawText("SCALY", scalyBtn.x + 100, scalyBtn.y + 10, 25, WHITE);

            if (CheckCollisionPointRec(mousePoint, lavaBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                currentSkin = lavaSkin;
            if (CheckCollisionPointRec(mousePoint, slimyBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                currentSkin = slimySkin;
            if (CheckCollisionPointRec(mousePoint, scalyBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                currentSkin = scalySkin;

            if (BackButtonPressed) {currentScreen = SCREEN_MAIN_MENU;}
        }

        else if (currentScreen == SCREEN_CUSTOM_BG) {
            ClearBackground(RAYWHITE);
            DrawTexture(currentTheme, 0, 0, WHITE);
            DrawRectangleRounded(BackButton, 1.0f, 15, DARKGREEN);
            DrawText("BACK", BackButton.x + 30, BackButton.y + 10, 30, WHITE);
            Rectangle forestBtn = {centerX - 160, centerY - 60, 300, 50};
            Rectangle underwaterBtn = {centerX - 160, centerY + 10, 300, 50};
            Rectangle spaceBtn = {centerX - 160, centerY + 80, 300, 50};

            DrawRectangleRounded(forestBtn, 0.6f, 10, GREEN);
            DrawText("Forest", forestBtn.x + 100, forestBtn.y + 10, 25, WHITE);
            DrawRectangleRounded(underwaterBtn, 0.6f, 10, BLUE);
            DrawText("Underwater", underwaterBtn.x + 70, underwaterBtn.y + 10, 25, WHITE);
            DrawRectangleRounded(spaceBtn, 0.6f, 10,PURPLE);
            DrawText("Space", spaceBtn.x + 100, spaceBtn.y + 10, 25, WHITE);

            if (CheckCollisionPointRec(mousePoint, forestBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
               { currentTheme = ForestTheme;
                 currentFood = ForestFood;
                 currentEnemy = ForestEnemy;}
            if (CheckCollisionPointRec(mousePoint, underwaterBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {currentTheme = UnderwaterTheme;
                 currentFood = UnderwatrerFood;
                 currentEnemy = UnderWaterEnemy;}
            if (CheckCollisionPointRec(mousePoint, spaceBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {currentTheme = SpaceTheme;
                 currentFood = spaceFood;
                currentEnemy = SpaceEnemy;}

            if (BackButtonPressed) currentScreen = SCREEN_MAIN_MENU;
        }

       /* else if (currentScreen == SCREEN_DIFFICULTY) {
            ClearBackground(RAYWHITE);
            DrawRectangleRounded(BackButton, 1.0f, 15, DARKGREEN);
            DrawText("BACK", BackButton.x + 30, BackButton.y + 10, 30, WHITE);
            DrawText("Choose Difficulty Screen!", centerX - 200, centerY, 30, BLACK);
            if (BackButtonPressed) currentScreen = SCREEN_MAIN_MENU;
        }*/

        else if (currentScreen == SCREEN_HIGHSCORE) {
            ClearBackground(RAYWHITE);
            DrawTexture(HSbg, 0, 0, WHITE);
            DrawRectangleRounded(BackButton, 1.0f, 15, DARKGREEN);
            DrawText("BACK", BackButton.x + 30, BackButton.y + 10, 30, WHITE);
            DrawText(TextFormat("HIGH SCORE : %d", highScore), centerX - 160, centerY + 10, 50, DARKGREEN);

            if (BackButtonPressed) currentScreen = SCREEN_MAIN_MENU;
        }

        EndDrawing();
    }

    // Unload stuff after closing
    UnloadTexture(currentTheme);
    UnloadTexture(ForestTheme);
    UnloadTexture(UnderwaterTheme);
    UnloadTexture(SpaceTheme);
    UnloadTexture(logo);
    UnloadTexture(black_circle);
    UnloadFont(myfont);
    UnloadMusicStream(introMusic);
    UnloadSound(eatingSound);
    UnloadSound(collisionSound);
    UnloadTexture(scalySkin);
    UnloadTexture(lavaSkin);
    UnloadTexture(slimySkin);
    UnloadSound(enemyCollisionSound);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

// --- Other functions unchanged ---

void InitSnake(Snake *snake) {
    snake->size = 1;
    snake->position[0] = (Vector2){ WIN_WIDTH / (2 * SQUARE_SIZE), WIN_HEIGHT / (2 * SQUARE_SIZE) };
    snake->direction = (Vector2){ 0, 0 };
}

bool CheckCollision(Vector2 a, Vector2 b) {
    return (a.x == b.x && a.y == b.y);
}

void InitFood(food *Food, Snake snake) {
    Food->active = true;
    bool validPosition = false;

    while (!validPosition) {
        validPosition = true;
        Food->position.x = (float)GetRandomValue(0, WIN_WIDTH / SQUARE_SIZE - 1);
        Food->position.y = (float)GetRandomValue(0, WIN_HEIGHT / SQUARE_SIZE - 1);

        for (int i = 0; i < snake.size; i++) {
            if (CheckCollision(Food->position, snake.position[i])) {
                validPosition = false;
                break;
            }
        }
    }
}

void UpdateSnake(Snake *snake) {
    for (int i = snake->size - 1; i > 0; i--) {
        snake->position[i] = snake->position[i - 1];
    }
    snake->position[0].x += snake->direction.x;
    snake->position[0].y += snake->direction.y;
}

void DrawSnake(Snake snake) {
    for (int i = 0; i < snake.size; i++) {
        Vector2 pos = snake.position[i];

        float offset = sinf(GetTime() * 5.0f + i * 0.5f) * 5.0f;

        Vector2 perp = { -snake.direction.y, snake.direction.x };

        Vector2 drawPos = {
            pos.x * SQUARE_SIZE + perp.x * offset,
            pos.y * SQUARE_SIZE + perp.y * offset
        };

        DrawTexture(currentSkin, drawPos.x, drawPos.y, WHITE);
    }
}


void DrawFood(food Food)
{
    DrawTexture(currentFood , Food.position.x * SQUARE_SIZE, Food.position.y * SQUARE_SIZE, WHITE);
}
