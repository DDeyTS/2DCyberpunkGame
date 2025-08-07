//**************************************************************************
//**
//** File: bitmap.c (CyberSP Project)
//** Purpose: Sprite handling (animation, movement)
//** Last Update: 07-08-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"
#include "main.h"

static int reset_frame = 0;

SpriteSheetInfo spr;
SpriteSheetInfo ent;

//==========================================================================
//
//    InitBitmap
//
//    Loads the sprite sheet and each sprite.
//
//==========================================================================

void InitBitmap() {
  spr.protag = al_load_bitmap("sprites/regis_spritesheet.png");
  if (!spr.protag) {
    perror("Fail to load spr.protag!\n");
    exit(1);
  }

  chatbox = al_load_bitmap("sprites/largechatbox_sprite.png");
  if (!chatbox) {
    perror("Fail to load chatbox!\n");
    exit(1);
  }

  protagonist = al_load_bitmap("portraits/regis_portrait.png");
  if (!protagonist) {
    perror("Fail to load protagonist's face!\n");
    exit(1);
  }

  mouse_bmp = al_load_bitmap("sprites/cursordebugg_sprite.png");
  if (!mouse_bmp) {
    perror("Fail to load mouse cursor!\n");
    exit(1);
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
  al_destroy_bitmap(spr.protag);
  al_destroy_bitmap(chatbox);
  al_destroy_bitmap(protagonist);
  al_destroy_bitmap(mouse_bmp);
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
  al_draw_scaled_bitmap(spr.protag, spr.frame_w, spr.frame_h, 16, 24, spr.px,
                        spr.py, 32, 48, 0);
}

//==========================================================================
//
//    SpriteMovement
//
//    Moves the sprite toward the direction he's facing.
//
//==========================================================================

void SpriteMovement(bool keys[], float *px, float *py, float sp, int *fx,
                    int *fy, float frames) {
  int dx = 0, dy = 0;
  spr.px = *px;
  spr.py = *py;
  spr.cols = 16;
  spr.rows = 24;
  float fq = (spr.cols * frames) + spr.cols; // frame queue

  // Diagonal Movement
  if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_D]) {
    // Up-right
    *fx = fq, *fy = spr.rows * 5;
    reset_frame = *fy;
  } else if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_A]) {
    // Up-left
    *fx = fq, *fy = spr.rows * 6;
    reset_frame = *fy;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_D]) {
    // Down-right
    *fx = fq, *fy = spr.rows * 2;
    reset_frame = *fy;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_A]) {
    // Down-left
    *fx = fq, *fy = spr.rows;
    reset_frame = *fy;

    // Straight Movement
  } else if (keys[ALLEGRO_KEY_D]) {
    *fx = fq, *fy = spr.rows * 4;
    reset_frame = *fy;
  } else if (keys[ALLEGRO_KEY_A]) {
    *fx = fq, *fy = spr.rows * 3;
    reset_frame = *fy;
  } else if (keys[ALLEGRO_KEY_S]) {
    *fx = fq, *fy = 0;
    reset_frame = *fy;
  } else if (keys[ALLEGRO_KEY_W]) {
    *fx = fq, *fy = spr.rows * 7;
    reset_frame = *fy;
  } else {
    *fx = 0;
    *fy = reset_frame;
  }

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
}
