//**************************************************************************
//**
//** File: bitmap.c (CyberSP Project)
//** Purpose: Sprite handling (animation, movement)
//** Last Update: 30-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"

SpriteSheetInfo spr;
SpriteSheetInfo ent;

//==========================================================================
//
//    BitmapGrab
//
//    Prepares all the bitmaps in the game.
//
//==========================================================================

ALLEGRO_BITMAP *BitmapGrab(int x, int y, int w, int h) {
  ALLEGRO_BITMAP *sprite = al_create_sub_bitmap(spr.sheet, x, y, w, h);
  if (!sprite) {
    printf("Error: fail to load sub-bitmap!\n");
    exit(1);
  }
  return sprite;
}

//==========================================================================
//
//    BitmapInit
//
//    Loads the sprite sheet and each sprite.
//
//==========================================================================

void InitBitmap() {
  spr.sheet = al_load_bitmap("sprites/bandit_still_sheet.png");
  if (!spr.sheet) {
    perror("Fail to load spr.bandit!\n");
    exit(1);
  }

  spr.bandit = BitmapGrab(0, 0, 16, 24);
  if (!spr.bandit) {
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
//    Destroys everything made when the game is closed!!!
//
//==========================================================================

void BitmapExplode() {
  al_destroy_bitmap(spr.sheet);
  al_destroy_bitmap(spr.bandit);
  al_destroy_bitmap(chatbox);
  al_destroy_bitmap(protagonist);
}

//==========================================================================
//
//    BanditDraw
//
//    Draws the sprite tester.
//
//    WARNING: if the sprite is scaled, take care of remembering to adapt
//    the code for its new size.
//
//==========================================================================

void BanditDraw() {
  al_draw_scaled_bitmap(spr.bandit, spr.frame_w, spr.frame_h, 16, 24, spr.px,
                        spr.py, 32, 48, 0);
}

//==========================================================================
//
//    BanditDirection
//
//    Moves the sprite tester toward the he's facing.
//
//==========================================================================

void BanditDirection(bool keys[], int *fx, int *fy) {
  int cols = spr.cols = 16;
  int rows = spr.rows = 24;
  spr.frame_w = *fx;
  spr.frame_h = *fy;

  // Diagonal Movement
  if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_D]) {
    // Up-right
    *fx = cols, *fy = rows * 2;
  } else if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_A]) {
    // Up-left
    *fx = cols - 16, *fy = rows * 2;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_D]) {
    // Down-right
    *fx = cols, *fy = rows;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_A]) {
    // Down-left
    *fx = cols, *fy = cols - 16;
    // Straight Movement
  } else if (keys[ALLEGRO_KEY_D]) {
    *fx = cols - 16, *fy = rows;
  } else if (keys[ALLEGRO_KEY_A]) {
    *fx = cols - 16, *fy = rows - 24;
  } else if (keys[ALLEGRO_KEY_S]) {
    *fx = cols * 2, *fy = rows - 24;
  } else if (keys[ALLEGRO_KEY_W]) {
    *fx = cols * 2, *fy = rows;
  }
}

//==========================================================================
//
//    BanditMove
//
//    Straight and diagonal movement logic and collision trigger.
//
//    WARNING: this has not link with .tmx stuff.
//
//==========================================================================

void BanditMove(bool keys[], float *px, float *py, float sp) {
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
  // bool circle = CircleColl(circle_x, circle_y, spr_r, ent.cx, ent.cy, ent.ray);
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
