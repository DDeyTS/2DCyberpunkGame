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

extern bool show_intro;
extern int speaker;
extern int selected_topic;
extern tmx_map *map;
extern ALLEGRO_BITMAP *mouse_bmp;

#endif
