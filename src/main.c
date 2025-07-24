//**************************************************************************
//**
//** File: main.c (CyberSP Project)
//** Purpose: Main loop
//** Last Update: 23-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "main.h"
#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"
#include "tile_render.h"

tmx_map *map = NULL;
ALLEGRO_FONT *font;

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

  // NPC dialogue
  bool dlg_open = true;
  npc = CreateNpc("Clowngirl", 3);
  npc->portrait_id = al_load_bitmap("portraits/clowngirl_portrait.png");
  // if (!clowngirl->portrait_id) {
  //   fprintf(stderr, "Erro: não foi possível carregar retrato\n");
  //   exit(1);
  // }
  FillTopic(npc, 0, "Who are you?",
            "Hi, little man. I'm Harley! The clown icon inside some host "
            "around cyberspace... I guess.");
  LoadDlg(npc, "Who are you?");
  FillTopic(npc, 1, "Escape",
            "Escape? Just get out through the way you did before, isn't easy?");
  LoadDlg(npc, "Escape");
  FillTopic(npc, 2, "Clown",
            "Yes, I am... Why? There aren't clowns in your world?");
  LoadDlg(npc, "Clown");

  int selected_topic = 0;
  bool choosing_topic = true;

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

  // defines callbacks for libTMX
  tmx_img_load_func = AllegTexLoader;
  tmx_img_free_func = AllegTexFree;
  map = tmx_load("tiles/tile_garbage/debugavenue.tmx");
  if (!map) {
    tmx_perror("Cannot load map");
    return 1;
  }

  while (running) {
    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    // Keyboard boolean
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

    // Dialog Trigger Button
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      if (keys[ALLEGRO_KEY_SPACE]) {
        dlg_open = true;
        choosing_topic = true;
      }
    }

    // Topic Selector
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      if (dlg_open && choosing_topic) {
        if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
          selected_topic++;
          if (selected_topic >= npc->num_topic)
            selected_topic = 0;
        }
        if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
          selected_topic--;
          if (selected_topic < 0)
            selected_topic = npc->num_topic - 1;
        }
        if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
          choosing_topic = false;
        }
      } else if (!choosing_topic && ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
        dlg_open = false; // close dialogue if ENTER is pressed again
      }
    }

    BanditMove(keys, &spr.px, &spr.py, sp);
    BanditDirection(keys, &spr.frame_w, &spr.frame_h);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    // if (map != NULL) {
    //   printf("we fucking good\n");
    // } else {
    //   printf("we not fuckin good\n");
    // }

    if (redraw && al_is_event_queue_empty(queue)) {
      // al_set_target_backbuffer(disp);
      al_clear_to_color(al_map_rgb(0, 0, 0));
      RenderMap(map);

      // Collisions drawings
      al_draw_rectangle(ent.rx, ent.ry, ent.rw + ent.rx, ent.rh + ent.ry,
                        al_map_rgb(255, 0, 0), 5.0);
      al_draw_circle(ent.cx, ent.cy, ent.ray, al_map_rgb(0, 255, 0), 5);

      BanditDraw();

      if (dlg_open) {
        if (choosing_topic) {
          DrawTopicMenu(npc, selected_topic);
        } else {
          const char *topic = npc->topics[selected_topic].topic;
          LoadDlg(npc, topic);
        }
      }

      al_flip_display();
      redraw = false;
    }
  }

  al_destroy_display(disp);

  // Dialog Box
  ExplodeDlgBox(npc->portrait_id);
  for (int i = 0; i < npc->num_topic; i++) {
    free((char *)npc->topics[i].topic);
    free((char *)npc->topics[i].text);
  }
  free(npc->topics);
  free(npc);
  
  // Map
  tmx_map_free(map);

  al_destroy_font(font);

  // Sprites
  BitmapExplode();

  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  return 0;
}
