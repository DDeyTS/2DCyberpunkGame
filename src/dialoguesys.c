//**************************************************************************
//**
//** File: dialoguesys.c (CyberSP Project)
//** Purpose: NPC chat window
//** Last Update: 21-08-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "dialoguesys.h"
#include "textdat.h"

#define WORDS_MAX 1024

// EXTERNAL FUNCTION PROTOTYPES ///////////
void InitStdFont();
TopicID GetTopicID(const char *topic);
void LearnTopic(const char *topic);
void ExplodeFont();

// EXTERNAL DATA DECLARATIONS ///////////
NPC *npc[NUM_NPCS];
ALLEGRO_FONT *font_std, *font_subtitle;
ALLEGRO_COLOR font_color, name_color;
ALLEGRO_BITMAP *chatbox, *protagonist, *chatbox_light = NULL;
bool learned_topics[NUM_TOPICS] = {false};

// PRIVATE DATA DEFINITIONS ///////////
static ALLEGRO_COLOR highlight_color;
static ALLEGRO_FONT *font_name;

//==========================================================================
//
//    CreateNpc
//
//    Stores the NPC's name and amount of possible topics.
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
//    Stores the topic's ID and tag, as well as the dialogue text related to
//    it.
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
//    Stores the NPC's introduction text.
//
//==========================================================================

void FillIntro(NPC *npc, char *text)
{
    npc->topics->intro_text = strdup(text);
}

//==========================================================================
//
//    DlgBox
//
//==========================================================================

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, char *text)
{
    // Box attributes
    float box_w = 640;
    float box_h = 200;
    float x = 0, y = 0;

    // Portrait attributes
    float portrait_size = 165;

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
//    DrawTopicMenu
//
//    Window with a list of topics to ask for.
//
//    FIXME: controls the height of the topic list.
//
//==========================================================================

void DrawTopicMenu(NPC *npc, int selected)
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
//    Loads every NPC's dialogue and relacted topic to trigger it.
//
//==========================================================================

void LoadDlg(NPC *npc, const char *topic)
{
    if (npc->topics->intro_text) {
        DlgBox(npc->portrait_id, npc->name, npc->topics->intro_text);
    }

    for (int i = 0; i < npc->num_topic; i++) {
        if (strcmp(npc->topics[i].topic, topic) == 0) {
            DlgBox(npc->portrait_id, npc->name, npc->topics[i].text);
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
//    Memorizes any said topic by any NPC.
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
//    Loads the learned topic.
//
//==========================================================================

void LearnTopic(const char *topic)
{
    TopicID id = GetTopicID(topic);
    if (id != NONE_TOPIC && !learned_topics[id]) {
        learned_topics[id] = true;
        // debugger
        printf("Novo tópico aprendido: %s (ID %d)\n", topic, id);

        UnlockExtraTopics();
    }
}

//==========================================================================
//
//    DescBox
//
//    Prints little descriptions about anything player want to see in detail
//
//==========================================================================

void DescBox(const char *text)
{
    float max_box = 250;
    float box_x = 100, box_y = 100;
    float padding = 10;

    float text_x = box_x + padding;
    float text_y = box_y + padding;

    int line_h     = al_get_font_line_height(font_std);
    int line_count = 1;

    // LINES COUNTER ///////////
    char buffer[WORDS_MAX];
    strncpy(buffer, text, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *word     = strtok(buffer, " ");
    float cursor_x = text_x; // text cursor, not the mouse cursor
    float safe_w   = max_box - 2 * padding;

    while (word) {
        int word_w  = al_get_text_width(font_std, word);
        int space_w = al_get_text_width(font_std, " ");

        if (cursor_x + word_w > text_x + safe_w) {
            line_count++;
            cursor_x = text_x;
        }
        cursor_x += word_w + space_w;

        word = strtok(NULL, " ");
    }

    int box_w = max_box;
    int box_h = (line_count * line_h) + padding * 2;

    // DRAWS TEXT TEMPORARY BOX ///////////
    al_draw_filled_rectangle(box_x, box_y, box_x + box_w, box_y + box_h,
                             al_map_rgba(0, 0, 50, 200));

    // DESCRIPTION PRINTER ///////////
    // resets to start printing text
    strncpy(buffer, text, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    word     = strtok(buffer, " ");
    cursor_x = text_x; // text cursor, not the mouse cursor
    text_y   = box_y + padding;

    while (word) {
        bool is_highlight     = false;
        const char *draw_word = word;

        if (word[0] == '|') {
            is_highlight = true;
            draw_word    = word + 1;
        }

        int word_w  = al_get_text_width(font_std, draw_word);
        int space_w = al_get_text_width(font_std, " ");

        if (cursor_x + word_w > text_x + safe_w) {
            text_y += line_h;
            cursor_x = text_x;

            // if (line_count >= max_lines)
            //     break;
        }

        al_draw_text(font_std, is_highlight ? highlight_color : font_color, cursor_x,
                     text_y, 0, draw_word);

        cursor_x += word_w + space_w;
        word = strtok(NULL, " ");
    }
}

//==========================================================================
//
//    InitStdFont
//
//    Loads and run the game's standard fonts.
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
//==========================================================================

void ExplodeFont()
{
    al_destroy_font(font_std);
    al_destroy_font(font_name);
}
