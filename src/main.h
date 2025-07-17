#ifndef MAIN_H
#define MAIN_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/keyboard.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <tmx.h>

#define DISPW 640
#define DISPH 400

typedef struct {
  int frame_w, frame_h, rows, cols;
  float px, py;

  int rw, rh, rx, ry;

  float cx, cy, ray;
  ALLEGRO_BITMAP *sheet, *bandit;
} SpriteSheetInfo;

extern SpriteSheetInfo spr;
extern SpriteSheetInfo ent;
extern tmx_map *map;

// collision.c
bool RectSqColl(float px, float py, int pw, int ph,
                float wall_x1, float wall_y1,
                int wall_x2, int wall_y2);
bool CircleColl(float cx1, float cy1, float r1,
                float cx2, float cy2, float r2);

// bitmap.c
ALLEGRO_BITMAP *BitmapGrab(int x, int y, int w, int h);
void BitmapInit(void);
void BitmapExplode(void);

void BanditDraw(void);
void BanditDirection(bool keys[], int *fx, int *fy);
void BanditMove(bool keys[], float *px, float *py, float sp);

#endif

