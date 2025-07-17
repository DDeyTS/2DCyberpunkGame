//**************************************************************************
//**
//** File: main.c (CyberSP Project)
//** Purpose: Main loop
//** Last Update: 17-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "main.h"
#include "tile_render.h"

SpriteSheetInfo spr;
SpriteSheetInfo ent;

tmx_map *map = NULL;

/*
      First feature: finding out an adequated way
      to make an isometric movement.
          Update: the feature successful ran.

  (X) Second feature: loading sprite sheets
      from .txt files.
          Update: it went wrong due to complications
          to read each line which set the specific
          sprite in the sprite sheet.

      Third feature: collision. I'll make two
      rectangles (invisible if possible) to draw
      the collision size.
          Update: the collision reader worked
          perfectly, but the wall (the next feature)
          is putting my brains in trouble.
          Update 2: the wall collision worked exactly
          like I'd thought... of course, after a lot of
          hours of code!
          Update 2: After struggling to reach a more
          complex collision system, I've ended up in
          a new perspective to this project: it's all
          detailed inside "collision.c".
      Fourth feature: making a cool and practical tilemap.
        Update: the map for test was 100% loaded and flipped.
        Now I'll have to put the objects on.
        Update 2: map render is working well, though the map
        objects aren't flipping appropriately. Maybe I'll need
        to remake another tilemap to solve that.

      (X) = Discarted.

      First Feature Update - Beginning of July 2025.
      Last Feature Update - July 16, 2025.
*/

//==========================================================================
//
//    game_main
//
//==========================================================================
int main() {
  if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon() ||
      !al_install_keyboard()) {
    fprintf(stderr, "Falha ao inicializar Allegro\n");
    return 1;
  }
  BitmapInit();

  ALLEGRO_DISPLAY *disp = al_create_display(DISPW, DISPH);
  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_start_timer(timer);

  // Bandit Position
  spr.px = 320;
  spr.py = 200;
  // Frames (and the initial frame when the program begins)
  spr.frame_w = 32;
  spr.frame_h = 0;
  // Movement Speed
  int sp = 3;

  // Rectangle Position
  ent.rx = 300;
  ent.ry = 100;
  // Edges
  ent.rw = 200;
  ent.rh = 50;

  // Circle Position
  ent.cx = 100;
  ent.cy = 150;
  ent.ray = 80;

  // Loop and Bitmap Redrawing
  bool running = 1;
  bool redraw = true;

  bool keys[ALLEGRO_KEY_MAX];
  memset(keys, 0, sizeof(keys)); // reset array

  while (running) {
    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      keys[ev.keyboard.keycode] = true;
    } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
      keys[ev.keyboard.keycode] = false;
    }

    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
      running = 0;

    if (ev.type == ALLEGRO_EVENT_TIMER) {
      redraw = true;
    }

    // define callbacks para libTMX
    tmx_img_load_func = AllegTexLoader;
    tmx_img_free_func = AllegTexFree;

    map = tmx_load("tiles/closedstreet_map.tmx");
    if (!map) {
      tmx_perror("Cannot load map");
      return 1;
    }

    BanditMove(keys, &spr.px, &spr.py, sp);
    BanditDirection(keys, &spr.frame_w, &spr.frame_h);

    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    if (redraw && al_is_event_queue_empty(queue)) {
      al_set_target_backbuffer(disp);
      al_clear_to_color(al_map_rgb(0, 0, 0));
      RenderMap(map);
      al_draw_rectangle(ent.rx, ent.ry, ent.rw + ent.rx, ent.rh + ent.ry,
                        al_map_rgb(255, 0, 0), 5.0);
      al_draw_circle(ent.cx, ent.cy, ent.ray, al_map_rgb(0, 255, 0), 5);
      BanditDraw();
      al_flip_display();
      redraw = false;
    }
  }

  al_destroy_display(disp);
  tmx_map_free(map);
  BitmapExplode();
  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  return 0;
}
