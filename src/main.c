//**************************************************************************
//**
//** File: main.c (CyberSP Project)
//** Purpose: Main game stuff
//**
//** Last Update: 25-08-2025 00:51
//** Author: DDeyTS
//**
//**************************************************************************

/*
 * LIST OF FEATURES TO DO (24-08-25)
 * 1. Collision walls on the map.
 * 2. NPC sprite render.
 * 3. Shooter mode.
 *     3a. Shooting with pistol.
 *     3b. Damage system.
 * 4. Description text storage.
 */

#include "main.h"
#include "bitmap.h"
#include "collision.h"
#include "dialoguesys.h"
#include "textdat.h"
#include "tile_render.h"

// PRIVATE FUNCTION PROTOTYPES //////////////////////////////////////////////

static void KeyboardOn(void);
static void MouseOn(void);
static bool InitCursor(ALLEGRO_DISPLAY *disp);

// EXTERNAL DATA DECLARATIONS ///////////////////////////////////////////////

Mousecursors cursors;
tmx_map *map = NULL;
ALLEGRO_EVENT ev;
bool keys[ALLEGRO_KEY_MAX], mouse[MOUSE_MAX + 1];
int mouse_x, mouse_y = 0;

// PUBLIC DATA DEFINITIONS //////////////////////////////////////////////////

ALLEGRO_DISPLAY *disp;
ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_TIMER *timer;
ALLEGRO_MOUSE_CURSOR *current_cursor = NULL;
enum CursorType cursor_flag          = CURSOR_NORMAL;
ALLEGRO_MOUSE_CURSOR *chosen_cursor;
bool mouse_animating = false;
bool show_desc       = false;

// PRIVATE DATA DEFINITIONS /////////////////////////////////////////////////

static double anim_timer    = 0.0;  // NOTE: in case of trouble, use double
static double anim_duration = 0.15; // NOTE: same above
static bool show_intro      = true; // turn on the intro dialogue
static bool dlg_open        = false; // turn on the chat mode
static bool choosing_topic  = true; // turn on the topic list
static int speaker          = 0; // flags the current NPC 
static int selected_topic   = 0; 
static int active_topic     = -1;

//==========================================================================
//
//    Main Game
//
//    Initializers, game loop, crusher... etc.
//
//==========================================================================

