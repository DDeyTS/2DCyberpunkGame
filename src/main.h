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
#define MOUSE_MAX 5

typedef struct {
    ALLEGRO_BITMAP *mouse_bmp, *target_bmp, *click_bmp, *eye_bmp;
    ALLEGRO_MOUSE_CURSOR *aim, *clicking, *normal, *view;
} Mousecursors;

extern Mousecursors cursors;

enum CursorType {
    CURSOR_NORMAL,
    // CURSOR_CLICK,
    CURSOR_TARGET,
    CURSOR_EYE,
};

extern bool keys[ALLEGRO_KEY_MAX], mouse[MOUSE_MAX + 1];
extern int mouse_x, mouse_y;
extern tmx_map* map;

#endif
