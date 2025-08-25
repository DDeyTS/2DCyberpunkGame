//**************************************************************************
//**
//** File: dialoguesys.c (CyberSP Project)
//** Purpose: NPC chat window
//**
//** Last Update: 23-08-2025 14:25
//** Author: DDeyTS
//**
//**************************************************************************

#include "dialoguesys.h"
#include "textdat.h"
#include "bitmap.h"

// MACROS ///////////////////////////////////////////////////////////////////

#define WORDS_MAX 1024

// EXTERNAL FUNCTION PROTOTYPES /////////////////////////////////////////////

void InitStdFont();
TopicID GetTopicID(const char *topic);
void LearnTopic(const char *topic);
void ExplodeFont();

// EXTERNAL DATA DECLARATIONS ///////////////////////////////////////////////

NPC *npc[NUM_NPCS];
DescriptionObj *desc[100]; // TODO: finding a lighter way to quantify the amount of
                          // description texts inside this array
ALLEGRO_FONT *font_std, *font_subtitle;
ALLEGRO_COLOR font_color, name_color;
bool learned_topics[NUM_TOPICS] = {false};

// PRIVATE DATA DEFINITIONS /////////////////////////////////////////////////

static ALLEGRO_COLOR highlight_color;
static ALLEGRO_FONT *font_name;

//==========================================================================
//
//    CreateNpc
//
//    Argument: const char *name    - NPC's name
//              int num_topic       - number of topics the NPC can have
//    Return:   NPC                 - pointer to newly allocated NPC struct
//
//==========================================================================

NPC *CreateNpc(const char *name, int num_topic)
{
    NPC *npc       = malloc(sizeof(NPC));
    npc->name      = name;
    npc->num_topic = num_topic;
    npc->topics    = malloc(sizeof(Topic) * num_topic);
    return npc;
}

//==========================================================================
//
//    FillTopic
//
//    Argument: NPC *npc            - pointer to the specific NPC
//              int index           - index of the topic to fill
//              int char *topic     - topic tag name
//              int char *text      - respective dialogue
//    Return:   void
//
//==========================================================================

void FillTopic(NPC *npc, int index, char *topic, char *text)
{
    // NOTE: ignore the warnings, perhaps it's a glibc bug.
    npc->topics[index].topic = strdup(topic);
    npc->topics[index].text  = strdup(text);
}

//==========================================================================
//
//    FillIntro
//
//    Argument: NPC *npc        - pointer to the specific NPC
//              char * text     - introduction dialogue
//    Retunr:   void
//
//==========================================================================

void FillIntro(NPC *npc, char *text)
{
    npc->topics->intro_text = strdup(text);
}

//==========================================================================
//
//    InitDlgBox
//
//    Argument: ALLEGRO_BITMAP * portrait    - portrait of NPC
//              const char *name             - NPC's name
//              char *text                   - dialogue text to display
//    Return:   void
//
//==========================================================================

