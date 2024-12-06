#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <float.h>
#include "stb_easy_font.h"
#include "menu.h"
#include "textur.h"

using namespace std;

#define TILE_SIZE 31.5f

GLuint texture;
GLuint sprite;
GLuint background;

int width = 1080;
int height = 720;
float gravity = 0.2;
float speed = 5.5;
float currentframe = 2;
bool directional;
bool gameState = 0;
float chSiX;
float chSiY;
int n = 0;
int m = 0;
float H = 22;
float W = 34;
float maxYVelocity = 47.0f;
float groundLevel = 0.0f;

string TileMap[34] = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "B                                B",
    "B                    BBBBBBB     B",
    "B                                B",
    "B     BBBBBBBBBBBB               B",
    "B                                B",
    "B                                B",
    "B               BBBBBBBBBBBB     B",
    "B                                B",
    "B                                B",
    "B  BBBBBBBBBBBBBB                B",
    "B                                B",
    "B                                B",
    "B                BBBBBBBBBBBBB   B",
    "B                                B",
    "B                                B",
    "B                                B",
    "B                                B",
    "B                                B",
    "B                                B",
    "B                                B",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
};

typedef struct {
    float left, right, top, bottom, textX, textY;
} sritCoord;

typedef struct {
    float x, y, dx, dy;
    const float HERO_HEIGHT = 60.0f;
    const float HERO_WIDTH = 40.5f;
    bool isAirborne;
    bool isMoving;
    bool facingLeft;
} Hero;

Hero pers;
sritCoord vrtcoord;
sritCoord BackGroundCoord;

void Hero_Init(Hero *obj, float x1, float y1, float dx1, float dy1) {
    obj->x = x1;
    obj->y = y1;
    obj->dx = dx1;
    obj->dy = dy1;
    obj->isAirborne = true;
    obj->isMoving = false;
    obj->facingLeft = false;
}

void Reflect(float *da, float *a, bool cond, float wall) {
    if (!cond) {
        return;
    }
    *da = 0;
    *a = wall;
}

void DrawCollisionBox(Hero *hero) {
    glColor3f(1.0f, 0.0f, 0.0f); // Установка красного цвета для прямоугольника коллизии
    glDisable(GL_TEXTURE_2D); // Отключение текстурирования для рисования цветных форм

    glBegin(GL_LINE_LOOP); // Начинаем рисовать линии
        glVertex2f(hero->x, hero->y);
        glVertex2f(hero->x + hero->HERO_WIDTH, hero->y);
        glVertex2f(hero->x + hero->HERO_WIDTH, hero->y + hero->HERO_HEIGHT);
        glVertex2f(hero->x, hero->y + hero->HERO_HEIGHT);
    glEnd();

    glEnable(GL_TEXTURE_2D); // Включение текстурирования обратно для остальной части сцены
    glColor3f(1.0f, 1.0f, 1.0f); // Возвращение цвета к белому для последующей отрисовки текстур
}


bool isSolidTileAt(float x, float y) {
    int tileX = (int)(x / TILE_SIZE);
    int tileY = (int)(y / TILE_SIZE);

    if (tileX < 0 || tileX >= W || tileY < 0 || tileY >= H) {
        return false;
    }

    char tile = TileMap[tileY][tileX];
    return tile == 'B';
}

void UpdateGroundLevelForHero(Hero* hero) {
    float nearestGround = FLT_MAX;
    bool groundFound = false;

    float feetY = hero->y + hero->HERO_HEIGHT;

    int tileXStart = (int)(hero->x / TILE_SIZE);
    int tileXEnd = (int)((hero->x + hero->HERO_WIDTH) / TILE_SIZE);

    for (int x = tileXStart; x <= tileXEnd; x++) {
        for (int y = (int)(feetY / TILE_SIZE); y < H; y++) {
            if (isSolidTileAt(x * TILE_SIZE, y * TILE_SIZE)) {
                float groundY = y * TILE_SIZE - hero->HERO_HEIGHT;
                if (groundY < nearestGround) {
                    nearestGround = groundY;
                    groundFound = true;
                }
                break;
            }
        }
    }

    if (groundFound) {
        groundLevel = nearestGround;
    } else {
        groundLevel = FLT_MAX;
    }
}

bool CheckHorizontalCollision(float newX, Hero *hero, bool* isWallHitX) {
    *isWallHitX = false;
    bool collision = false;

    int leftTile = floor(newX / TILE_SIZE);
    int rightTile = ceil((newX + hero->HERO_WIDTH) / TILE_SIZE) - 1;

    for (int y = floor(hero->y / TILE_SIZE); y <= ceil((hero->y + hero->HERO_HEIGHT) / TILE_SIZE) - 1; y++) {
        for (int x = leftTile; x <= rightTile; x++) {
            if (x < 0 || x >= W || y < 0 || y >= H) continue;
            if (isSolidTileAt(x * TILE_SIZE, y * TILE_SIZE)) {
                collision = true;
                if (x == leftTile || x == rightTile) {
                    *isWallHitX = true;
                    break;
                }
            }
        }
        if (*isWallHitX) break;
    }
    return collision;
}

