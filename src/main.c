//**************************************************************************
//**
//** File: main.c (CyberSP Project)
//** Purpose: Main game stuff
//** Last Update: 10-08-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "main.h"
#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"
#include "textdat.h"
#include "tile_render.h"

// dialogue stuff
static bool show_intro = true;
static bool dlg_open = false;
static bool choosing_topic = true;
static int speaker = 0;
static int selected_topic = 0;
static int active_topic = -1;
// keyboard & mouse stuff
bool keys[ALLEGRO_KEY_MAX];
bool mouse[MOUSE_MAX + 1];
bool mouse_animating = false;
double anim_timer = 0.0;
double anim_duration = 0.3;

tmx_map *map = NULL;
ALLEGRO_DISPLAY *disp;
ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_TIMER *timer;
ALLEGRO_BITMAP *mouse_bmp, *target_bmp, *mouse_click_bmp = NULL;
ALLEGRO_MOUSE_CURSOR *cursor, *cursor_aim, *cursor_clicking,
    *cursor_normal = NULL;
ALLEGRO_EVENT ev;

//==========================================================================
//
//    KeyboardOn
//
//==========================================================================

void KeyboardOn() {
    // Keyboard boolean
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        keys[ev.keyboard.keycode] = true;
    } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
        keys[ev.keyboard.keycode] = false;
    }

    // Cursor Bitmap Changer
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN && keys[ALLEGRO_KEY_T]) {
        if (cursor)
            al_destroy_mouse_cursor(cursor);
        cursor = al_create_mouse_cursor(target_bmp, 0, 0);
        al_set_mouse_cursor(disp, cursor);
    } else if (keys[ALLEGRO_KEY_H]) {
        if (cursor)
            al_destroy_mouse_cursor(cursor);
        cursor = al_create_mouse_cursor(mouse_bmp, 0, 0);
        al_set_mouse_cursor(disp, cursor);
    }

    // Dialog Trigger Button (for debugging)
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN && keys[ALLEGRO_KEY_SPACE]) {
        dlg_open = true;
        choosing_topic = true;
        show_intro = true;
    }
    // Closes the Dialogue Box window
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN && keys[ALLEGRO_KEY_ESCAPE]) {
        dlg_open = false;
        active_topic = -1;
        show_intro = false;
    }

    // NPC Changer (for debugging)
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN && keys[ALLEGRO_KEY_1]) {
        speaker++;
        if (speaker >= NUM_NPCS) {
            perror("You has exceed the NPC limit.\n");
            exit(1);
        }
    } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && keys[ALLEGRO_KEY_2]) {
        speaker = 0;
    }
}

//==========================================================================
//
//    MouseOn
//
//==========================================================================

void MouseOn() {
    // Mouse boolean
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        mouse[ev.mouse.button] = true;
    } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        mouse[ev.mouse.button] = false;
    }

    if (mouse[1]) {
        mouse_animating = true;
        anim_timer = al_get_time();
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

                    if (mouse[1]) {
                        active_topic = selected_topic;
                        choosing_topic = false;
                        show_intro = false;
                    } else if (mouse[2]) {
                        dlg_open = false;
                    }

                    break;
                }
            }
        }
    }
}

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
    NpcLoader(npc);

    disp = al_create_display(DISPW, DISPH);
    queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 30.0);
    cursor_normal = al_create_mouse_cursor(mouse_bmp, 0, 0);
    cursor_clicking = al_create_mouse_cursor(mouse_click_bmp, 0, 0);
    cursor_aim = al_create_mouse_cursor(target_bmp, 0, 0);
    cursor = cursor_normal;
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
    //    Player Movement
    //
    //======================

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

    bool running = true;
    bool redraw = true;

    // reset these arrays
    memset(keys, 0, sizeof(keys));
    memset(mouse, 0, sizeof(mouse));

    // defines callbacks for libTMX
    tmx_img_load_func = AllegTexLoader;
    tmx_img_free_func = AllegTexFree;
    map = tmx_load("tiles/debugavenue.tmx");
    if (!map) {
        tmx_perror("Cannot load map");
        return 1;
    }

    while (running) {
        al_wait_for_event(queue, &ev);

        KeyboardOn();
        MouseOn();

        // close the game
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            running = false;

        // Main Timer
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // character animation
            frames += 0.3f;   // frame speed
            if (frames > 4) { // reset frame queue
                frames -= 4;
            }
            SpriteMovement(keys, &spr.px, &spr.py, sp, &spr.frame_w,
                           &spr.frame_h, (int)frames);

            // mouse clicking
            if (mouse_animating) {
                if (cursor != cursor_clicking) {
                    cursor = cursor_clicking;
                    al_set_mouse_cursor(disp, cursor);
                }
                if ((al_get_time() - anim_timer) >= anim_duration) {
                    mouse_animating = false;
                }
            } else {
                if (cursor != cursor_normal) {
                    cursor = cursor_normal;
                    al_set_mouse_cursor(disp, cursor);
                }
            }

            redraw = true;
        }

        // Game Loop
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

    al_destroy_mouse_cursor(cursor);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    return 0;
}
