#include "./Lib/graphics.h"
#include "time.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_THREADS 3
using namespace std;

struct Brick {
  int lx, ly;
  int type;
};
struct Ball {
  double x, y;
  double radius;
  double vx, vy;
};
struct Scene {
  //�����
  Ball Balls[20];
  struct Ball *balss;
  int num;

  //�����
  Brick AllBricks[100][100];
  int row, column;

  //�����
  double width, height;
  //�������� �����
  int Score;
  char TableScore[20];
  // Paddle
  double paddleX;
  double paddleXSpeed;
  //����� � ������
  int lifes;
  double timer;
};
bool DynamicCreateBalls(Scene *Sc) {
  Sc->balss = (struct Ball *)malloc(Sc->num * sizeof(Ball));
  return false;
}
//�������� � ����������
bool LoadLevel(const char name[25], Scene *Sc) {
  FILE *level = fopen(name, "rt");
  if (!level) {
    outtextxy(320, 220, "Failed to load Level file");
    delay(1000);
    return 0;
  }

  //�������� ���� � ��� ���������
  fscanf(level, "%d", &Sc->num);
  for (int i = 0; i < Sc->num; i++) {
    fscanf(level, "%lf", &Sc->Balls[i].x);
    fscanf(level, "%lf", &Sc->Balls[i].y);
    fscanf(level, "%lf", &Sc->Balls[i].radius);
    fscanf(level, "%lf", &Sc->Balls[i].vx);
    fscanf(level, "%lf", &Sc->Balls[i].vy);
  }
  fscanf(level, "%lf", &Sc->timer);
  fscanf(level, "%d", &Sc->Score);
  fscanf(level, "%d", &Sc->lifes);
  fscanf(level, "%lf", &Sc->paddleX);
  fscanf(level, "%lf", &Sc->paddleXSpeed);
  fscanf(level, "%d", &Sc->row);
  fscanf(level, "%d", &Sc->column);
  //�����
  for (int i = 0; i < Sc->row; i++)
    for (int j = 0; j < Sc->column; j++)
      fscanf(level, "%d", &Sc->AllBricks[i][j].type);

  fclose(level);
  return 1;
}
bool Save(Scene *Sc) {
  FILE *level = fopen("Save.txt", "wt");
  if (!level) {
    outtextxy(320, 220, "Failed to save Level file");
    delay(1000);
    return 0;
  }
  //�������� ���� � ��� ���������
  fprintf(level, "%d\n", Sc->num);
  for (int i = 0; i < Sc->num; i++) {
    fprintf(level, "%lf ", Sc->Balls[i].x);
    fprintf(level, "%lf ", Sc->Balls[i].y);
    fprintf(level, "%lf ", Sc->Balls[i].radius);
    fprintf(level, "%lf ", Sc->Balls[i].vx);
    fprintf(level, "%lf ", Sc->Balls[i].vy);
  }
  fprintf(level, "\n");
  fprintf(level, "%lf\n", Sc->timer);
  fprintf(level, "%d\n", Sc->Score);
  fprintf(level, "%d\n", Sc->lifes);
  fprintf(level, "%lf\n", Sc->paddleX);
  fprintf(level, "%lf\n", Sc->paddleXSpeed);
  fprintf(level, "%d\n", Sc->row);
  fprintf(level, "%d\n", Sc->column);
  //�����
  for (int i = 0; i < Sc->row; i++) {
    for (int j = 0; j < Sc->column; j++)
      fprintf(level, "%d ", Sc->AllBricks[i][j].type);
    fprintf(level, "\n");
  }

  fclose(level);
}
bool LoadRecords(const char name[20]) {
  FILE *Score;
  int score;
  Score = fopen(name, "rt");
  if (!Score) {
    outtextxy(220, 220, "Failed to load records Score");
    return 0;
  }

  char Records[150];
  int y = 160;
  setcolor(WHITE);
  while (!feof(Score)) {
    fgets(Records, 150, Score);
    // fscanf(Score, "%d", score);
    outtextxy(400, y, Records);
    y += 20;
  }

  fclose(Score);
  return 1;
}
bool SaveRecords(Scene *Sc) {
  FILE *Score;
  Score = fopen("./resources/saveRecords", "rt");
  if (!Score) {
    outtextxy(220, 220, "Failed to save records");
    return 0;
  }
  setcolor(WHITE);
  while (!feof(Score)) {
    fprintf(Score, "Enter name %d\n", Sc->Score);
  }

  fclose(Score);
  return 1;
}