bool CheckVerticalCollision(float newY, Hero *hero, bool* isWallHitY) {
    *isWallHitY = false;
    bool collision = false;

    int topTile = floor(newY / TILE_SIZE);
    int bottomTile = ceil((newY + hero->HERO_HEIGHT) / TILE_SIZE) - 1;

    for (int x = floor(hero->x / TILE_SIZE); x <= ceil((hero->x + hero->HERO_WIDTH) / TILE_SIZE) - 1; x++) {
        for (int y = topTile; y <= bottomTile; y++) {
            if (x < 0 || x >= W || y < 0 || y >= H) continue;
            if (isSolidTileAt(x * TILE_SIZE, y * TILE_SIZE)) {
                collision = true;
                if (y == topTile || y == bottomTile) {
                    *isWallHitY = true;
                    break;
                }
            }
        }
        if (*isWallHitY) break;
    }
    return collision;
}

void UpdateHeroPositionAndCollisions(Hero *hero, float deltaTime) {
    // Обновляем уровень земли для героя
    UpdateGroundLevelForHero(hero);

    // Предполагаемая новая позиция героя по оси X
    float potentialNewX = hero->x + hero->dx * deltaTime;
    bool isWallHitX = false;

    // Проверка столкновения по оси X
    if (!CheckHorizontalCollision(potentialNewX, hero, &isWallHitX)) {
        hero->x = potentialNewX;
    } else {
        // При столкновении справа или слева корректируем позицию
        if (hero->dx > 0) { // Движение вправо
            hero->x = floor((potentialNewX + hero->HERO_WIDTH) / TILE_SIZE) * TILE_SIZE - hero->HERO_WIDTH - 0.01;
        } else if (hero->dx < 0) { // Движение влево
            hero->x = ceil(potentialNewX / TILE_SIZE) * TILE_SIZE + 0.01;
        }
        hero->dx = 0;
    }

    // Применение гравитации и обновление по Y
    hero->dy -= gravity * deltaTime;
    float potentialNewY = hero->y + hero->dy * deltaTime;
    bool isWallHitY = false;

    // Проверка столкновения по оси Y
    if (!CheckVerticalCollision(potentialNewY, hero, &isWallHitY)) {
        hero->y = potentialNewY;
        hero->isAirborne = true;
    } else {
        // При столкновении сверху или снизу
        if (hero->dy > 0) { // Если движение вверх
            hero->y = ceil((hero->y + hero->HERO_HEIGHT) / TILE_SIZE) * TILE_SIZE - hero->HERO_HEIGHT - 0.01;
        } else { // Если движение вниз
            hero->y = floor(potentialNewY / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
            hero->isAirborne = false;
        }
        hero->dy = 0;
    }

    // Ограничение вертикальной скорости
    if (hero->dy > maxYVelocity) {
        hero->dy = maxYVelocity;
    } else if (hero->dy < -maxYVelocity) {
        hero->dy = -maxYVelocity;
    }
}

void Collision(Hero& hero) {
    const float tile_size = 31.5f;
    bool isWallHitX = false;
    bool isWallHitY = false;

    float left = hero.x;
    float right = hero.x + hero.HERO_WIDTH;
    float top = hero.y;
    float bottom = hero.y + hero.HERO_HEIGHT;

    int left_tile = (int)(floor(left / tile_size));
    int right_tile = (int)(ceil(right / tile_size)) - 1;
    int top_tile = (int)(floor(top / tile_size));
    int bottom_tile = (int)(ceil(bottom / tile_size)) - 1;

    // Горизонтальная коллизия
    if (hero.dx != 0) {
        float new_x = hero.x + hero.dx;

        if (hero.dx > 0) { // Движение вправо
            for (int y = top_tile; y <= bottom_tile; y++) {
                if (TileMap[y][right_tile] == 'B') {
                    isWallHitX = true;
                    hero.x = right_tile * tile_size - hero.HERO_WIDTH - 0.01;
                    break;
                }
            }
        } else { // Движение влево
            for (int y = top_tile; y <= bottom_tile; y++) {
                if (TileMap[y][left_tile] == 'B') {
                    isWallHitX = true;
                    hero.x = (left_tile + 1) * tile_size + 0.01;
                    break;
                }
            }
        }

        if (!isWallHitX) {
            hero.x += hero.dx;
        }
        hero.dx = 0;
    }

    // Вертикальная коллизия
    if (hero.dy != 0) {
        float new_y = hero.y + hero.dy;

        if (hero.dy > 0) { // Движение вверх
            for (int x = left_tile; x <= right_tile; x++) {
                if (TileMap[top_tile][x] == 'B') {
                    isWallHitY = true;
                    hero.y = top_tile * tile_size - hero.HERO_HEIGHT - 0.01;
                    break;
                }
            }
        } else { // Движение вниз
            for (int x = left_tile; x <= right_tile; x++) {
                if (TileMap[bottom_tile][x] == 'B') {
                    isWallHitY = true;
                    hero.y = (bottom_tile + 1) * tile_size + 0.01;
                    hero.isAirborne = false;
                    break;
                }
            }
        }

        if (!isWallHitY) {
            hero.y += hero.dy;
            hero.isAirborne = true;
        }
        hero.dy = 0;
    }

    // Применение гравитации
    hero.dy -= gravity;
}

void Hero_Move(Hero *obj) {
    if (GetKeyState(VK_LEFT) < 0 && gameState == 1) {
        currentframe += 0.15;
        obj->dx = -speed;
        if (currentframe > 8) {
            currentframe -= 7;
        }
        directional = 1;
    } else if (GetKeyState(VK_RIGHT) < 0 && gameState == 1) {
        currentframe += 0.15;
        obj->dx = speed;
        if (currentframe > 8) {
            currentframe -= 7;
        }
        directional = 0;
    } else {
        obj->dx = 0;
    }

    if (GetKeyState(VK_UP) < 0 && gameState == 1 && !obj->isAirborne) {
        obj->dy = speed * 1.2;
        obj->isAirborne = true;
    }

    UpdateHeroPositionAndCollisions(obj, 1.0f); // deltaTime можно заменить на реальное время между кадрами
}

void Sprite_animation(GLuint texture, int n, Hero *obj) {
    static float svertix[] = {0, 0, 80, 0, 80, 80, 0, 80};
    static float TexCord[] = {0, 0, 0.12, 0, 0.12, 1, 0, 1};

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.7);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, svertix);
    glTexCoordPointer(2, GL_FLOAT, 0, TexCord);

    static float spriteXsize = 800;
    static float spriteYsize = 100;
    static float charsizey = 34;
    static float charsizex = 100;
    vrtcoord.left = (charsizex * n) / spriteXsize;
    vrtcoord.right = vrtcoord.left + (charsizex / spriteXsize);
    vrtcoord.top = (charsizey * 0) / spriteYsize;
    vrtcoord.bottom = vrtcoord.top + (charsizey / spriteYsize);

    TexCord[1] = TexCord[3] = vrtcoord.bottom;
    TexCord[5] = TexCord[7] = vrtcoord.top;

    if (directional) {
        TexCord[2] = TexCord[4] = vrtcoord.left;
        TexCord[0] = TexCord[6] = vrtcoord.right;
    } else {
        TexCord[0] = TexCord[6] = vrtcoord.left;
        TexCord[2] = TexCord[4] = vrtcoord.right;
    }

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisable(GL_ALPHA_TEST);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    chSiX = charsizex;
    chSiY = charsizey;
}

