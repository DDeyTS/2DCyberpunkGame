//**************************************************************************
//**
//** File: main.c (CyberSP Project)
//** Purpose: Main game stuff
//** Last Update: 07-08-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "main.h"
#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"
#include "textdat.h"
#include "tile_render.h"

bool show_intro = true;
int speaker = 0;
int selected_topic = 0;

tmx_map *map = NULL;
ALLEGRO_DISPLAY *disp;
ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_TIMER *timer;
ALLEGRO_BITMAP *mouse_bmp;
ALLEGRO_MOUSE_CURSOR *cursor;

//==========================================================================
//
//    Main Game
//
//==========================================================================

int main() {
    //======================
    //
    //    Initializers
    //
    //======================

    if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon() ||
        !al_install_keyboard() || !al_init_font_addon() ||
        !al_init_ttf_addon() || !al_install_mouse()) {
        perror("Fail to initialize Allegro\n");
        return 1;
    }
    InitStdFont();
    InitBitmap();

    disp = al_create_display(DISPW, DISPH);
    queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 30.0);
    cursor = al_create_mouse_cursor(mouse_bmp, 0, 0);
    al_set_mouse_cursor(disp, cursor);

    //======================
    //
    //    Event Queue
    //
    //======================

    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    //======================
    //
    //    Dialogue Storage
    //
    //======================

    bool dlg_open = false;
    NpcLoader(npc);
    int active_topic = -1;
    bool choosing_topic = true;

    //======================
    //
    //    Player Movement
    //
    //======================

    // Bandit Position
    spr.px = 320;
    spr.py = 200;
    // Frames (and the initial frame when the program begins)
    spr.frame_w = 0;
    spr.frame_h = 0;
    // Movement Speed
    float sp = 3.5;
    // Sprite Frames
    float frames = 0.f;

    //======================
    //
    //    Main Loop
    //
    //======================

    bool running = 1;
    bool redraw = true;

    bool keys[ALLEGRO_KEY_MAX];
    memset(keys, 0, sizeof(keys)); // reset array

    // defines callbacks for libTMX
    tmx_img_load_func = AllegTexLoader;
    tmx_img_free_func = AllegTexFree;
    map = tmx_load("tiles/debugavenue.tmx");
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
            frames += 0.3f;   // frame speed
            if (frames > 4) { // reset frame queue
                frames -= 4;
            }
            SpriteMovement(keys, &spr.px, &spr.py, sp, &spr.frame_w,
                           &spr.frame_h, (int)frames);

            redraw = true;
        }

        // Dialog Trigger Button (for debugging)
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (keys[ALLEGRO_KEY_SPACE]) {
                dlg_open = true;
                choosing_topic = true;
                show_intro = true;
            }
            // Closes the Dialogue Box window
            if (keys[ALLEGRO_KEY_ESCAPE]) {
                dlg_open = false;
                active_topic = -1;
                show_intro = false;
            }
        }

        // NPC Changer (for debugging)
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (keys[ALLEGRO_KEY_1]) {
                speaker++;
                if (speaker >= NUM_NPCS) {
                    perror("You has exceed the NPC limit.\n");
                    return 1;
                }
            } else if (keys[ALLEGRO_KEY_2]) {
                speaker = 0;
            }
        }

        // Dialogue Controller via Mouse
        if ((ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
             ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) &&
            dlg_open) {

            if (!choosing_topic && ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
                // it does nothing, just ignore this event
            } else {
                int mouse_x = ev.mouse.x;
                int mouse_y = ev.mouse.y;

                int tx = 50;       // topics' axes
                int ty = 250;      // topics' initial axes
                int spacing = 20;  // vertical space between topics
                int topic_w = 150; // area able to click on
                int topic_h = spacing;

                for (int i = 0; i < npc[speaker]->num_topic; i++) {
                    int top_y = ty + i * spacing;

                    if (mouse_x >= tx && mouse_x <= tx + topic_w &&
                        mouse_y >= top_y && mouse_y <= top_y + topic_h) {
                        selected_topic = i;

                        if (!choosing_topic) {
                            choosing_topic = true;
                            active_topic = -1;
                        }

                        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN &&
                            ev.mouse.button == 1) {
                            active_topic = selected_topic;
                            choosing_topic = false;
                            show_intro = false;
                        }

                        break;
                    }
                }
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            RenderMap(map);

            DrawProtag();

            if (dlg_open) {
                if (show_intro) {
                    DlgBox(npc[speaker]->portrait_id, npc[speaker]->name,
                           npc[speaker]->topics->intro_text);
                } else if (active_topic >= 0) {
                    const char *topic =
                        npc[speaker]->topics[selected_topic].topic;
                    LoadDlg(npc[speaker], topic);
                }
                TopicMenu(npc[speaker], selected_topic);
            }

            al_flip_display();
            redraw = false;
        }
    }

    //======================
    //
    //    Game Crusher
    //
    //======================

    al_destroy_display(disp);

    // Dialogue Box
    al_destroy_bitmap(npc[speaker]->portrait_id);
    for (int i = 0; i < npc[speaker]->num_topic; i++) {
        free((char *)npc[speaker]->topics[i].topic);
        free((char *)npc[speaker]->topics[i].text);
    }
    free(npc[speaker]->topics);
    free(npc[speaker]);

    tmx_map_free(map);

    ExplodeFont();

    BitmapExplode();

    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    return 0;
}