//���������
void drawbrick(int i, int j, Scene *Sc) {
  switch (Sc->AllBricks[i][j].type) {
    //��� �����
  case 0:
    break;
    //���� �������
  case 1:
    setcolor(WHITE);
    rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
              Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
    rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
              Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 - 2);
    break;
    //���� � ��������� ������
  case 2:
    setcolor(YELLOW);
    rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
              Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
    rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
              Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 - 2);
    setcolor(WHITE);
    break;
    //���� � ��������� ��������
  case 3:
    setcolor(RED);
    rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
              Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
    rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
              Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 - 2);
    setcolor(WHITE);
    break;
  case 4:
    setcolor(LIGHTBLUE);
    rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
              Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
    rectangle(Sc->AllBricks[i][j].lx + 2, Sc->AllBricks[i][j].ly + 2,
              Sc->AllBricks[i][j].lx + 41 - 2, Sc->AllBricks[i][j].ly + 15 - 2);
    setcolor(WHITE);
    break;
  case 9:
    setcolor(LIGHTGRAY);
    rectangle(Sc->AllBricks[i][j].lx, Sc->AllBricks[i][j].ly,
              Sc->AllBricks[i][j].lx + 41, Sc->AllBricks[i][j].ly + 15);
    floodfill(Sc->AllBricks[i][j].lx + 10, Sc->AllBricks[i][j].ly + 7,
              LIGHTGRAY);
    setcolor(WHITE);
  }
}
void bricks(Scene *Sc) {
  int i, j;
  for (i = 0; i < Sc->row; i++) // 5rows
  {
    for (j = 0; j < Sc->column; j++) // 10columns
    {
      drawbrick(i, j, Sc);
      Sc->AllBricks[i][j].lx = 60 + (52 * j);
      Sc->AllBricks[i][j].ly = 20 + (20 * i);
    }
  }
}
void blackbricks() {
  int i, j, lx = 60, ly = 20;
  for (i = 0; i < 5; i++) // 5rows
  {
    for (j = 0; j < 10; j++) // 10columns
    {
      setcolor(BLACK);
      rectangle(lx, ly, lx + 41, ly + 15);
      rectangle(lx + 2, ly + 2, lx + 41 - 2, ly + 15 - 2);
      setcolor(WHITE);
      lx = lx + 52;
    }
    lx = 60;
    ly = ly + 20;
  }
}
//��������� ����
void DrawBall(Scene *Sc) {
  for (int i = 0; i < Sc->num; i++) {
    setcolor(BLACK);
    circle(Sc->Balls[i].x, Sc->Balls[i].y, Sc->Balls[i].radius);
    setcolor(WHITE);
    Sc->Balls[i].x = Sc->Balls[i].x + Sc->Balls[i].vx;
    Sc->Balls[i].y = Sc->Balls[i].y + Sc->Balls[i].vy;
    circle(Sc->Balls[i].x, Sc->Balls[i].y, Sc->Balls[i].radius);
  }
}
//�������� ������������
void Window(Scene *Sc) {
  for (int i = 0; i < Sc->num; i++) {
    if (Sc->Balls[i].x + Sc->Balls[i].radius >= Sc->width) {
      Sc->Balls[i].x = Sc->width - Sc->Balls[i].radius;
      Sc->Balls[i].vx *= -0.9;
      cleardevice();
      /*bricks(Sc);*/
    }
    if (Sc->Balls[i].x - Sc->Balls[i].radius <= 0) {
      Sc->Balls[i].x = Sc->Balls[i].radius;
      Sc->Balls[i].vx *= -0.9;
      cleardevice();
      /*bricks(Sc);*/
    }
    if (Sc->Balls[i].radius + Sc->Balls[i].y >= Sc->height) {
      Sc->num--;
      for (int j = i; j < Sc->num; j++) {
        Sc->Balls[j].x = Sc->Balls[j + 1].x;
        Sc->Balls[j].y = Sc->Balls[j + 1].y;
        Sc->Balls[j].vx = Sc->Balls[j + 1].vx;
        Sc->Balls[j].vy = Sc->Balls[j + 1].vy;
        Sc->Balls[j].radius = Sc->Balls[j + 1].radius;
      }
      cleardevice();
      bricks(Sc);
      if (Sc->num < 1) {
        Sc->lifes -= 1;
        if (Sc->lifes == 2)
          outtextxy(5, 450, "2 lifes remaining");
        if (Sc->lifes == 1)
          outtextxy(5, 450, "1 life remaining");
        Sc->num++;
        delay(500);
        Sc->Balls[0].x = 300;
        Sc->Balls[0].y = 300;
        Sc->Balls[0].vx = 0.5;
        Sc->Balls[0].vy = 0.5;
      }
    }
    if (Sc->Balls[i].y - Sc->Balls[i].radius <= 0) {
      Sc->Balls[i].y = Sc->Balls[i].radius;
      Sc->Balls[i].vy *= -1.3;
      cleardevice();
      // bricks(Sc);*/
    }
  }
}
void PlatformCheck(Scene *Sc) {
  for (int i = 0; i < Sc->num; i++) {
    if (Sc->Balls[i].radius + Sc->Balls[i].y >= Sc->height - 27) {
      //������ �������� ��� ������� ������
      if (Sc->Balls[i].x > Sc->paddleX - 40 &&
          Sc->Balls[i].x < Sc->paddleX - 30) {
        Sc->Balls[i].vy *= -1;
        Sc->Balls[i].vx *= -2;
      }
      if (Sc->Balls[i].x > Sc->paddleX - 30 &&
          Sc->Balls[i].x < Sc->paddleX - 20) {
        Sc->Balls[i].vy *= -1;
        Sc->Balls[i].vx *= 1.6;
      }
      if (Sc->Balls[i].x > Sc->paddleX - 20 &&
          Sc->Balls[i].x < Sc->paddleX - 10) {
        Sc->Balls[i].vy *= -1;
        Sc->Balls[i].vx *= 1.3;
      }
      if (Sc->Balls[i].x > Sc->paddleX - 10 &&
          Sc->Balls[i].x < Sc->paddleX + 10) {
        Sc->Balls[i].vy *= -1;
        Sc->Balls[i].vx *= 0.6;
      }
      if (Sc->Balls[i].x > Sc->paddleX + 10 &&
          Sc->Balls[i].x < Sc->paddleX + 20) {
        Sc->Balls[i].vy *= -1;
        Sc->Balls[i].vx *= 1.3;
      }
      if (Sc->Balls[i].x > Sc->paddleX + 20 &&
          Sc->Balls[i].x < Sc->paddleX + 30) {
        Sc->Balls[i].vy *= -1;
        Sc->Balls[i].vx *= 1.6;
      }
      if (Sc->Balls[i].x > Sc->paddleX + 30 &&
          Sc->Balls[i].x < Sc->paddleX + 40) {
        Sc->Balls[i].vy *= -1;
        Sc->Balls[i].vx *= -2;
      }
      //������������ �������� ������, � �� ����� ������ ��� �������
      if (Sc->Balls[i].vx > 10)
        Sc->Balls[i].vx = 9.9;
      if (Sc->Balls[i].vx < 0.5)
        Sc->Balls[i].vx *= 2;
    }
  }
}
void BlockHit(Scene *Sc) {
  for (int i = 0; i < Sc->row; i++)
    for (int j = 0; j < Sc->column; j++) {
      //�������� �� ��, ��� ���� ���������� � �� ��� ��� ����� �������
      if (Sc->AllBricks[i][j].type > 0) {
        for (int h = 0; h < Sc->num; h++) {
          //���� �������� �� �������
          if (Sc->Balls[h].y - Sc->Balls[h].radius <=
              Sc->AllBricks[i][j].ly + 15) {
            if (Sc->Balls[h].radius + Sc->Balls[h].y >=
                Sc->AllBricks[i][j].ly) {
              if (Sc->Balls[h].x - Sc->Balls[h].radius <=
                  Sc->AllBricks[i][j].lx + 41) {
                if (Sc->Balls[h].radius + Sc->Balls[h].x >=
                    Sc->AllBricks[i][j].lx) {
                  //�������b �� �� ������ ���� ���� ��� ������ �����
                  if (Sc->AllBricks[i][j].type == 1)
                    Sc->Score += 100;
                  if (Sc->AllBricks[i][j].type == 2)
                    Sc->Score += 200;
                  if (Sc->AllBricks[i][j].type == 3) {
                    Sc->Score += 500;
                    Sc->num += 2;
                    for (int e = Sc->num - 2; e < Sc->num; e++) {
                      Sc->Balls[e].x = 9;
                      Sc->Balls[e].y = 220 + (20 * e);
                      Sc->Balls[e].radius = 8;
                      Sc->Balls[e].vx = 0.5 * e;
                      Sc->Balls[e].vy = 0.3 * e;
                    }
                  }
                  if (Sc->AllBricks[i][j].type == 4) {
                    Sc->Score += 300;
                    Sc->paddleXSpeed *= 2;
                  }
                  //�������� �����, ��������� ��������
                  if (Sc->AllBricks[i][j].type < 9)
                    Sc->AllBricks[i][j].type = 0;
                  Sc->Balls[h].vy *= -1.05;
                  Sc->Balls[h].vx *= -1.05;
                  //������ ����� ������� ������������ �������, ������ ��� �-�
                  //����������� ��. ��������
                  blackbricks();
                  bricks(Sc);
                }
              }
            }
          }
        }
      }
    }
}
int BlocksLeft(Scene *Sc) {
  //������� ���������� ������
  int count = 0;
  for (int i = 0; i < Sc->row; i++)
    for (int j = 0; j < Sc->column; j++)
      if (Sc->AllBricks[i][j].type > 0 && Sc->AllBricks[i][j].type < 9) {
        count++;
      }
  return count;
}
//�������
void Background() {
  int maxx, maxy, midx, midy;
  midx = 320;
  midy = 240;
  maxx = 639;
  maxy = 479;
  rectangle(0, 0, maxx, maxy - 12);
  rectangle(midx - 24, maxy - 21, midx + 25, maxy - 12);
  floodfill(midx, maxy - 13, WHITE);
  setcolor(BLUE);
  circle(midx, maxy - 29, 8);
  floodfill(midx, maxy - 30, BLUE);
  settextstyle(0, 0, 7);
  outtextxy(270, 200, "Lagonoid");
  settextstyle(0, 0, 1);
}
void DrawMenu() {
  settextstyle(0, 0, 1);
  Background();
  // initanimMenu();
  //������� �������
  setcolor(WHITE);
  outtextxy(15, 135, "Select any of the following:");
  rectangle(15, 164, 85, 182);
  rectangle(15, 184, 158, 202);
  rectangle(15, 204, 113, 222);
  rectangle(15, 224, 85, 242);
  outtextxy(20, 165, "Play(P)");
  outtextxy(20, 185, "Instructions(I)");
  outtextxy(20, 205, "Records(R)");
  outtextxy(20, 225, "Exit(E)");
}
bool DrawLevelMenu(Scene *Sc) {
  Background();
  setcolor(WHITE);
  outtextxy(15, 135, "Chose level:");
  rectangle(15, 164, 120, 182);
  rectangle(15, 184, 120, 202);
  rectangle(15, 204, 120, 222);
  rectangle(15, 224, 120, 242);
  rectangle(15, 244, 120, 262);
  outtextxy(20, 165, "Level 1 (1)");
  outtextxy(20, 185, "Level 2 (2)");
  outtextxy(20, 205, "Level 3 (3)");
  outtextxy(20, 225, "Level 4 (4)");
  outtextxy(20, 245, "Level 5 (5)");
  outtextxy(20, 265, "Continue(C)");
  rectangle(15, 264, 120, 282);

  do {
    if (kbhit()) {
      int key = getch();
      if (key == 0)
        key = getch();
      switch (key) {
      case '1':
        if (LoadLevel("./resources/level1", Sc))
          return 1;
        else
          return 0;
        break;
      case '2':
        if (LoadLevel("./resources/level2", Sc))
          return 1;
        else
          return 0;
        break;
      case '3':
        if (LoadLevel("./resources/level3", Sc))
          return 1;
        else
          return 0;
        break;
      case '4':
        if (LoadLevel("./resources/level4", Sc))
          return 1;
        else
          return 0;
        break;
      case '5':
        if (LoadLevel("./resources/level5", Sc))
          return 1;
        else
          return 0;
        break;
      case 'c':
        if (LoadLevel("./resources/save", Sc))
          return 1;
        else
          return 0;
        break;
      case 27:
        return 0;
      }
    }
  } while (true);
}
void animMenu() {
  int x;
  int y;
  do {
    x = mousex();
    y = mousey();
    if (x < 165 && y > 164 && y < 182) {
      setcolor(YELLOW);
      outtextxy(20, 165, "Play(P)");
    } else {
      setcolor(WHITE);
      outtextxy(20, 165, "Play(P)");
    }

    if (x < 165 && y > 184 && y < 202) {
      setcolor(YELLOW);
      outtextxy(20, 185, "Instructions(I)");
    } else {
      setcolor(WHITE);
      outtextxy(20, 185, "Instructions(I)");
    }

    if (x < 165 && y > 204 && y < 222) {
      setcolor(YELLOW);
      outtextxy(20, 205, "Records(R)");
    } else {
      setcolor(WHITE);
      outtextxy(20, 205, "Records(R)");
    }

    if (x < 165 && y > 224 && y < 242) {
      setcolor(YELLOW);
      outtextxy(20, 225, "Exit(E)");
    } else {
      setcolor(WHITE);
      outtextxy(20, 225, "Exit(E)");
    }
    if (kbhit()) {
      int key = getch();
      if (key == 0)
        key = getch();
      switch (key)
      case 'p':
        return;
    }
  } while (true);
}
void initanimMenu() {
  HANDLE hThread;
  BOOL Active;
  DWORD ThreadId;
  hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)animMenu,
                         (void *)&Active, 0, &ThreadId);
  if (hThread == NULL) {
    outtextxy(20, 135, "CreateThread");
  }
  while (Active == TRUE)
    ;
  CloseHandle(hThread);
}
//������
void threadFunctionMusic() {
  int type = 3;
  switch (type) {
  case 1:
    while (true) {
      // "Metallica : Harvester of Sorrow"
      Beep(329, 300);
      Beep(493, 300);
      Beep(698, 300);
      Beep(659, 600);
      Beep(783, 300);
      Beep(698, 300);
      Beep(659, 600);
      Beep(329, 100);
      Beep(493, 300);
      Beep(698, 300);
      Beep(659, 600);
      Beep(392, 250);
      Beep(440, 200);
      Beep(587, 300);
      Beep(349, 250);
      Beep(587, 500);
      Beep(329, 300);
      Beep(493, 300);
      Beep(698, 300);
      Beep(659, 600);
      Beep(783, 300);
      Beep(698, 300);
      Beep(659, 600);
      Beep(329, 100);
      Beep(493, 300);
      Beep(698, 300);
      Beep(659, 600);
      Beep(392, 250);
      Beep(440, 200);
      Beep(587, 300);
      Beep(349, 250);
      Beep(587, 400);
    }
    break;
  case 2:
    while (true) {
      // "Mario"
      Beep(330, 100);
      Sleep(100);
      Beep(330, 100);
      Sleep(300);
      Beep(330, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(100);
      Beep(330, 100);
      Sleep(300);
      Beep(392, 100);
      Sleep(700);
      Beep(196, 100);
      Sleep(700);
      Beep(262, 300);
      Sleep(300);
      Beep(196, 300);
      Sleep(300);
      Beep(164, 300);
      Sleep(300);
      Beep(220, 300);
      Sleep(100);
      Beep(246, 100);
      Sleep(300);
      Beep(233, 200);
      Beep(220, 100);
      Sleep(300);
      Beep(196, 100);
      Sleep(150);
      Beep(330, 100);
      Sleep(150);
      Beep(392, 100);
      Sleep(150);
      Beep(440, 100);
      Sleep(300);
      Beep(349, 100);
      Sleep(100);
      Beep(392, 100);
      Sleep(300);
      Beep(330, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(100);
      Beep(294, 100);
      Sleep(100);
      Beep(247, 100);
      Sleep(500);
      Beep(262, 300);
      Sleep(300);
      Beep(196, 300);
      Sleep(300);
      Beep(164, 300);
      Sleep(300);
      Beep(220, 300);
      Sleep(100);
      Beep(246, 100);
      Sleep(300);
      Beep(233, 200);
      Beep(220, 100);
      Sleep(300);
      Beep(196, 100);
      Sleep(150);
      Beep(330, 100);
      Sleep(150);
      Beep(392, 100);
      Sleep(150);
      Beep(440, 100);
      Sleep(300);
      Beep(349, 100);
      Sleep(100);
      Beep(392, 100);
      Sleep(300);
      Beep(330, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(100);
      Beep(294, 100);
      Sleep(100);
      Beep(247, 100);
      Sleep(900);
      Beep(392, 100);
      Sleep(100);
      Beep(370, 100);
      Sleep(100);
      Beep(349, 100);
      Sleep(100);
      Beep(311, 100);
      Sleep(300);
      Beep(330, 100);
      Sleep(300);
      Beep(207, 100);
      Sleep(100);
      Beep(220, 100);
      Sleep(100);
      Beep(262, 100);
      Sleep(300);
      Beep(220, 100);
      Sleep(100);
      Beep(262, 100);
      Sleep(100);
      Beep(294, 100);
      Sleep(500);
      Beep(392, 100);
      Sleep(100);
      Beep(370, 100);
      Sleep(100);
      Beep(349, 100);
      Sleep(100);
      Beep(311, 100);
      Sleep(300);
      Beep(330, 100);
      Sleep(300);
      Beep(523, 100);
      Sleep(300);
      Beep(523, 100);
      Sleep(100);
      Beep(523, 100);
      Sleep(1100);
      Beep(392, 100);
      Sleep(100);
      Beep(370, 100);
      Sleep(100);
      Beep(349, 100);
      Sleep(100);
      Beep(311, 100);
      Sleep(300);
      Beep(330, 100);
      Sleep(300);
      Beep(207, 100);
      Sleep(100);
      Beep(220, 100);
      Sleep(100);
      Beep(262, 100);
      Sleep(300);
      Beep(220, 100);
      Sleep(100);
      Beep(262, 100);
      Sleep(100);
      Beep(294, 100);
      Sleep(500);
      Beep(311, 300);
      Sleep(300);
      Beep(296, 300);
      Sleep(300);
      Beep(262, 300);
      Sleep(1300);
      Beep(262, 100);
      Sleep(100);
      Beep(262, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(100);
      Beep(294, 100);
      Sleep(300);
      Beep(330, 200);
      Sleep(50);
      Beep(262, 200);
      Sleep(50);
      Beep(220, 200);
      Sleep(50);
      Beep(196, 100);
      Sleep(700);
      Beep(262, 100);
      Sleep(100);
      Beep(262, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(100);
      Beep(294, 100);
      Sleep(100);
      Beep(330, 100);
      Sleep(700);
      Beep(440, 100);
      Sleep(300);
      Beep(392, 100);
      Sleep(500);
      Beep(262, 100);
      Sleep(100);
      Beep(262, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(300);
      Beep(262, 100);
      Sleep(100);
      Beep(294, 100);
      Sleep(300);
      Beep(330, 200);
      Sleep(50);
      Beep(262, 200);
      Sleep(50);
      Beep(220, 200);
      Sleep(50);
      Beep(196, 100);
      Sleep(700);
    }
    break;
  case 3:
    while (true) {
      // Mario Theme by joker
      Beep(480, 200);
      Beep(1568, 200);
      Beep(1568, 200);
      Beep(1568, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(369.99, 200);
      Beep(392, 200);
      Beep(369.99, 200);
      Beep(392, 200);
      Beep(392, 400);
      Beep(196, 400);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(739.99, 200);
      Beep(83.99, 200);
      Beep(880, 200);
      Beep(830.61, 200);
      Beep(880, 200);
      Beep(987.77, 400);
      Beep(880, 200);
      Beep(783.99, 200);
      Beep(698.46, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(880, 200);
      Beep(830.61, 200);
      Beep(880, 200);
      Beep(987.77, 400);
      Sleep(200);
      Beep(1108, 10);
      Beep(1174.7, 200);
      Beep(1480, 10);
      Beep(1568, 200);
      Sleep(200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(783.99, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(880, 200);
      Beep(830.61, 200);
      Beep(880, 200);
      Beep(987.77, 400);
      Beep(880, 200);
      Beep(783.99, 200);
      Beep(698.46, 200);
      Beep(659.25, 200);
      Beep(698.46, 200);
      Beep(784, 200);
      Beep(880, 400);
      Beep(784, 200);
      Beep(698.46, 200);
      Beep(659.25, 200);
      Beep(587.33, 200);
      Beep(659.25, 200);
      Beep(698.46, 200);
      Beep(784, 400);
      Beep(698.46, 200);
      Beep(659.25, 200);
      Beep(587.33, 200);
      Beep(523.25, 200);
      Beep(587.33, 200);
      Beep(659.25, 200);
      Beep(698.46, 400);
      Beep(659.25, 200);
      Beep(587.33, 200);
      Beep(493.88, 200);
      Beep(523.25, 200);
      Sleep(400);
      Beep(349.23, 400);
      Beep(392, 200);
      Beep(329.63, 200);
      Beep(523.25, 200);
      Beep(493.88, 200);
      Beep(466.16, 200);
      Beep(440, 200);
      Beep(493.88, 200);
      Beep(523.25, 200);
      Beep(880, 200);
      Beep(493.88, 200);
      Beep(880, 200);
      Beep(1760, 200);
      Beep(440, 200);
      Beep(392, 200);
      Beep(440, 200);
      Beep(493.88, 200);
      Beep(783.99, 200);
      Beep(440, 200);
      Beep(783.99, 200);
      Beep(1568, 200);
      Beep(392, 200);
      Beep(349.23, 200);
      Beep(392, 200);
      Beep(440, 200);
      Beep(698.46, 200);
      Beep(415.2, 200);
      Beep(698.46, 200);
      Beep(1396.92, 200);
      Beep(349.23, 200);
      Beep(329.63, 200);
      Beep(311.13, 200);
      Beep(329.63, 200);
      Beep(659.25, 200);
      Beep(698.46, 400);
      Beep(783.99, 400);
      Beep(440, 200);
      Beep(493.88, 200);
      Beep(523.25, 200);
      Beep(880, 200);
      Beep(493.88, 200);
      Beep(880, 200);
      Beep(1760, 200);
      Beep(440, 200);
      Beep(392, 200);
      Beep(440, 200);
      Beep(493.88, 200);
      Beep(783.99, 200);
      Beep(440, 200);
      Beep(783.99, 200);
      Beep(1568, 200);
      Beep(392, 200);
      Beep(349.23, 200);
      Beep(392, 200);
      Beep(440, 00);
      Beep(698.46, 200);
      Beep(659.25, 200);
      Beep(698.46, 200);
      Beep(739.99, 200);
      Beep(783.99, 200);
      Beep(392, 200);
      Beep(392, 200);
      Beep(392, 200);
      Beep(392, 200);
      Beep(196, 200);
      Beep(196, 200);
      Beep(196, 200);
      Beep(185, 200);
      Beep(196, 200);
      Beep(185, 200);
      Beep(196, 200);
      Beep(207.65, 200);
      Beep(220, 200);
      Beep(233.08, 200);
      Beep(246.94, 200);
    }
    break;
  }
}
void initmusic() {
  HANDLE hThread;
  BOOL Active;
  DWORD ThreadId;
  hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadFunctionMusic,
                         (void *)&Active, 0, &ThreadId);
  if (hThread == NULL) {
    outtextxy(140, 135, "CreateThread");
  }
  while (Active == TRUE)
    ;
  CloseHandle(hThread);
}
void PaddleAndControls(Scene *Sc) {
  int maxx, maxy;
  maxx = 639;
  maxy = 479;
  UINT key_1 = MapVirtualKey(27, 1);
  UINT key_2 = MapVirtualKey(77, 1);
  UINT key_3 = MapVirtualKey(75, 1);
  if (GetAsyncKeyState(key_2)) {
    setcolor(BLACK);
    rectangle(Sc->paddleX - 40, maxy - 26, Sc->paddleX + 40, maxy - 13);
    setcolor(WHITE);
    Sc->paddleX += Sc->paddleXSpeed;
  }
  if (GetAsyncKeyState(key_3)) {
    setcolor(BLACK);
    rectangle(Sc->paddleX - 40, maxy - 26, Sc->paddleX + 40, maxy - 13);
    setcolor(WHITE);
    Sc->paddleX -= Sc->paddleXSpeed;
  }
  if (Sc->paddleX < 0)
    Sc->paddleX = 639;
  if (Sc->paddleX > 639)
    Sc->paddleX = 1;
  rectangle(Sc->paddleX - 40, maxy - 26, Sc->paddleX + 40, maxy - 12);
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  int maxx, maxy;
  maxx = 639;
  maxy = 479;
  time_t timestart;
  time_t current_time;
  char Time[100];
  //�����
  Scene Sc;
  Sc.width = 640;
  Sc.height = 480;
  initwindow(Sc.width, Sc.height, "Arkanoid v0.5");

  //��������� ���� � ������ �����������
  setusercharsize(20, 20, 20, 20);
  DrawMenu();
  initmusic();
  // initanimMenu();
  do {
    if (kbhit()) {
      int key = getch();
      if (key == 0)
        key = getch();
      switch (key) {
        // p
      case 80:
      case 112:
        cleardevice();
        if (!DrawLevelMenu(&Sc)) {
          cleardevice();
          DrawMenu();
          break;
        } else {
          // ���������� ���������� ������ � �����, ���� ��� ������ ���������
          // ����� ���
          cleardevice();
          //����� ��� �������
          timestart = time(NULL);
          if (timestart == ((time_t)-1)) {
            return EXIT_FAILURE;
          }
          while (Sc.lifes > 0) {
            rectangle(0, 0, maxx, maxy - 12);
            bricks(&Sc);
            //�������� ������������ c� �������� � �������� ������������ � �����
            Window(&Sc);
            //�������� ������������ � ������ � ��� �����������
            BlockHit(&Sc);
            //����������, ��������� � �������� ������������ � ������
            PaddleAndControls(&Sc);
            //�������� ������������
            PlatformCheck(&Sc);
            //������� ��������� � ��������� ����
            DrawBall(&Sc);
            //����
            sprintf(Sc.TableScore, "Score:%d", Sc.Score);
            outtextxy(530, 2, Sc.TableScore);
            //�����
            current_time = time(NULL);
            if (current_time == ((time_t)-1)) {
              (void)fprintf(stderr, "Failure to compute the current time.");
              return EXIT_FAILURE;
            }
            sprintf(Time, "Time: %0.0lf",
                    difftime(current_time, timestart) + Sc.timer);
            outtextxy(30, 2, Time);

            //���� ��������� �����
            int blocksleft = BlocksLeft(&Sc);
            if (blocksleft == 0) {
              Sc.timer += difftime(current_time, timestart);
              settextstyle(0, 0, 7);
              outtextxy(30, 200, "Level complete");
              settextstyle(0, 0, 1);
              // SaveResults();
              delay(1000);
              if (LoadLevel("./resources/level2", &Sc)) {
                cleardevice();
                continue;
              } else {
                Sc.Score = Sc.Score + (1000 - Sc.timer) * 100;
                SaveRecords(&Sc);
                break;
              }
            }

            //����������� �����
            if (kbhit()) {
              int key = getch();
              if (key == 27) {
                Sc.timer += difftime(current_time, timestart);
                Save(&Sc);
                Sc.lifes = 0;
              }
            }
            delay(5);
          }
          Sc.Score = Sc.Score + (1000 - Sc.timer) * 10;
          SaveRecords(&Sc);
          cleardevice();
          DrawMenu();
        }
        break;

        // i
      case 73:
      case 'i':
        cleardevice();
        outtextxy(
            30, 10,
            "��������� - �����-����� ��������� ��� ������������ ������� DOS");
        outtextxy(240, 30, "����������: <- ->");
        outtextxy(30, 50,
                  "��� �������������� ������� �������� ���� �� ������ � ������ "
                  "level");
        break;
        // r
      case 82:
      case 114:
        cleardevice(); // records
        LoadRecords("./resources/records");
        break;
        // Esc
      case 27:
        cleardevice(); // Escape
        DrawMenu();
        break;

        // e
      case 69:
      case 101:
        exit(1); // Exit
      }
    }
  } while (true);
}