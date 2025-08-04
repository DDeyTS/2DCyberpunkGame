//**************************************************************************
//**
//** File: bitmap.c (CyberSP Project)
//** Purpose: Sprite handling (animation, movement)
//** Last Update: 03-08-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"

static int reset_frame = 0;

SpriteSheetInfo spr;
SpriteSheetInfo ent;

//==========================================================================
//
//    BitmapGrab
//
//    Prepares the sprites on the sheet.
//
//==========================================================================

ALLEGRO_BITMAP *BitmapGrab(ALLEGRO_BITMAP *sheet, int x, int y, int w, int h) {
  ALLEGRO_BITMAP *sprite = al_create_sub_bitmap(sheet, x, y, w, h);
  if (!sprite) {
    perror("Fail to load sub-bitmap!\n");
    exit(1);
  }
  return sprite;
}

//==========================================================================
//
//    InitBitmap
//
//    Loads the sprite sheet and each sprite.
//
//==========================================================================

void InitBitmap() {
  spr.sheet = al_load_bitmap("sprites/prota_spritesheet.png");
  if (!spr.sheet) {
    perror("Fail to load spr.bandit!\n");
    exit(1);
  }

  spr.protag = BitmapGrab(spr.sheet, 0, 0, 16, 24);
  if (!spr.protag) {
    perror("BitmapGrab returned NULL\n");
    exit(1);
  }

  chatbox = al_load_bitmap("sprites/largechatbox_sprite.png");
  if (!chatbox) {
    perror("Fail to load largechatbox_sprite.png\n");
    exit(1);
  }

  protagonist = al_load_bitmap("portraits/bandit_portrait.png");
  if (!protagonist) {
    perror("Fail to load bandit_portrai.png");
  }
}

//==========================================================================
//
//    BitmapExplode
//
//    Destroys every made sprite when the game is closed!!!
//
//==========================================================================

void BitmapExplode() {
  al_destroy_bitmap(spr.sheet);
  al_destroy_bitmap(spr.protag);
  al_destroy_bitmap(chatbox);
  al_destroy_bitmap(protagonist);
}

//==========================================================================
//
//    DrawProtag
//
//    Draws the protagonist's sprite.
//
//    WARNING: If the sprite is scaled, take care of remembering to adapt
//    the code for its new size.
//
//==========================================================================

void DrawProtag() {
  al_draw_scaled_bitmap(spr.sheet, spr.frame_w, spr.frame_h, 16, 24, spr.px,
                        spr.py, 32, 48, 0);
}

//==========================================================================
//
//    ProtagDirection
//
//    Moves the sprite toward the direction he's facing.
//
//==========================================================================

void ProtagDirection(bool keys[], int *fx, int *fy, float frames) {
  spr.cols = 16;
  spr.rows = 24;

  // Diagonal Movement
  if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_D]) {
    // Up-right
    *fx = 0, *fy = spr.rows * 5;
  } else if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_A]) {
    // Up-left
    *fx = 0, *fy = spr.rows * 6;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_D]) {
    // Down-right
    *fx = 0, *fy = spr.rows * 2;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_A]) {
    // Down-left
    *fx = 0, *fy = spr.rows;
    // Straight Movement
  } else if (keys[ALLEGRO_KEY_D]) {
    *fx = 0, *fy = spr.rows * 4;
  } else if (keys[ALLEGRO_KEY_A]) {
    *fx = 0, *fy = spr.rows * 3;
  } else if (keys[ALLEGRO_KEY_S]) {
    *fx = 16 * frames, *fy = 0;
    reset_frame = *fy;
  } else if (keys[ALLEGRO_KEY_W]) {
    *fx = 0, *fy = spr.rows * 7;
  } else {
    *fx = 0;
    *fy = reset_frame;
  }
}

//==========================================================================
//
//    ProtagMove
//
//    Straight and diagonal movement logic and collision trigger.
//
//    WARNING: This has not link with .tmx stuff.
//
//==========================================================================

void ProtagMove(bool keys[], float *px, float *py, float sp) {
  int dx = 0, dy = 0;
  spr.px = *px;
  spr.py = *py;

  // Pressed directions
  if (keys[ALLEGRO_KEY_D])
    dx += 1;
  if (keys[ALLEGRO_KEY_A])
    dx -= 1;
  if (keys[ALLEGRO_KEY_S])
    dy += 1;
  if (keys[ALLEGRO_KEY_W])
    dy -= 1;

  // Movement Values
  float mov_x = dx * sp, mov_y = dy * sp;
  // Adjust the speed
  if (dx != 0 && dy != 0) {
    float adj = 1 / sqrt(2); // = 0.707f
    mov_x *= adj;
    mov_y *= adj;
  }
  spr.px = *px + mov_x;
  spr.py = *py + mov_y;

  // // Rectangle Collison
  // SpriteSheetInfo next = spr;
  // next.px = *px + mov_x;
  // next.py = *py + mov_y;
  // bool rectangle_x = RectSqColl(next.px, *py, 32, 48, 300, 100, 200, 50);
  // bool rectangle_y = RectSqColl(*px, next.py, 32, 48, 300, 100, 200, 50);
  //
  // // Circle Collison
  // ent.cx = 100;
  // ent.cy = 150;
  // ent.ray = 80;
  // float spr_r = 16;
  // float circle_x = next.px + 32 / 2.0;
  // float circle_y = next.py + 48 / 2.0;
  // bool circle = CircleColl(circle_x, circle_y, spr_r, ent.cx, ent.cy,
  // ent.ray);
  //
  // // Collision Triggers
  // if (!rectangle_x && !circle) {
  //   *px = next.px;
  // }
  // if (!rectangle_y && !circle) {
  //   *py = next.py;
  // }
  // spr.px = *px;
  // spr.py = *py;
}
