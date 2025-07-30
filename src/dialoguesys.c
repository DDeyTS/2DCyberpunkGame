//**************************************************************************
//**
//** File: dialoguesys.c (CyberSP Project)
//** Purpose: NPC chat window
//** Last Update: 30-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "dialoguesys.h"

NPC *npc[40];
ALLEGRO_FONT *font_std;
static ALLEGRO_FONT *font_name;
ALLEGRO_FONT *font_subtitle;
ALLEGRO_COLOR font_color;
ALLEGRO_COLOR name_color;
ALLEGRO_BITMAP *chatbox = NULL;
ALLEGRO_BITMAP *protagonist = NULL;
static ALLEGRO_COLOR highlight_color;

void InitStdFont();
void InitChatboxBitmap();
void ExplodeDlgBox(ALLEGRO_BITMAP *stuff);
void ExplodeFont();

//==========================================================================
//
//    CreateNpc
//
//    Stores the NPC's name and amount of possible topics.
//
//==========================================================================

NPC *CreateNpc(const char *name, int num_topic) {
  NPC *npc = malloc(sizeof(NPC));
  npc->name = name;
  npc->num_topic = num_topic;
  npc->topics = malloc(sizeof(Topic) * num_topic);
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

void FillTopic(NPC *npc, int index, const char *topic, const char *text) {
  npc->topics[index].topic = strdup(topic);
  npc->topics[index].text = strdup(text);
}

//==========================================================================
//
//    FillIntro
//
//    Stores the NPC's introduction text.
//
//==========================================================================

void FillIntro(NPC *npc, const char *text) {
  npc->topics->intro_text = strdup(text);
}

//==========================================================================
//
//    DlgBox
//
//==========================================================================

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, const char *text) {
  // Box attributes
  float box_w = 640;
  float box_h = 200;
  float x = 0, y = 0;

  // Portrait attributes
  float portrait_size = 128;

  // Text attributes
  float padding = 30;
  float text_x = x + portrait_size + 2 * padding;
  float line_y = y + padding;
  float text_max_w = (x + box_w) - text_x - padding + 30;
  float text_max_h = (y + box_h) - line_y - padding;
  float safe_width = text_max_w - 10.0f;
  float safe_height = text_max_h - 10.0f;

  al_draw_bitmap(chatbox, 0, 0, 0);
  al_draw_scaled_bitmap(protagonist, 0, 0, al_get_bitmap_width(protagonist),
                        al_get_bitmap_height(protagonist), 450, 225,
                        portrait_size, portrait_size, 0);

  if (portrait) {
    al_draw_scaled_bitmap(portrait, 0, 0, al_get_bitmap_width(portrait),
                          al_get_bitmap_height(portrait), x + padding,
                          y + padding, portrait_size, portrait_size, 0);
  } else if (!portrait) {
    printf("Error: fail to load portrait!\n");
    exit(1);
  }

  if (name) {
    al_draw_text(font_name, name_color, x + portrait_size + 2 * padding - 15,
                 y + padding, 0, name);
  }

  int line_height = al_get_font_line_height(font_std);
  int max_lines = safe_height / line_height;
  int line_count = 0;

  // Text buffer
  char buffer[1024];
  strncpy(buffer, text, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  char *word = strtok(buffer, " "); // put spaces between each word
  char line[1024] = "";             // stores full line
  float cursor_x = text_x;          // initial position to write

  while (word != NULL && line_count < max_lines) {
    bool is_highlight = false;
    const char *draw_word = word;

    if (word[0] == '|') {
      is_highlight = true;
      draw_word = word + 1; // ignores '|'
    }

    // line break
    int word_width = al_get_text_width(font_std, draw_word);
    int space_width = al_get_text_width(font_std, " ");

    if (cursor_x + word_width > text_x + safe_width) {
      // new line
      line_y += line_height;
      cursor_x = text_x;
      line_count++;
      if (line_count >= max_lines)
        break;
    }

    // print dialogue
    al_draw_text(font_std, is_highlight ? highlight_color : font_color,
                 cursor_x - 15, line_y + 20, 0, draw_word);

    // updates cursor to draw the current word. That way, the next word will
    // come in the sequence with a space betweent it.
    cursor_x += word_width + space_width;

    word = strtok(NULL, " ");
  }

  /*
    This is the old text printer. It's here for upload reasons
    in case of everything blowing up!
  */
  // // Get the first word of the text
  // char *word = strtok(buffer, " ");
  // char line[1024] = "";
  //
  // while (word != NULL && line_count < max_lines) {
  //   char temp[1024]; // temporary buffer
  //   if (strlen(line) == 0) {
  //     snprintf(temp, sizeof(temp), "%s", word);
  //   } else {
  //     snprintf(temp, sizeof(temp), "%s %s", line, word);
  //   }
  //   if (al_get_text_width(font_std, temp) > safe_width) {
  //     // Draws current line
  //     al_draw_text(font_std, font_color, text_x, line_y + 20, 0, line);
  //     line_y += line_height;
  //     line_count++;
  //     snprintf(line, sizeof(line), "%s",
  //              word); // Starts new line with current word
  //   } else {
  //     snprintf(line, sizeof(line), "%s",
  //              temp); // Continues accumulating words
  //   }
  //
  //   word = strtok(NULL, " ");
  // }

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
//==========================================================================

void DrawTopicMenu(NPC *npc, int selected) {
  if (npc->num_topic <= 0)
    return;

  float x = 100, y = 250;
  // float box_w = 150, box_h = npc->num_topic * 30 + 20;
  ALLEGRO_COLOR color;

  // Topic Menu
  // al_draw_filled_rectangle(x - 10, y - 20, x + box_w, y + box_h,
  //                          al_map_rgb(0, 0, 49));
  // al_draw_rectangle(x - 10, y - 20, x + box_w, y + box_h,
  //                   al_map_rgb(82, 82, 255), 2);
  al_draw_text(font_subtitle, al_map_rgb(255, 255, 255), x, y - 15, 0,
               "Ask About...");

  // Topic Scroller
  for (int i = 0; i < npc->num_topic; i++) {
    color =
        (i == selected) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    al_draw_textf(font_std, color, x, y + i * 20, 0, "%s",
                  npc->topics[i].topic);
  }
}

//==========================================================================
//
//    LoadDlg
//
//    Loads every NPC's dialogue and relacted topic to trigger it.
//
//==========================================================================

void LoadDlg(NPC *npc, const char *topic) {
  if (npc->topics->intro_text) {
    DlgBox(npc->portrait_id, npc->name, npc->topics->intro_text);
  }

  for (int i = 0; i < npc->num_topic; i++) {
    if (strcmp(npc->topics[i].topic, topic) == 0) {
      DlgBox(npc->portrait_id, npc->name, npc->topics[i].text);
      if (!npc->portrait_id) {
        fprintf(stderr, "Warning: NPC '%s' is without portrait\n", npc->name);
        exit(1);
      }
      return;
    }
  }
}

//==========================================================================
//
//    InitStdFont
//
//    Loads and run the game's standard fonts.
//
//==========================================================================

void InitStdFont() {
  const char *path = "fonts/Steelflight.ttf";
  FILE *f = fopen(path, "r");
  if (!f) {
    perror("Error from fopen");
    fprintf(stderr, "Font doesn't found: %s\n", path);
    exit(1);
  }
  fclose(f);

  font_std = al_load_ttf_font("fonts/Steelflight.ttf", 13, 0);
  if (!font_std) {
    printf("Error: fail to load font_std!\n");
    exit(1);
  }

  font_name = al_load_ttf_font("fonts/Steelflight.ttf", 14, 0);

  font_subtitle = al_load_ttf_font("fonts/Steelflight.ttf", 15, 0);

  font_color = al_map_rgb(255, 255, 255);
  name_color = al_map_rgb(255, 255, 0);
  highlight_color = al_map_rgb(255, 215, 0);
}

//==========================================================================
//
//    ExplodeFont
//
//==========================================================================

void ExplodeFont() {
  al_destroy_font(font_std);
  al_destroy_font(font_name);
}


