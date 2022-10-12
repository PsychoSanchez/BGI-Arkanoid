#include "./Lib/graphics.h"
#include "time.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_THREADS 3
using namespace std;

struct Vector2 {
  int x, y;
};

struct LongVector2 {
  double x, y;
};

struct BoundingRect {
  double left, right, top, bottom, width, height;
};

struct Sphere {
  LongVector2 speed;
  LongVector2 position;
  double radius;

  BoundingRect getBoundingRect() {
    return {this->position.x - this->radius,
            this->position.x + this->radius * 2,
            this->position.y - this->radius,
            this->position.y + this->radius * 2,
            this->radius * 2,
            this->radius * 2};
  }
};

struct StaticBoxObject {
  Vector2 position;
  Vector2 size;
  BoundingRect getBoundingRect() {
    return {(double)this->position.x, (double)this->position.x + this->size.x,
            (double)this->position.y, (double)this->position.y + this->size.y,
            (double)this->size.x,     (double)this->size.y};
  }
};

enum BrickType {
  Empty = 0,
  SingleHit = 1,
  DoubleHit = 2,
  Unbreakable = 3,
  // Bonuses
  MoreSpheresBonus = 4,
  ExtraLifeBonus = 5,
  PadSpeedBonus = 6,
};

struct GameBrick {
  StaticBoxObject boundaries;
  BrickType type;

  // Score value
  int value;
};

struct GameScene {
  StaticBoxObject boundaries;
  Vector2 brickGridSize;
  GameBrick *bricks;
  int sphereCount;
  Sphere *spheres;

  int score;
  int lifes;
  double timer;
};

enum GameState {
  MainMenu,
  LevelSelect,
  Pause,
  Game,
};

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const char *FIRST_LEVEL_PATH = "./resources/level1";

const Vector2 BRICK_SIZE = {40, 15};
const int HORIZONTAL_PADDING = 10;
const int VERTICAL_PADDING = 5;
const int TOP_LEFT_PADDING = 10;

GameScene createEmptyScene() {
  Vector2 rootPosition = {0, 0};
  Vector2 size = {WINDOW_WIDTH, WINDOW_HEIGHT};
  StaticBoxObject boundaries = {rootPosition, size};
  GameScene scene = {boundaries};

  return scene;
}

Sphere loadSphere(FILE *level) {
  Sphere sphere;

  fscanf_s(level, "%lf", &sphere.position.x);
  fscanf_s(level, "%lf", &sphere.position.y);
  fscanf_s(level, "%lf", &sphere.radius);
  fscanf_s(level, "%lf", &sphere.speed.x);
  fscanf_s(level, "%lf", &sphere.speed.y);

  return sphere;
}

GameBrick *getBrick(GameScene *scene, int row, int column) {
  auto offset = column * scene->brickGridSize.x;

  return &scene->bricks[offset + row];
}

void loadSpheres(GameScene *&scene, FILE *&level) {
  fscanf_s(level, "%d", &scene->sphereCount);
  scene->spheres = (Sphere *)malloc(scene->sphereCount * sizeof(Sphere));

  for (int i = 0; i < scene->sphereCount; i++) {
    auto sphere = loadSphere(level);

    scene->spheres[i] = sphere;
  }
}

void loadBricks(GameScene *&scene, FILE *&level) {
  fscanf_s(level, "%d", &scene->brickGridSize.y);
  fscanf_s(level, "%d", &scene->brickGridSize.x);

  scene->bricks = (GameBrick *)malloc(
      (scene->brickGridSize.x * scene->brickGridSize.y + 10) *
      sizeof(GameBrick));

  for (int column = 0; column < scene->brickGridSize.y; column++) {
    for (int row = 0; row < scene->brickGridSize.x; row++) {
      auto offset = column * scene->brickGridSize.x;

      GameBrick brick;

      brick.boundaries.size = BRICK_SIZE;
      brick.boundaries.position = {
          TOP_LEFT_PADDING + HORIZONTAL_PADDING * row + BRICK_SIZE.x * row,
          TOP_LEFT_PADDING + VERTICAL_PADDING * column + BRICK_SIZE.y * column,
      };

      int brickType;
      fscanf_s(level, "%d", &brickType);
      brick.type = (BrickType)brickType;

      scene->bricks[offset + row] = brick;
    }
  }
}

