//**************************************************************************
//**
//** File: main.c (CyberSP Project)
//** Purpose: Main loop
//** Last Update: 19-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "main.h"
#include "bitmap.h"
#include "collision.h"
#include "dialogue_sys.h"
#include "tile_render.h"
#include <string.h>

tmx_map *map = NULL;
ALLEGRO_FONT *font;

/*
      First feature: finding out an adequated way
      to make an isometric movement.
          Update: the feature successful ran.

      Second feature: loading sprite sheets
      from .txt files. (Discarted.)
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
      Fifth feature: a dialogue box.
        Update: it ran stupidly well!
        Update 2: now the dialogue box disappears when the text
        is over.

      First Feature Update - Beginning of July 2025.
      Last Feature Update - July 19, 2025.
*/

//==========================================================================
//
//    game_main
//
//==========================================================================

int main() {
  if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon() ||
      !al_install_keyboard() || !al_init_font_addon() || !al_init_ttf_addon()) {
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

  // NPC portrait
  ALLEGRO_BITMAP *clowngirl = al_load_bitmap("clowngirl_portrait.png");
  // NPC dialogue
  clown.name = "Harley the Funny";
  clown.num_dlg = 4;
  clown.dialog = malloc(sizeof(char *) * clown.num_dlg);
  clown.dialog[0] = strdup("Why do ya still looking at me, little man?");
  clown.dialog[1] = strdup("Fuck you, bro.");
  clown.dialog[2] =
      strdup("After that, I'll not tell ya where's the exit, freaky! You're "
             "lucky cuz my brothers aren't here right now. Otherwise they'll "
             "fuck you up!");
  clown.dialog[3] = strdup("Well... goodbye, little void's man.");

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

    // Keyboard boolean
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      keys[ev.keyboard.keycode] = true;
    } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
      keys[ev.keyboard.keycode] = false;
    }

    // Dialogue skipper for debug reasons
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      if (keys[ALLEGRO_KEY_ENTER]) {
        if (clown.current_dlg < clown.num_dlg - 1) {
          clown.current_dlg++;
        } else {
          clown.current_dlg = clown.num_dlg;
        }
      }
    }

    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
      running = 0;

    if (ev.type == ALLEGRO_EVENT_TIMER) {
      redraw = true;
    }

    // defines callbacks for libTMX
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

      // Collisions drawings
      al_draw_rectangle(ent.rx, ent.ry, ent.rw + ent.rx, ent.rh + ent.ry,
                        al_map_rgb(255, 0, 0), 5.0);
      al_draw_circle(ent.cx, ent.cy, ent.ray, al_map_rgb(0, 255, 0), 5);

      BanditDraw();
      if (clown.current_dlg != clown.num_dlg) {
        DlgBox(clowngirl, clown.name, clown.dialog[clown.current_dlg]);
      }

      al_flip_display();
      redraw = false;
    }
  }

  al_destroy_display(disp);
  // free(clown.dialog);
  for (int i = 0; i < clown.num_dlg; i++) {
    free(clown.dialog[i]);
  }
  free(npc.dialog);
  tmx_map_free(map);
  al_destroy_font(font);
  BitmapExplode();
  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  return 0;
}
