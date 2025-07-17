#ifndef BITMAP_H
#define BITMAP_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <math.h>

typedef struct {
  // Character sprite attributes
  int frame_w, frame_h, rows, cols;
  float px, py;
  // Rectangle entity attributes
  int rw, rh, rx, ry;
  // Circle entity attributes
  float cx, cy, ray;
  ALLEGRO_BITMAP *sheet, *bandit;
} SpriteSheetInfo;

extern SpriteSheetInfo spr;
extern SpriteSheetInfo ent;

ALLEGRO_BITMAP *BitmapGrab(int x, int y, int w, int h);
void BitmapInit(void);
void BitmapExplode(void);
void BanditDraw(void);

void BanditDirection(bool keys[], int *fx, int *fy);
void BanditMove(bool keys[], float *px, float *py, float sp);

#endif