int main(void)
{
    // INITIALIZERS /////////////////////////////////////////////////////////

    if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon() ||
        !al_install_keyboard() || !al_init_font_addon() || !al_init_ttf_addon() ||
        !al_install_mouse()) {
        perror("Fail to initialize Allegro\n");
        return 1;
    }

    disp  = al_create_display(DISPW, DISPH);
    queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 30.0);
    if (!disp || !queue || !timer) {
        perror("Fail to initialize basic stuff!\n");
        return 1;
    }

    InitStdFont();
    InitBitmap();
    InitCursor(disp);
    NpcDlgStorage(npc);

    // EVENT QUEUE //////////////////////////////////////////////////////////

    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    // PLAYER MOVEMENT //////////////////////////////////////////////////////

    spr.px = 320;
    spr.py = 200;
    // Frames (and the initial frame when the program begins)
    spr.frame_w = 0;
    spr.frame_h = 0;
    // Movement Speed
    float sp = 3.5;
    // Sprite Frames
    float frames = 0.f;

    // GAME LOOP ////////////////////////////////////////////////////////////

    bool running = true;
    bool redraw  = true;

    // reset these arrays
    memset(keys, 0, sizeof(keys));
    memset(mouse, 0, sizeof(mouse));

    // defines callbacks for libTMX
    tmx_img_load_func = AllegTexLoader;
    tmx_img_free_func = AllegTexFree;
    map               = tmx_load("tiles/buspoint.tmx");
    if (!map) {
        tmx_perror("Cannot load map");
        return 1;
    }

    while (running) {
        al_wait_for_event(queue, &ev);

        KeyboardOn();
        MouseOn();

        // close the game
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;

        // Main Timer
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // mouse clicking
            if (mouse_animating) {
                if (current_cursor != cursors.clicking && cursors.clicking) {
                    current_cursor = cursors.clicking;
                    al_set_mouse_cursor(disp, current_cursor);
                }
                if ((al_get_time() - anim_timer) >= anim_duration) {
                    mouse_animating = false;

                    current_cursor = chosen_cursor;
                    al_set_mouse_cursor(disp, current_cursor);
                }
            } else {
                if (current_cursor != cursors.normal) {
                    current_cursor = chosen_cursor;
                    al_set_mouse_cursor(disp, current_cursor);
                }
            }

            // character animation
            frames += 0.3f;   // frame speed
            if (frames > 4) { // reset frame queue
                frames -= 4;
            }

            // sprite follows cursor when aiming
            if (chosen_cursor == cursors.aim) {
                SpriteAimAtCursor(spr.px, spr.py, &spr.frame_h);
                spr.frame_w = 0;
            } else {
                SpriteMovement(keys, &spr.px, &spr.py, sp, &spr.frame_w,
                               &spr.frame_h, (int)frames);
            }

            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            // TODO: make functions for each game aspect like either
            // DrawProtag() or RenderMap()

            al_clear_to_color(al_map_rgb(0, 0, 0));
            RenderMap(map);

            DrawProtag();

            // NOTE: rectangle to debug description window
            // al_draw_filled_rectangle(350, 125, 450, 200, al_map_rgba(0, 100, 0,
            // 200));
            if (show_desc) {
                InitDescBox(325, 100, "A lone bus stop.");
            }

            if (dlg_open) {
                if (show_intro) {
                    InitDlgBox(npc[speaker]->portrait_id, npc[speaker]->name,
                               npc[speaker]->topics->intro_text);
                } else if (active_topic >= 0) {
                    const char *topic = npc[speaker]->topics[selected_topic].topic;
                    LoadDlg(npc[speaker], topic);
                }
                InitTopicMenu(npc[speaker], selected_topic);
            }

            al_flip_display();
            redraw = false;
        }
    }

    // GAME CRUSHER /////////////////////////////////////////////////////////

    // Dialogue Sys
    al_destroy_bitmap(npc[speaker]->portrait_id);
    for (int i = 0; i < npc[speaker]->num_topic; i++) {
        free(npc[speaker]->topics[i].topic);
        free(npc[speaker]->topics[i].text);
    }
    free(npc[speaker]->topics);
    free(npc[speaker]);

    tmx_map_free(map);
    ExplodeFont();
    BitmapExplode();
    if (cursors.normal) al_destroy_mouse_cursor(cursors.normal);
    if (cursors.view) al_destroy_mouse_cursor(cursors.view);
    if (cursors.aim) al_destroy_mouse_cursor(cursors.aim);
    if (cursors.clicking) al_destroy_mouse_cursor(cursors.clicking);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_display(disp);

    return 0;
}

//==========================================================================
//
//    KeyboardOn
//
//    Argument: void
//    Return:   void
//
//    NOTE: this functions contains every keyboard operation.
//    TODO: optimize it to contain only input stuff.
//
//==========================================================================

void KeyboardOn()
{
    // Keyboard Boolean
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        keys[ev.keyboard.keycode] = true;
    } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
        keys[ev.keyboard.keycode] = false;
    }

    // Cursor Bitmap Changer
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        if (keys[ALLEGRO_KEY_T]) {          // NOTE: target mode
            chosen_cursor = cursors.aim;    // the cursor bitmap is changed
            cursor_flag   = CURSOR_TARGET;  // flagging the current cursor
            if (dlg_open) dlg_open = false; // doesn't work during chat mode
            if (!mouse_animating) {         // doesn't work during cursor click
                current_cursor = chosen_cursor;
                al_set_mouse_cursor(disp, current_cursor);
            }
        } else if (keys[ALLEGRO_KEY_H]) { // NOTE: hand/normal mode
            chosen_cursor = cursors.normal;
            cursor_flag   = CURSOR_NORMAL;
            al_set_mouse_cursor(disp, current_cursor);
        } else if (keys[ALLEGRO_KEY_E]) { // NOTE: eye/view mode
            chosen_cursor = cursors.view;
            cursor_flag   = CURSOR_EYE;
            if (dlg_open) dlg_open = false;
            if (!mouse_animating) {
                current_cursor = chosen_cursor;
                al_set_mouse_cursor(disp, current_cursor);
            }
        }
    }

    // Dialogue Trigger Button
    // NOTE: for debugging
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN && keys[ALLEGRO_KEY_SPACE]) {
        dlg_open       = true;
        choosing_topic = true;
        show_intro     = true;
    }

    // Dialogue Box Exit
    // TODO: bitmap button to do it
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN && keys[ALLEGRO_KEY_ESCAPE]) {
        dlg_open     = false;
        active_topic = -1;
        show_intro   = false;
    }

    // NPC Changer
    // NOTE: for debugging
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
//    Argument: void
//    Return:   void
//
//    NOTE: this function contains every mouse operation.
//    TODO: optimize it to contain only input stuff.
//
//==========================================================================