int loadLevel(const char *name, GameScene *scene) {
  FILE *level = fopen(name, "rt");

  if (!level) {
    outtextxy(320, 220, "Failed to load Level file");
    delay(1000);
    return 0;
  }

  loadSpheres(scene, level);

  fscanf_s(level, "%lf", &scene->timer);
  fscanf_s(level, "%d", &scene->score);
  fscanf_s(level, "%d", &scene->lifes);

  int notInterested;
  fscanf_s(level, "%lf", &notInterested);
  fscanf_s(level, "%lf", &notInterested);

  loadBricks(scene, level);

  fclose(level);
  return 1;
}

const int INNER_RECTANGLE_SIZE_DIFF = 2;

void drawBrick(GameBrick brick) {
  auto boundingRect = brick.boundaries.getBoundingRect();

  switch (brick.type) {
  case Empty:
    setcolor(BLACK);
    rectangle(boundingRect.left, boundingRect.top, boundingRect.right,
              boundingRect.bottom);
    // floodfill(midx, maxy - 13, WHITE);
    return;
  case SingleHit:
  case DoubleHit:
    if (brick.type == DoubleHit) {
      setcolor(LIGHTGRAY);
      rectangle(boundingRect.left + INNER_RECTANGLE_SIZE_DIFF,
                boundingRect.top + INNER_RECTANGLE_SIZE_DIFF,
                boundingRect.right - INNER_RECTANGLE_SIZE_DIFF,
                boundingRect.bottom - INNER_RECTANGLE_SIZE_DIFF);
    }

    setcolor(WHITE);
    break;
  case Unbreakable:
    setcolor(LIGHTRED);
    break;
  case MoreSpheresBonus:
    setcolor(LIGHTCYAN);
    break;
  case ExtraLifeBonus:
    setcolor(LIGHTGREEN);
    break;
  case PadSpeedBonus:
    break;
  }

  rectangle(boundingRect.left, boundingRect.top, boundingRect.right,
            boundingRect.bottom);
}

void drawBrickGrid(GameBrick *bricks, Vector2 gridSize) {
  auto length = gridSize.x * gridSize.y;

  for (int i = 0; i < length; i++) {
    drawBrick(bricks[i]);
  }
}

void drawSphere(Sphere *sphere, colors color) {
  setcolor(color);
  circle(sphere->position.x, sphere->position.y, sphere->radius);
}

void drawSpherePosition(Sphere *sphere) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "X: %g, Y: %g", sphere->position.x,
           sphere->position.y);
  outtextxy(500, 450, buffer);
}

void checkSphereCollisionWithBorders(Sphere *sphere) {
  if (sphere->position.x + sphere->radius * 2 > WINDOW_WIDTH) {
    sphere->speed.x = -1 * abs(sphere->speed.x);
  } else if (sphere->position.x - sphere->radius < 0) {
    sphere->speed.x = abs(sphere->speed.x);
  }

  if (sphere->position.y + sphere->radius * 2 > WINDOW_HEIGHT) {
    sphere->speed.y = -1 * abs(sphere->speed.y);
  } else if (sphere->position.y - sphere->radius < 0) {
    sphere->speed.y = abs(sphere->speed.y);
  }
}

enum CollisionDirection { Top, Bottom, Left, Right, None };

CollisionDirection getHorizontalCollisionDirection(BoundingRect a,
                                                   BoundingRect b) {
  if (a.left <= b.right && a.left >= b.left) {
    return Left;
  }
  if (a.right <= b.left && a.right >= b.right) {
    return Right;
  }

  return None;
}

CollisionDirection getVerticalCollisionDirection(BoundingRect a,
                                                 BoundingRect b) {
  if (a.top <= b.bottom && a.top >= b.top) {
    return Top;
  }
  if (a.bottom <= b.top && a.bottom >= b.bottom) {
    return Bottom;
  }

  return None;
}

bool isColliding(BoundingRect a, BoundingRect b) {
  auto vertical = getVerticalCollisionDirection(a, b);
  auto horizontal = getHorizontalCollisionDirection(a, b);

  return vertical != None && horizontal != None;
}

