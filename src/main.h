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

// extern bool show_intro;
// extern int speaker;
// extern int selected_topic;
extern bool keys[ALLEGRO_KEY_MAX], mouse[MOUSE_MAX + 1];
extern tmx_map *map;
extern ALLEGRO_BITMAP *mouse_bmp, *target_bmp, *mouse_click_bmp;

#endif