void MouseOn()
{
    // Global mouse position
    if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
        mouse_x = ev.mouse.x;
        mouse_y = ev.mouse.y;
    }

    // Mouse Boolean
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        mouse[ev.mouse.button] = true;

        // checks if eye cursor click on the object
        if (chosen_cursor == cursors.view && mouse[1]) {
            int rect_x = 350, rect_y = 125;
            int rect_w = rect_x + 100, rect_h = rect_y + 75;

            // finds the collisin to trigger InitDescBox()
            if (mouse_x >= rect_x && mouse_x <= rect_w && mouse_y >= rect_y &&
                mouse_y <= rect_h) {
                show_desc = true;
                // debugger below
                printf("Apareceu a caixa!\n");
            }
        }
    } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        mouse[ev.mouse.button] = false;
    }

    // Click Animation
    if (mouse[1] && chosen_cursor == cursors.normal) {
        mouse_animating = true;
        anim_timer      = al_get_time();
        if (cursors.clicking) {
            current_cursor = cursors.clicking;
            al_set_mouse_cursor(disp, current_cursor);
        }
    }

    // Dialogue Interaction
    if ((ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
         ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) &&
        dlg_open) {

        if (!choosing_topic && ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            // nothing happens, just ignore the event
        } else {
            mouse_x = ev.mouse.x;
            mouse_y = ev.mouse.y;

            int tx      = 50;  // topics' axes
            int ty      = 250; // topics' initial axes
            int spacing = 20;  // vertical space between topics
            int topic_w = 150; // area able to click on
            int topic_h = spacing;

            // as long as integer is lesser than NPC's number of topics, do the loop
            // body, then increment integer by 1 for the next topic:
            for (int i = 0; i < npc[speaker]->num_topic; i++) {
                int top_y = ty + i * spacing;

                // checks if mouse is inside the clickable area of the topic
                if (mouse_x >= tx && mouse_x <= tx + topic_w && mouse_y >= top_y &&
                    mouse_y <= top_y + topic_h) {
                    selected_topic = i; // defines the chosen topic

                    if (!choosing_topic) {
                        choosing_topic = true;
                        active_topic   = -1; // no active topic, yet
                    }

                    if (mouse[1]) {
                        active_topic   = selected_topic; // topic is activated
                        choosing_topic = false;          // quit from choosing mode
                        show_intro     = false;          // hide the intro dialogue
                    }

                    break; // finish loop when the mouse finds a topic
                }
            }
        }
    }
}

//==========================================================================
//
//    InitCursor
//
//    Argument: ALLEGRO_DISPLAY *disp    - linker for cursor to the display
//    Return:   bool
//
//==========================================================================

bool InitCursor(ALLEGRO_DISPLAY *disp)
{
    if (!cursors.eye_bmp || !cursors.mouse_bmp || !cursors.click_bmp ||
        !cursors.target_bmp) {
        perror("Fail to load cursor bitmap\n");
        return false;
    }

    cursors.normal   = al_create_mouse_cursor(cursors.mouse_bmp, 0, 0);
    cursors.clicking = al_create_mouse_cursor(cursors.click_bmp, 0, 0);
    cursors.aim      = al_create_mouse_cursor(cursors.target_bmp, 0, 0);
    cursors.view     = al_create_mouse_cursor(cursors.eye_bmp, 0, 0);
    if (!cursors.normal || !cursors.clicking || !cursors.aim || !cursors.view) {
        fprintf(stderr, "Error: fail to to create mouse cursor!\n");
        return false;
    }

    chosen_cursor  = cursors.normal;
    cursor_flag    = CURSOR_NORMAL;
    current_cursor = chosen_cursor;
    al_set_mouse_cursor(disp, current_cursor);
    al_show_mouse_cursor(disp);

    return true;
}