void InitDlgBox(ALLEGRO_BITMAP *portrait, const char *name, char *text)
{
    // Box attributes
    float box_w = 640;
    float box_h = 200;
    float x = 0, y = 0;

    // Portrait attributes
    float portrait_size = 173;

    // Text attributes
    float padding     = 30;
    float text_x      = x + portrait_size + 2 * padding;
    float line_y      = y + padding / 2;
    float text_max_w  = (x + box_w) - text_x - padding + 30;
    float text_max_h  = (y + box_h) - line_y - padding + 15;
    float safe_width  = text_max_w - 10.0f;
    float safe_height = text_max_h - 10.0f;

    if (portrait) {
        al_draw_scaled_bitmap(portrait, 0, 0, al_get_bitmap_width(portrait),
                              al_get_bitmap_height(portrait), x + padding,
                              y + padding - 12, portrait_size, portrait_size, 0);
    } else if (!portrait) {
        perror("Fail to load portrait!\n");
        exit(1);
    }

    // Protagonist's face
    al_draw_scaled_bitmap(protagonist, 0, 0, al_get_bitmap_width(protagonist),
                          al_get_bitmap_height(protagonist), 450, 215, portrait_size,
                          portrait_size, 0);

    // Dialogue box sprite
    al_draw_bitmap(chatbox, 0, 0, 0);

    static int light_frame   = 0;
    static double last_frame = 0;
    const double frame_delay = 0.2;
    const int total_frames   = 8;
    const int frame_w = 8, frame_h = 18;

    if (al_get_time() - last_frame >= frame_delay) {
        light_frame++;
        if (light_frame >= total_frames) {
            light_frame = 0;
        }
        last_frame = al_get_time();
    }

    if (chatbox_light) {
        al_draw_bitmap_region(chatbox_light, 0, light_frame * frame_h, frame_w,
                              frame_h, 632, 21, 0);
    }

    if (name) {
        al_draw_text(font_name, name_color, x + portrait_size + 2 * padding - 15,
                     y + padding / 2, 0, name);
    }

    int line_height = al_get_font_line_height(font_std);
    int max_lines   = safe_height / line_height;
    int line_count  = 0;

    // Text buffer
    char buffer[WORDS_MAX];
    strncpy(buffer, text, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *word           = strtok(buffer, " "); // put spaces between each word
    char line[WORDS_MAX] = "";                  // stores full line
    float cursor_x       = text_x;              // initial position to write

    while (word != NULL && line_count < max_lines) {
        bool is_highlight     = false;
        const char *draw_word = word;

        if (word[0] == '|') {
            is_highlight = true;
            draw_word    = word + 1; // ignores '|'
            LearnTopic(draw_word);   // memorize via enum
        }

        // line break
        int word_width  = al_get_text_width(font_std, draw_word);
        int space_width = al_get_text_width(font_std, " ");

        if (cursor_x + word_width > text_x + safe_width) {
            // new line
            line_y += line_height;
            cursor_x = text_x;
            line_count++;
            if (line_count >= max_lines) break;
        }

        // print dialogue
        al_draw_text(font_std, is_highlight ? highlight_color : font_color,
                     cursor_x - 15, line_y + 20, 0, draw_word);

        // updates cursor to draw the current word. That way, the next word will
        // come in the sequence with a space betweent it.
        cursor_x += word_width + space_width;

        word = strtok(NULL, " ");
    }

    // Draws the last line
    if (line_count < max_lines && strlen(line) > 0) {
        al_draw_text(font_std, font_color, text_x, line_y + 20, 0, line);
    }
}

//==========================================================================
//
//    InitTopicMenu
//
//    Argument: NPC *npc        - pointer to the specific NPC
//              int selected    - index of the currently selected topic
//    Return    void
//
//    FIXME: need to control the height of the topic list.
//
//==========================================================================

void InitTopicMenu(NPC *npc, int selected)
{
    if (npc->num_topic <= 0) return;

    float x = 100, y = 250;
    ALLEGRO_COLOR color;

    al_draw_text(font_subtitle, al_map_rgb(255, 255, 255), x, y - 15, 0,
                 "Ask About...");

    // Topic Scroller
    for (int i = 0; i < npc->num_topic; i++) {
        color =
            (i == selected) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
        al_draw_textf(font_std, color, x, y + i * 20, 0, "%s", npc->topics[i].topic);
    }
}

//==========================================================================
//
//    LoadDlg
//
//    Argument: NPC *npc            - pointer to the specific NPC
//              const char *topic   - dialogue to load
//    Return:   void
//
//==========================================================================

void LoadDlg(NPC *npc, const char *dialogue)
{
    // Introduction dialogue
    if (npc->topics->intro_text) {
        InitDlgBox(npc->portrait_id, npc->name, npc->topics->intro_text);
    }

    // Topic dialogue
    for (int i = 0; i < npc->num_topic; i++) {
        if (strcmp(npc->topics[i].topic, dialogue) == 0) {
            InitDlgBox(npc->portrait_id, npc->name, npc->topics[i].text);
            if (!npc->portrait_id) {
                printf("Warning: NPC '%s' is without portrait\n", npc->name);
                exit(1);
            }

            return;
        }
    }
}

//==========================================================================
//
//    GetTopicID
//
//    Argument: const char* topic       - specific topic to get
//    Return:   TopicID                 - enum for the topic
//
//==========================================================================

TopicID GetTopicID(const char *topic)
{
    if (strcmp(topic, "corp") == 0) return TOPIC_CORP;
    if (strcmp(topic, "price.") == 0) return TOPIC_PRICE;
    if (strcmp(topic, "kingdom") == 0) return TOPIC_KINGDOM_OF_CASH;
    if (strcmp(topic, "Ronaldo.") == 0) return TOPIC_RONALDO;

    return NONE_TOPIC;
}

//==========================================================================
//
//    LearnTopic
//
//    Argument: const char *topic       - specific topic to learn
//    Return:   void
//
//==========================================================================

void LearnTopic(const char *topic)
{
    TopicID id = GetTopicID(topic);
    if (id != NONE_TOPIC && !learned_topics[id]) {
        learned_topics[id] = true;
        // NOTE: debugger below
        printf("Novo tópico aprendido: %s (ID %d)\n", topic, id);

        UnlockExtraTopics();
    }
}

//==========================================================================
//
//    InitDescBox
//
//    Argument: float box_x            - X position of the description box
//              float box_y            - Y position of the description box
//              const char *text       - description text to display
//    Return:   void
//
//    TODO: making a function to store every description text. 
//
//==========================================================================

void InitDescBox(float box_x, float box_y, const char *text)
{
    const float padding    = 10.0f;
    const float wrap_w_cap = 200.0f; // max width to line break (without padding)
    const int line_h       = al_get_font_line_height(
        font_std); // height of each line based on the used font
    const int space_w = al_get_text_width(
        font_std, " "); // width of each space key based on the used font

    // copy the description text to a word buffer
    char buffer[WORDS_MAX];
    strncpy(buffer, text, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    // acumulators to size the box
    float line_w = 0.0f; // max width of the current line (without padding)
    float widest = 0.0f; // bigger width between all the lines (without padding)
    int lines    = 1;    // starts from 1st line

    // it's time to size how much lines and what width it need
    // for each word in the buffer, split by spaces and stored in the pointer, as
    // long as it isn't null, the pointer will update to store the next word:
    for (char *w = strtok(buffer, " "); w; w = strtok(NULL, " ")) {
        const char *draw = (w[0] == '|') ? w + 1 : w;
        int ww           = al_get_text_width(font_std, draw); // current width

        // adds padding before printed words (except at first line)
        int add = (line_w > 0) ? space_w : 0;

        // if it adding this word + space will blow up the width limit:
        if (line_w + add + ww > wrap_w_cap) {
            if (line_w > widest) widest = line_w; // stores bigger line so far
            lines++;                              // new line
            line_w = (float)ww;                   // original width of the new line
        } else {
            line_w += add + ww; // continues accmulating in the same line
        }
    }
    if (line_w > widest) widest = line_w; // last line can be the widest

    // computes description window with padding around the text
    float inner_w = widest;
    float box_w   = inner_w + 2 * padding;
    if (box_w < 60.0f) box_w = 60.0f; // minimum width
    if (box_w > wrap_w_cap + 2 * padding)
        box_w = wrap_w_cap + 2 * padding; // maximum width

    float box_h = lines * line_h + 2 * padding;

    // Draws description box
    al_draw_filled_rectangle(box_x, box_y, box_x + box_w, box_y + box_h,
                             al_map_rgba(0, 0, 50, 100));

    // Print description text
    float text_x = box_x + padding;
    float text_y = box_y + padding;
    float safe_w = box_w - 2 * padding; // safe width inside the box

    // resets word buffer to use strtok() again
    strncpy(buffer, text, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    float cx = 0.0f; // horizontal cursor inside the current line

    for (char *w = strtok(buffer, " "); w; w = strtok(NULL, " ")) {
        bool is_highlight = (w[0] == '|'); // checks if it has '|'
        const char *draw  = is_highlight ? w + 1 : w;
        int ww            = al_get_text_width(font_std, draw);

        int add = (cx > 0.0f) ? space_w : 0;

        // if it goes beyond the safe width, break the line
        if (cx + add + ww > safe_w) {
            // next line
            text_y += line_h; // resets cursor
            cx  = 0.0f;       // first word doesn't have space before it
            add = 0;
        }

        // last position of this word
        cx += add;

        // draws the word with normal color or highlight (yellow)
        al_draw_text(font_std, is_highlight ? highlight_color : font_color,
                     text_x + cx, text_y, 0, draw);

        // goes ahead the cursor to the next word
        cx += ww;

        // everything will repeat until strtok() becoming NULL
    }

    /*
     * keep this old block of code below for emergency reasons
     */

    // float padding = 10;
    // float text_x  = box_x + padding;
    // float text_y  = box_y + padding;
    // float max_box = text_x;
    // if (max_box >= 250) max_box = 250;
    // int line_h     = al_get_font_line_height(font_std);
    // int line_count = 1;
    //
    // // Lines Counter
    //
    // char buffer[WORDS_MAX];
    // strncpy(buffer, text, sizeof(buffer));
    // buffer[sizeof(buffer) - 1] = '\0';
    //
    // char *word     = strtok(buffer, " ");
    // float cursor_x = text_x; // text cursor, not the mouse cursor
    // float safe_w   = max_box - 2 * padding;
    //
    // int text_w = 0;
    //
    // while (word) {
    //     int word_w  = al_get_text_width(font_std, word);
    //     int space_w = al_get_text_width(font_std, " ");
    //
    //     if (cursor_x + word_w > text_x + safe_w) {
    //         line_count++;
    //         cursor_x = text_x;
    //
    //         if (text_w == 0) {
    //             text_w = word_w;
    //         }
    //     }
    //     cursor_x += word_w + space_w;
    //
    //     word = strtok(NULL, " ");
    // }
    //
    // printf("%d\n", text_w);
    //
    // int box_w = max_box;
    // int box_h = (line_count * line_h) + padding * 2;
    //
    // // Temporary Box
    //
    // al_draw_filled_rectangle(box_x, box_y, box_x + text_w, box_y + box_h,
    //                          al_map_rgba(0, 0, 50, 200));
    //
    // // Description Printer
    //
    // // resets to start printing text
    // strncpy(buffer, text, sizeof(buffer));
    // buffer[sizeof(buffer) - 1] = '\0';
    //
    // word     = strtok(buffer, " ");
    // cursor_x = text_x; // text cursor, not the mouse cursor
    // text_y   = box_y + padding;
    //
    // while (word) {
    //     bool is_highlight     = false;
    //     const char *draw_word = word;
    //
    //     if (word[0] == '|') {
    //         is_highlight = true;
    //         draw_word    = word + 1;
    //     }
    //
    //     int word_w  = al_get_text_width(font_std, draw_word);
    //     int space_w = al_get_text_width(font_std, " ");
    //
    //     if (cursor_x + word_w > text_x + safe_w) {
    //         text_y += line_h;
    //         cursor_x = text_x;
    //
    //         // if (line_count >= max_lines)
    //         //     break;
    //     }
    //
    //     al_draw_text(font_std, is_highlight ? highlight_color : font_color,
    //     cursor_x,
    //                  text_y, 0, draw_word);
    //
    //     cursor_x += word_w + space_w;
    //     word = strtok(NULL, " ");
    // }
}

//==========================================================================
//
//    InitStdFont
//
//    Argument: void
//    Return:   void
//
//    NOTE: this function loads all the fonts of the game.
//
//==========================================================================

void InitStdFont()
{
    const char *path = "fonts/Steelflight.ttf";
    FILE *f          = fopen(path, "r");
    if (!f) {
        perror("Error from fopen");
        fprintf(stderr, "Font doesn't found: %s\n", path);
        exit(1);
    }
    fclose(f);

    int f_size = 13;

    font_std = al_load_ttf_font("fonts/Steelflight.ttf", f_size, 0);
    if (!font_std) {
        printf("Error: fail to load font_std!\n");
        exit(1);
    }

    font_name = al_load_ttf_font("fonts/Steelflight.ttf", f_size + 1, 0);

    font_subtitle = al_load_ttf_font("fonts/Steelflight.ttf", f_size + 2, 0);

    font_color      = al_map_rgb(255, 255, 255); // white
    name_color      = al_map_rgb(255, 255, 0);   // yellow
    highlight_color = al_map_rgb(255, 215, 0);   // golden yellow
}

//==========================================================================
//
//    ExplodeFont
//
//    Argument: void
//    Return:   void
//
//    NOTE: this function destroys every font loaded before.
//
//==========================================================================

void ExplodeFont()
{
    al_destroy_font(font_std);
    al_destroy_font(font_name);
}
