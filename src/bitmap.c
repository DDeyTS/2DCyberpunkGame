//**************************************************************************
//**
//** File: bitmap.c (CyberSP Project)
//** Purpose: Sprite handling (animation, movement)
//**
//** Last Update: 23-08-2025 15:09
//** Author: DDeyTS
//**
//**************************************************************************

#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"
#include "main.h"

// EXTERNAL DATA DECLARATIONS ///////////////////////////////////////////////

SpriteSheetInfo spr, ent[NUM_ENTITY];

// PRIVATE DATA DEFINITIONS /////////////////////////////////////////////////

static int reset_frame = 0;

//==========================================================================
//
//    InitBitmap
//
//    Argument: void
//    Return:   void
//
//    Note: this function loads all the sprites.
//
//==========================================================================

void InitBitmap()
{
    spr.protag = al_load_bitmap("sprites/regis_spritesheet.png");
    if (!spr.protag) {
        perror("Fail to load spr.protag!\n");
        exit(1);
    }
    protagonist = al_load_bitmap("portraits/regis_face.png");
    if (!protagonist) {
        perror("Fail to load protagonist's face!\n");
        exit(1);
    }

    chatbox = al_load_bitmap("sprites/chatbox_sprite.png");
    if (!chatbox) {
        perror("Fail to load chatbox!\n");
        exit(1);
    }

    cursors.mouse_bmp = al_load_bitmap("sprites/handcursor_sprite.png");
    if (!cursors.mouse_bmp) {
        perror("Fail to load mouse cursor!\n");
        exit(1);
    }
    cursors.click_bmp = al_load_bitmap("sprites/handcursor_click_sprite.png");
    if (!cursors.click_bmp) {
        perror("Fail to load mouse cursor clicking on frame!\n");
        exit(1);
    }
    cursors.target_bmp = al_load_bitmap("sprites/target_sprite.png");
    if (!cursors.target_bmp) {
        perror("Fail to load target cursor!\n");
        exit(1);
    }
    cursors.eye_bmp = al_load_bitmap("sprites/eye_cursor.png");
    if (!cursors.eye_bmp) {
        perror("Fail to load eye cursor\n");
        exit(1);
    }

    chatbox_light = al_load_bitmap("sprites/signal_light_chatbox_spritesheet.png");
    if (!chatbox_light) {
        perror("Fail to load little computer light!\n");
        exit(1);
    }
}

//==========================================================================
//
//    BitmapExplode
//
//    Argument: void
//    Return:   void
//
//    NOTE: this function destroys every sprite when the game is closed!!!
//
//==========================================================================

void BitmapExplode()
{
    al_destroy_bitmap(spr.protag);
    al_destroy_bitmap(chatbox);
    al_destroy_bitmap(chatbox_light);
    al_destroy_bitmap(protagonist);
    if (cursors.mouse_bmp) al_destroy_bitmap(cursors.mouse_bmp);
    if (cursors.click_bmp) al_destroy_bitmap(cursors.click_bmp);
    if (cursors.target_bmp) al_destroy_bitmap(cursors.target_bmp);
    if (cursors.eye_bmp) al_destroy_bitmap(cursors.eye_bmp);
}

//==========================================================================
//
//    DrawProtag
//
//    Argument: void
//    Return:   void
//
//    NOTE: Draws the protagonist's sprite.
//    NOTE: if the sprite is scaled, take care of remembering to adapt
//    the code for its new size.
//
//==========================================================================

void DrawProtag()
{
    al_draw_scaled_bitmap(spr.protag, spr.frame_w, spr.frame_h, 16, 24, spr.px,
                          spr.py, 32, 48, 0);
}

//==========================================================================
//
//    SpriteMovement
//
//    Argument: bool keys[]        - read which keys are pressed
//              float *px          - entity's current X position
//              float *py          - entity's current Y position
//              float sp           - movement speed
//              int *fx            - sprite sheet's row to animate
//              int *fy            - sprite sheet's column to animate
//              float frames       - number of frames per second
//    Return:   void
//
//    TODO: try to use it with NPCs, too.
//    TODO: aligning the movement when another key is pressed at the same time.
//
//==========================================================================

