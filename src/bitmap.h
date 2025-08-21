#ifndef BITMAP_H
#define BITMAP_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include <math.h>
#include <stdio.h>

typedef enum {
    NONE_ENTITY = -1,
    ENTITY_GANGMEMBER,
    NUM_ENTITY,
} EntitiesID;

typedef struct {
    int frame_w, frame_h, rows, cols;
    float px, py;
    ALLEGRO_BITMAP *protag;
} SpriteSheetInfo;

extern SpriteSheetInfo spr, ent[];

void InitBitmap(void);
void BitmapExplode(void);
void DrawProtag(void);
void SpriteMovement(bool keys[], float *px, float *py, float sp, int *fx, int *fy,
                    float frames);
void SpriteAimAtCursor(float px, float py, int *fy);
#endif
