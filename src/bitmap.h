#ifndef BITMAP_H
#define BITMAP_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <stdio.h>

typedef struct {
  // Character sprite attributes
  int frame_w, frame_h, rows, cols;
  float px, py;

  // // Rectangle entity attributes
  // int rw, rh, rx, ry;
  //
  // // Circle entity attributes
  // float cx, cy, ray;

  ALLEGRO_BITMAP *sheet, *protag;
} SpriteSheetInfo;

extern SpriteSheetInfo spr;
extern SpriteSheetInfo ent;

// ALLEGRO_BITMAP *BitmapGrab(ALLEGRO_BITMAP *sheet, int x, int y, int w, int h);
void InitBitmap(void);
void BitmapExplode(void);
void DrawProtag(void);
void SpriteMovement(bool keys[], float *px, float *py, float sp, int *fx, int *fy, float frames);
#endif