void SpriteMovement(bool keys[], float *px, float *py, float sp, int *fx, int *fy,
                    float frames)
{
    int dx = 0, dy = 0;
    int cols = 16;
    int rows = 24;
    float fq = (cols * frames) + cols; // frame queue

    // Diagonal Movement
    if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_D]) {
        // Up-right
        *fx = fq, *fy = rows * 5;
        reset_frame = *fy;
    } else if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_A]) {
        // Up-left
        *fx = fq, *fy = rows * 6;
        reset_frame = *fy;
    } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_D]) {
        // Down-right
        *fx = fq, *fy = rows * 2;
        reset_frame = *fy;
    } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_A]) {
        // Down-left
        *fx = fq, *fy = rows;
        reset_frame = *fy;

        // Straight Movement
    } else if (keys[ALLEGRO_KEY_D]) {
        *fx = fq, *fy = rows * 4;
        reset_frame = *fy;
    } else if (keys[ALLEGRO_KEY_A]) {
        *fx = fq, *fy = rows * 3;
        reset_frame = *fy;
    } else if (keys[ALLEGRO_KEY_S]) {
        *fx = fq, *fy = 0;
        reset_frame = *fy;
    } else if (keys[ALLEGRO_KEY_W]) {
        *fx = fq, *fy = (rows * 7) + 1;
        reset_frame = *fy;
    } else {
        *fx = 0;
        *fy = reset_frame;
    }

    // Pressed directions
    if (keys[ALLEGRO_KEY_D]) dx += 1;
    if (keys[ALLEGRO_KEY_A]) dx -= 1;
    if (keys[ALLEGRO_KEY_S]) dy += 1;
    if (keys[ALLEGRO_KEY_W]) dy -= 1;

    // Apply speed
    float mov_x = dx * sp, mov_y = dy * sp;

    // Adjust speed
    if (dx != 0 && dy != 0) {
        float adj = 0.707f; // aka 1 / sqrt(2)
        mov_x *= adj;
        mov_y *= adj;
    }

    *px += mov_x;
    *py += mov_y;
}

//==========================================================================
//
//    SpriteAimAtCursor
//
//    Argument: float px        - current sprite's X position
//              float py        - current sprite's Y position
//              float *fy       - sprite sheet's column to animate
//    Return:   void
//
//==========================================================================

void SpriteAimAtCursor(float px, float py, int *fy)
{
    float t_dx    = mouse_x - (spr.px + 16); // sprite center
    float t_dy    = mouse_y - (spr.py + 24); // same above
    float t_angle = atan2(t_dy, t_dx);       // radianus (-PI to +PI)

    int dir;
    if (t_angle >= -ALLEGRO_PI / 8 && t_angle < ALLEGRO_PI / 8)
        dir = 4; // right
    else if (t_angle >= ALLEGRO_PI / 8 && t_angle < 3 * ALLEGRO_PI / 8)
        dir = 2; // down-right
    else if (t_angle >= 3 * ALLEGRO_PI / 8 && t_angle < 5 * ALLEGRO_PI / 8)
        dir = 0; // down
    else if (t_angle >= 5 * ALLEGRO_PI / 8 && t_angle < 7 * ALLEGRO_PI / 8)
        dir = 1; // down-left
    else if (t_angle >= 7 * ALLEGRO_PI / 8 || t_angle < -7 * ALLEGRO_PI / 8)
        dir = 3; // left
    else if (t_angle >= -7 * ALLEGRO_PI / 8 && t_angle < -5 * ALLEGRO_PI / 8)
        dir = 6; // up-left
    else if (t_angle >= -5 * ALLEGRO_PI / 8 && t_angle < -3 * ALLEGRO_PI / 8)
        dir = 7; // up
    else
        dir = 5; // up-right

    *fy = dir * 24;
}