void Main_Init() {
    Hero_Init(&pers, width / 2.0, 32, 0, 0);
    Load_Texture("img/Anim.png", &sprite, GL_CLAMP, GL_CLAMP, GL_NEAREST);
    Load_Texture("img/background.png", &background, GL_REPEAT, GL_CLAMP, GL_NEAREST);
}

void Menu_Init() {
    Menu_AddButton("Play", 10.0, 10.0, 100.0, 30.0, 2);
    Menu_AddButton("Menu", 10.0, 50.0, 100.0, 30.0, 2);
    Menu_AddButton("Quit", 10.0, 90.0, 100.0, 30.0, 2);
}

void Show_Background(GLuint texture) {
    static float svertix[] = {-1080, 0, -1080, 700, 1080 * 2, 700, 1080 * 2, 0};
    static float TexCord[] = {-1, 1, -1, 0, 2, 0, 2, 1};

    glClearColor(0, 0, 0, 0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, svertix);
    glTexCoordPointer(2, GL_FLOAT, 0, TexCord);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MouseDown() {
    int buttonId = Menu_MouseDown();
    if (buttonId < 0) return;
    char *name = Menu_GetButtonName(buttonId);
    printf("%s\n", name, &buttonId);
    switch (buttonId) {
        case 0:
            if (gameState == 0) {
                gameState = 1;
            }
            break;
        case 1:
            if (gameState == 1) {
                gameState = 0;
            }
            break;
        case 2:
            PostQuitMessage(0);
            break;
    }
}

void draw_map() {
    glColor3f(0.9f, 0.1f, 0.0f); // Коричневый цве
    const float tsize = 31.5f;
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < TileMap[i].length(); j++) {
            if (TileMap[i][j] == 'B') {
                float x1 = j * tsize + tsize;
                float y1 = i * tsize + tsize;
                float x2 = j * tsize + tsize;
                float y2 = i * tsize;
                float x3 = j * tsize;
                float y3 = i * tsize;
                float x4 = j * tsize;
                float y4 = i * tsize + tsize;

                float tile[] = {x1, y1, x2, y2, x3, y3, x4, y4};

                glVertexPointer(2, GL_FLOAT, 0, tile);
                glEnableClientState(GL_VERTEX_ARRAY);

                glDrawArrays(GL_QUADS, 0, 4);
                glDisableClientState(GL_VERTEX_ARRAY);
            }
        }
    }
}

void Hero_Show(Hero *obj) {
    draw_map();
    glColor3f(1,1,1);

    glPushMatrix();
    glTranslatef(obj->x, obj->y, 0);
    Sprite_animation(sprite, currentframe, &pers);
    glPopMatrix();

    DrawCollisionBox(obj); // Вызов функции рисования коллизионного бокса
}