void applyCollision(Sphere *&sphere, CollisionDirection &collidingFrom) {
  int isCollidingFromTop = collidingFrom == Top;
  int isCollidingFromBottom = collidingFrom == Bottom;
  int isCollidingFromLeft = collidingFrom == Left;
  int isCollidingFromRight = collidingFrom == Right;

  sphere->speed.x =
      isCollidingFromRight * -1 * abs(sphere->speed.x) +
      isCollidingFromLeft * abs(sphere->speed.x) +
      (int)(!isCollidingFromRight && !isCollidingFromLeft) * sphere->speed.x;

  sphere->speed.y =
      isCollidingFromBottom * -1 * abs(sphere->speed.y) +
      isCollidingFromTop * abs(sphere->speed.y) +
      (int)(!isCollidingFromBottom && !isCollidingFromTop) * sphere->speed.y;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {

  initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Arkanoid v0.51");
  setusercharsize(30, 30, 30, 30);
  cleardevice();
  settextstyle(0, 0, 1);

  GameScene scene = createEmptyScene();

  if (!loadLevel(FIRST_LEVEL_PATH, &scene)) {
    return 0;
  }

  scene.spheres[0].speed.x = 5;
  scene.spheres[0].speed.y = 5;

  drawBrickGrid(scene.bricks, scene.brickGridSize);
  while (true) {
    auto sphere = &scene.spheres[0];
    drawSphere(sphere, BLACK);

    // Move spheres
    sphere->position.x += sphere->speed.x;
    sphere->position.y += sphere->speed.y;

    // Check bricks collision with spheres
    for (int row = 0; row < scene.brickGridSize.x; row++) {
      for (int column = 0; column < scene.brickGridSize.y; column++) {
        GameBrick *brick = getBrick(&scene, row, column);

        if (brick->type == Empty) {
          continue;
        }

        for (int sphereIndex = 0; sphereIndex < scene.sphereCount;
             sphereIndex++) {
          Sphere *sphere = &scene.spheres[sphereIndex];
          BoundingRect sphereBoundingRect = sphere->getBoundingRect();
          BoundingRect brickBoundingRect = brick->boundaries.getBoundingRect();

          CollisionDirection verticalCollisionWithBrick =
              getVerticalCollisionDirection(sphereBoundingRect,
                                            brickBoundingRect);
          CollisionDirection horizontalCollisionWithBrick =
              getHorizontalCollisionDirection(sphereBoundingRect,
                                              brickBoundingRect);
          bool bIsColliding = verticalCollisionWithBrick != None &&
                              horizontalCollisionWithBrick != None;

          if (bIsColliding) {
            int vertPenetration =
                verticalCollisionWithBrick == Top
                    ? brickBoundingRect.bottom - sphereBoundingRect.top
                    : sphereBoundingRect.bottom - brickBoundingRect.top;
            int horizontalPenetration =
                horizontalCollisionWithBrick == Left
                    ? brickBoundingRect.right - sphereBoundingRect.left
                    : sphereBoundingRect.right - brickBoundingRect.left;
            CollisionDirection primaryCollision =
                vertPenetration > horizontalPenetration
                    ? verticalCollisionWithBrick
                    : horizontalCollisionWithBrick;
            applyCollision(sphere, primaryCollision);
          }

          // Check collisions with each other
          // for (int restSphereIndex = sphereIndex + 1;
          //      restSphereIndex < scene.sphereCount; restSphereIndex++) {
          //   Sphere *secondSphere = &scene.spheres[restSphereIndex];
          //   BoundingRect secondSphereBoundingRect =
          //       secondSphere->getBoundingRect();

          //   CollisionDirection verticalCollisionWithSecondSphere =
          //       getVerticalCollisionDirection(sphereBoundingRect,
          //                                     secondSphereBoundingRect);
          //   CollisionDirection horizontalCollisionWithSecondSphere =
          //       getHorizontalCollisionDirection(sphereBoundingRect,
          //                                       secondSphereBoundingRect);
          //   bool bIsColliding = verticalCollisionWithSecondSphere != None &&
          //                       horizontalCollisionWithSecondSphere != None;

          //   if (bIsColliding) {
          //     applyCollision(sphere, verticalCollisionWithSecondSphere);
          //     applyCollision(sphere, horizontalCollisionWithSecondSphere);
          //   }
          // }
        }
      }
    }

    checkSphereCollisionWithBorders(sphere);

    setcolor(WHITE);
    drawSpherePosition(sphere);

    drawSphere(sphere, WHITE);

    Sleep(16);
  }
}

// bool DynamicCreateBalls(Scene *Sc) {
//   Sc->balss = (struct Ball *)malloc(Sc->num * sizeof(Ball));
//   return false;
// }

// bool Save(Scene *Sc) {
//   FILE *level = fopen("Save.txt", "wt");
//   if (!level) {
//     outtextxy(320, 220, "Failed to save Level file");
//     delay(1000);
//     return 0;
//   }
//   //�������� ���� � ��� ���������
//   fprintf(level, "%d\n", Sc->num);
//   for (int i = 0; i < Sc->num; i++) {
//     fprintf(level, "%lf ", Sc->Balls[i].x);
//     fprintf(level, "%lf ", Sc->Balls[i].y);
//     fprintf(level, "%lf ", Sc->Balls[i].radius);
//     fprintf(level, "%lf ", Sc->Balls[i].vx);
//     fprintf(level, "%lf ", Sc->Balls[i].vy);
//   }
//   fprintf(level, "\n");
//   fprintf(level, "%lf\n", Sc->timer);
//   fprintf(level, "%d\n", Sc->Score);
//   fprintf(level, "%d\n", Sc->lifes);
//   fprintf(level, "%lf\n", Sc->paddleX);
//   fprintf(level, "%lf\n", Sc->paddleXSpeed);
//   fprintf(level, "%d\n", Sc->row);
//   fprintf(level, "%d\n", Sc->column);
//   //�����
//   for (int i = 0; i < Sc->row; i++) {
//     for (int j = 0; j < Sc->column; j++)
//       fprintf(level, "%d ", Sc->AllBricks[i][j].type);
//     fprintf(level, "\n");
//   }

//   fclose(level);
// }
// bool LoadRecords(const char name[20]) {
//   FILE *Score;
//   int score;
//   Score = fopen(name, "rt");
//   if (!Score) {
//     outtextxy(220, 220, "Failed to load records Score");
//     return 0;
//   }

//   char Records[150];
//   int y = 160;
//   setcolor(WHITE);
//   while (!feof(Score)) {
//     fgets(Records, 150, Score);
//     // fscanf(Score, "%d", score);
//     outtextxy(400, y, Records);
//     y += 20;
//   }

//   fclose(Score);
//   return 1;
// }
// bool SaveRecords(Scene *Sc) {
//   FILE *Score;
//   Score = fopen("SaveRecords.txt", "rt");
//   if (!Score) {
//     outtextxy(220, 220, "Failed to save records");
//     return 0;
//   }
//   setcolor(WHITE);
//   while (!feof(Score)) {
//     fprintf(Score, "Enter name %d\n", Sc->Score);
//   }

//   fclose(Score);
//   return 1;
// }

// //���������
// void drawbrick(int i, int j, Scene *Sc) {
//   switch (Sc->AllBricks[i][j].type) {
//     //��� �����
//   case 0:
//     break;
//     //���� �������
//   case 1:
//     setcolor(WHITE);
//     rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
//               Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
//     rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
//               Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 -
//               2);
//     break;
//     //���� � ��������� ������
//   case 2:
//     setcolor(YELLOW);
//     rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
//               Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
//     rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
//               Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 -
//               2);
//     setcolor(WHITE);
//     break;
//     //���� � ��������� ��������
//   case 3:
//     setcolor(RED);
//     rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
//               Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
//     rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
//               Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 -
//               2);
//     setcolor(WHITE);
//     break;
//   case 4:
//     setcolor(LIGHTBLUE);
//     rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
//               Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
//     rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
//               Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 -
//               2);
//     setcolor(WHITE);
//     break;
//   case 9:
//     setcolor(LIGHTGRAY);
//     rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
//               Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
//     floodfill(Sc->AllBricks[i][j].lx + 10, Sc->AllBricks[i][j].ly + 7,
//               LIGHTGRAY);
//     setcolor(WHITE);
//   }
// }
// void bricks(Scene *Sc) {
//   int i, j;
//   for (i = 0; i < Sc->row; i++) // 5rows
//   {
//     for (j = 0; j < Sc->column; j++) // 10columns
//     {
//       drawbrick(i, j, Sc);
//       Sc->AllBricks[i][j].lx = 60 + (52 * j);
//       Sc->AllBricks[i][j].ly = 20 + (20 * i);
//     }
//   }
// }
// void blackbricks() {
//   int i, j, lx = 60, ly = 20;
//   for (i = 0; i < 5; i++) // 5rows
//   {
//     for (j = 0; j < 10; j++) // 10columns
//     {
//       setcolor(BLACK);
//       rectangle(lx, ly, lx + 41, ly + 15);
//       rectangle(lx + 2, ly + 2, lx + 41 - 2, ly + 15 - 2);
//       setcolor(WHITE);
//       lx = lx + 52;
//     }
//     lx = 60;
//     ly = ly + 20;
//   }
// }
// //��������� ����
// void DrawBall(Scene *Sc) {
//   for (int i = 0; i < Sc->num; i++) {
//     setcolor(BLACK);
//     circle(Sc->Balls[i].x, Sc->Balls[i].y, Sc->Balls[i].radius);
//     setcolor(WHITE);
//     Sc->Balls[i].x = Sc->Balls[i].x + Sc->Balls[i].vx;
//     Sc->Balls[i].y = Sc->Balls[i].y + Sc->Balls[i].vy;
//     circle(Sc->Balls[i].x, Sc->Balls[i].y, Sc->Balls[i].radius);
//   }
// }
// //�������� ������������
// void Window(Scene *Sc) {
//   for (int i = 0; i < Sc->num; i++) {
//     if (Sc->Balls[i].x + Sc->Balls[i].radius >= Sc->width) {
//       Sc->Balls[i].x = Sc->width - Sc->Balls[i].radius;
//       Sc->Balls[i].vx *= -0.9;
//       cleardevice();
//       /*bricks(Sc);*/
//     }
//     if (Sc->Balls[i].x - Sc->Balls[i].radius <= 0) {
//       Sc->Balls[i].x = Sc->Balls[i].radius;
//       Sc->Balls[i].vx *= -0.9;
//       cleardevice();
//       /*bricks(Sc);*/
//     }
//     if (Sc->Balls[i].radius + Sc->Balls[i].y >= Sc->height) {
//       Sc->num--;
//       for (int j = i; j < Sc->num; j++) {
//         Sc->Balls[j].x = Sc->Balls[j + 1].x;
//         Sc->Balls[j].y = Sc->Balls[j + 1].y;
//         Sc->Balls[j].vx = Sc->Balls[j + 1].vx;
//         Sc->Balls[j].vy = Sc->Balls[j + 1].vy;
//         Sc->Balls[j].radius = Sc->Balls[j + 1].radius;
//       }
//       cleardevice();
//       bricks(Sc);
//       if (Sc->num < 1) {
//         Sc->lifes -= 1;
//         if (Sc->lifes == 2)
//           outtextxy(5, 450, "2 lifes remaining");
//         if (Sc->lifes == 1)
//           outtextxy(5, 450, "1 life remaining");
//         Sc->num++;
//         delay(500);
//         Sc->Balls[0].x = 300;
//         Sc->Balls[0].y = 300;
//         Sc->Balls[0].vx = 0.5;
//         Sc->Balls[0].vy = 0.5;
//       }
//     }
//     if (Sc->Balls[i].y - Sc->Balls[i].radius <= 0) {
//       Sc->Balls[i].y = Sc->Balls[i].radius;
//       Sc->Balls[i].vy *= -1.3;
//       cleardevice();
//       // bricks(Sc);*/
//     }
//   }
// }
// void PlatformCheck(Scene *Sc) {
//   for (int i = 0; i < Sc->num; i++) {
//     if (Sc->Balls[i].radius + Sc->Balls[i].y >= Sc->height - 27) {
//       //������ �������� ��� ������� ������
//       if (Sc->Balls[i].x > Sc->paddleX - 40 &&
//           Sc->Balls[i].x < Sc->paddleX - 30) {
//         Sc->Balls[i].vy *= -1;
//         Sc->Balls[i].vx *= -2;
//       }
//       if (Sc->Balls[i].x > Sc->paddleX - 30 &&
//           Sc->Balls[i].x < Sc->paddleX - 20) {
//         Sc->Balls[i].vy *= -1;
//         Sc->Balls[i].vx *= 1.6;
//       }
//       if (Sc->Balls[i].x > Sc->paddleX - 20 &&
//           Sc->Balls[i].x < Sc->paddleX - 10) {
//         Sc->Balls[i].vy *= -1;
//         Sc->Balls[i].vx *= 1.3;
//       }
//       if (Sc->Balls[i].x > Sc->paddleX - 10 &&
//           Sc->Balls[i].x < Sc->paddleX + 10) {
//         Sc->Balls[i].vy *= -1;
//         Sc->Balls[i].vx *= 0.6;
//       }
//       if (Sc->Balls[i].x > Sc->paddleX + 10 &&
//           Sc->Balls[i].x < Sc->paddleX + 20) {
//         Sc->Balls[i].vy *= -1;
//         Sc->Balls[i].vx *= 1.3;
//       }
//       if (Sc->Balls[i].x > Sc->paddleX + 20 &&
//           Sc->Balls[i].x < Sc->paddleX + 30) {
//         Sc->Balls[i].vy *= -1;
//         Sc->Balls[i].vx *= 1.6;
//       }
//       if (Sc->Balls[i].x > Sc->paddleX + 30 &&
//           Sc->Balls[i].x < Sc->paddleX + 40) {
//         Sc->Balls[i].vy *= -1;
//         Sc->Balls[i].vx *= -2;
//       }
//       //������������ �������� ������, � �� ����� ������ ��� �������
//       if (Sc->Balls[i].vx > 10)
//         Sc->Balls[i].vx = 9.9;
//       if (Sc->Balls[i].vx < 0.5)
//         Sc->Balls[i].vx *= 2;
//     }
//   }
// }
// void BlockHit(Scene *Sc) {
//   for (int i = 0; i < Sc->row; i++)
//     for (int j = 0; j < Sc->column; j++) {
//       //�������� �� ��, ��� ���� ���������� � �� ��� ��� ����� �������
//       if (Sc->AllBricks[i][j].type > 0) {
//         for (int h = 0; h < Sc->num; h++) {
//           //���� �������� �� �������
//           if (Sc->Balls[h].y - Sc->Balls[h].radius <=
//               Sc->AllBricks[i][j].ly + 15) {
//             if (Sc->Balls[h].radius + Sc->Balls[h].y >=
//                 Sc->AllBricks[i][j].ly) {
//               if (Sc->Balls[h].x - Sc->Balls[h].radius <=
//                   Sc->AllBricks[i][j].lx + 41) {
//                 if (Sc->Balls[h].radius + Sc->Balls[h].x >=
//                     Sc->AllBricks[i][j].lx) {
//                   //�������b �� �� ������ ���� ���� ��� ������ �����
//                   if (Sc->AllBricks[i][j].type == 1)
//                     Sc->Score += 100;
//                   if (Sc->AllBricks[i][j].type == 2)
//                     Sc->Score += 200;
//                   if (Sc->AllBricks[i][j].type == 3) {
//                     Sc->Score += 500;
//                     Sc->num += 2;
//                     for (int e = Sc->num - 2; e < Sc->num; e++) {
//                       Sc->Balls[e].x = 9;
//                       Sc->Balls[e].y = 220 + (20 * e);
//                       Sc->Balls[e].radius = 8;
//                       Sc->Balls[e].vx = 0.5 * e;
//                       Sc->Balls[e].vy = 0.3 * e;
//                     }
//                   }
//                   if (Sc->AllBricks[i][j].type == 4) {
//                     Sc->Score += 300;
//                     Sc->paddleXSpeed *= 2;
//                   }
//                   //�������� �����, ��������� ��������
//                   if (Sc->AllBricks[i][j].type < 9)
//                     Sc->AllBricks[i][j].type = 0;
//                   Sc->Balls[h].vy *= -1.05;
//                   Sc->Balls[h].vx *= -1.05;
//                   //������ ����� ������� ������������ �������, ������ ��� �-�
//                   //����������� ��. ��������
//                   blackbricks();
//                   bricks(Sc);
//                 }
//               }
//             }
//           }
//         }
//       }
//     }
// }
// int BlocksLeft(Scene *Sc) {
//   //������� ���������� ������
//   int count = 0;
//   for (int i = 0; i < Sc->row; i++)
//     for (int j = 0; j < Sc->column; j++)
//       if (Sc->AllBricks[i][j].type > 0 && Sc->AllBricks[i][j].type < 9) {
//         count++;
//       }
//   return count;
// }
// //�������
// void Background() {
//   int maxx, maxy, midx, midy;
//   midx = 320;
//   midy = 240;
//   maxx = 639;
//   maxy = 479;
//   rectangle(0, 0, maxx, maxy - 12);
//   rectangle(midx - 24, maxy - 21, midx + 25, maxy - 12);
//   floodfill(midx, maxy - 13, WHITE);
//   setcolor(BLUE);
//   circle(midx, maxy - 29, 8);
//   floodfill(midx, maxy - 30, BLUE);
//   settextstyle(0, 0, 7);
//   outtextxy(270, 200, "Lagonoid");
//   settextstyle(0, 0, 1);
// }
// void DrawMenu() {
//   settextstyle(0, 0, 1);
//   Background();

//   //������� �������
//   setcolor(WHITE);
//   outtextxy(15, 135, "Select any of the following:");
//   rectangle(15, 164, 85, 182);
//   rectangle(15, 184, 158, 202);
//   rectangle(15, 204, 113, 222);
//   rectangle(15, 224, 85, 242);
//   outtextxy(20, 165, "Play(P)");
//   outtextxy(20, 185, "Instructions(I)");
//   outtextxy(20, 205, "Records(R)");
//   outtextxy(20, 225, "Exit(E)");
// }

// const char *LEVEL_FILE_BASE_PATH = "./resources/level";
// const int NUMBER_MAX_LENGTH = 5;

// char *concatLevelPath(char *value) {
//   char *path_with_level_number =
//       (char *)malloc(strlen(LEVEL_FILE_BASE_PATH) + strlen(value));
//   strcpy(path_with_level_number, LEVEL_FILE_BASE_PATH);
//   strcat(path_with_level_number, value);

//   return path_with_level_number;
// }

// char *mapKeyToLevel(int asci_number) {
//   char parsedNumber[NUMBER_MAX_LENGTH];
//   sprintf(parsedNumber, "%d", asci_number);

//   return concatLevelPath(parsedNumber);
// }

// bool DrawLevelMenu(Scene *Sc) {
//   Background();
//   setcolor(WHITE);
//   outtextxy(15, 135, (char *)"Chose level:");
//   rectangle(15, 164, 120, 182);
//   rectangle(15, 184, 120, 202);
//   rectangle(15, 204, 120, 222);
//   rectangle(15, 224, 120, 242);
//   rectangle(15, 244, 120, 262);
//   outtextxy(20, 165, "Level 1 (1)");
//   outtextxy(20, 185, "Level 2 (2)");
//   outtextxy(20, 205, "Level 3 (3)");
//   outtextxy(20, 225, "Level 4 (4)");
//   outtextxy(20, 245, "Level 5 (5)");
//   outtextxy(20, 265, "Continue(C)");
//   rectangle(15, 264, 120, 282);

//   while (true) {
//     if (kbhit()) {
//       int key = getch();
//       if (key == 0)
//         key = getch();

//       switch (key) {
//       case '1':
//       case '2':
//       case '3':
//       case '4':
//       case '5': {
//         char keyChar[] = {(char)key};
//         char *levelPath = concatLevelPath(keyChar);

//         return loadLevel(levelPath, Sc);
//       }
//       case 'c':
//         return loadLevel("save", Sc);
//       case 27:
//         return 0;
//       }
//     }
//   };
// }

// void threadFunctionMusic() {}

// void initmusic() {
//   HANDLE hThread;
//   BOOL Active;
//   DWORD ThreadId;
//   hThread = CreateThread(NULL, 0,
//   (LPTHREAD_START_ROUTINE)threadFunctionMusic,
//                          (void *)&Active, 0, &ThreadId);
//   if (hThread == NULL) {
//     outtextxy(140, 135, "CreateThread");
//   }
//   while (Active == TRUE)
//     ;
//   CloseHandle(hThread);
// }
// void PaddleAndControls(Scene *Sc) {
//   int maxx, maxy;
//   maxx = 639;
//   maxy = 479;
//   UINT key_1 = MapVirtualKey(27, 1);
//   UINT key_2 = MapVirtualKey(77, 1);
//   UINT key_3 = MapVirtualKey(75, 1);
//   if (GetAsyncKeyState(key_2)) {
//     setcolor(BLACK);
//     rectangle(Sc->paddleX - 40, maxy - 26, Sc->paddleX + 40, maxy - 13);
//     setcolor(WHITE);
//     Sc->paddleX += Sc->paddleXSpeed;
//   }
//   if (GetAsyncKeyState(key_3)) {
//     setcolor(BLACK);
//     rectangle(Sc->paddleX - 40, maxy - 26, Sc->paddleX + 40, maxy - 13);
//     setcolor(WHITE);
//     Sc->paddleX -= Sc->paddleXSpeed;
//   }
//   if (Sc->paddleX < 0)
//     Sc->paddleX = 639;
//   if (Sc->paddleX > 639)
//     Sc->paddleX = 1;
//   rectangle(Sc->paddleX - 40, maxy - 26, Sc->paddleX + 40, maxy - 12);
// }

// void BallsCollistionCheck() {
//   for (int i = 0; i < scene.num; i++) {
//     for (int j = 0; j < scene.num; j++) {
//       if (i == j) {
//         continue;
//       }

//       if (scene.Balls[i].x + scene.Balls[i].radius / 2 < scene.Balls[j].x &&
//           scene.Balls[i].x - scene.Balls[i].radius / 2 > scene.Balls[j].x &&
//           scene.Balls[i].y + scene.Balls[i].radius / 2 < scene.Balls[j].y &&
//           scene.Balls[i].y - scene.Balls[i].radius / 2 > scene.Balls[j].y) {
//         outtextxy(20, 185, "Collision detected... Self terminating");
//         scene.Balls[i].vx *= 4;
//         scene.Balls[i].vy *= 4;
//       }
//     }
//   }
// }

// int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//                      LPSTR lpCmdLine, int nCmdShow) {
//   //   int maxx, maxy;
//   //   maxx = 639;
//   //   maxy = 479;
//   time_t timestart;
//   time_t current_time;
//   char Time[100];

//   auto Sc = GetScene();
//   Sc.width = WINDOW_WIDTH;
//   Sc.height = WINDOW_HEIGHT;
//   initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Arkanoid v0.5");

//   //��������� ���� � ������ �����������
//   setusercharsize(20, 20, 20, 20);
//   DrawMenu();
//   initmusic();
//   do {
//     if (kbhit()) {
//       int key = getch();
//       if (key == 0)
//         key = getch();
//       switch (key) {
//         // p
//       case 80:
//       case 112:
//         cleardevice();
//         if (!DrawLevelMenu(&Sc)) {
//           cleardevice();
//           DrawMenu();
//           continue;
//         } else {
//           // ���������� ���������� ������ � �����, ���� ��� ������ ���������
//           // ����� ���
//           cleardevice();
//           //����� ��� �������
//           timestart = time(NULL);
//           if (timestart == ((time_t)-1)) {
//             return EXIT_FAILURE;
//           }
//           while (Sc.lifes > 0) {
//             // rectangle(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 13);
//             rectangle(0, 0, 639 - 1, 479 - 12);
//             bricks(&Sc);
//             //�������� ������������ c� �������� � �������� ������������ �
//             ����� Window(&Sc);
//             //�������� ������������ � ������ � ��� �����������
//             BlockHit(&Sc);
//             //����������, ��������� � �������� ������������ � ������
//             PaddleAndControls(&Sc);
//             //�������� ������������
//             PlatformCheck(&Sc);
//             //����� �������� �����
//             BallsCollistionCheck();
//             //������� ��������� � ��������� ����
//             DrawBall(&Sc);
//             //����
//             sprintf(Sc.TableScore, "Score:%d", Sc.Score);
//             outtextxy(530, 2, Sc.TableScore);
//             //�����
//             current_time = time(NULL);
//             if (current_time == ((time_t)-1)) {
//               (void)fprintf(stderr, "Failure to compute the current time.");
//               return EXIT_FAILURE;
//             }
//             sprintf(Time, "Time: %0.0lf",
//                     difftime(current_time, timestart) + Sc.timer);
//             outtextxy(30, 2, Time);

//             //���� ��������� �����
//             int blocksleft = BlocksLeft(&Sc);
//             if (blocksleft == 0) {
//               Sc.timer += difftime(current_time, timestart);
//               settextstyle(0, 0, 7);
//               outtextxy(30, 200, "Level complete");
//               settextstyle(0, 0, 1);
//               // SaveResults();
//               delay(1000);
//               if (loadLevel("level2.txt", &Sc)) {
//                 cleardevice();
//                 continue;
//               } else {
//                 Sc.Score = Sc.Score + (1000 - Sc.timer) * 100;
//                 SaveRecords(&Sc);
//                 break;
//               }
//             }

//             //����������� �����
//             if (kbhit()) {
//               int key = getch();
//               if (key == 27) {
//                 Sc.timer += difftime(current_time, timestart);
//                 Save(&Sc);
//                 Sc.lifes = 0;
//               }
//             }
//             delay(5);
//           }
//           Sc.Score = Sc.Score + (1000 - Sc.timer) * 10;
//           SaveRecords(&Sc);
//           cleardevice();
//           DrawMenu();
//         }
//         break;

//         // i
//       case 73:
//       case 'i':
//         cleardevice();
//         outtextxy(
//             30, 10,
//             "��������� - �����-����� ��������� ��� ������������ �������
//             DOS");
//         outtextxy(240, 30, "����������: <- ->");
//         outtextxy(30, 50,
//                   "��� �������������� ������� �������� ���� �� ������ �
//                   ������ " "level");
//         break;
//         // r
//       case 82:
//       case 114:
//         cleardevice(); // records
//         LoadRecords("Records.txt");
//         break;
//         // Esc
//       case 27:
//         cleardevice(); // Escape
//         DrawMenu();
//         break;

//         // e
//       case 69:
//       case 101:
//         exit(1); // Exit
//       }
//     }
//   } while (true);
// }