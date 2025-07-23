//**************************************************************************
//**
//** File: dialogue_sys.c (CyberSP Project)
//** Purpose: NPC chat window
//** Last Update: 22-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "dialoguesys.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>

NPC dlg;
NPC clown;
NPC npc;

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
//    DlgBox
//
//==========================================================================

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, const char *text) {
  // Box sizes
  float box_w = 640;
  float box_h = 125;
  float x = 0, y = 0;
  // Portrait attributes
  float portrait_size = 96;
  // Text attributes
  float padding = 10;
  float text_x = x + portrait_size + 2 * padding;
  float line_y = y + padding;
  float text_max_w = (x + box_w) - text_x - padding;
  float text_max_h = (y + box_h) - line_y - padding;
  float safe_width = text_max_w - 10.0f;
  float safe_height = text_max_h - 10.0f;

  ALLEGRO_FONT *font = al_load_ttf_font("frizquadrata_tt_regular.ttf", 17, 0);
  ALLEGRO_COLOR font_color = al_map_rgb(255, 255, 255);
  ALLEGRO_COLOR name_color = al_map_rgb(255, 255, 0);

  // Dialogue Box
  ALLEGRO_BITMAP *box = al_load_bitmap("chatbox_sprite.png");
  al_draw_bitmap(box, 0, 0, 0);

  if (portrait) {
    al_draw_scaled_bitmap(portrait, 0, 0, al_get_bitmap_width(portrait),
                          al_get_bitmap_height(portrait), x + padding,
                          y + padding, portrait_size, portrait_size, 0);
  }

  if (name) {
    al_draw_text(font, name_color, x + portrait_size + 2 * padding, y + padding,
                 0, name);
  }

  int line_height = al_get_font_line_height(font);
  int max_lines = safe_height / line_height;
  int line_count = 0;

  // Text buffer
  char buffer[1024];
  strncpy(buffer, text, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  // Get the first word of the text
  char *word = strtok(buffer, " ");
  char line[1024] = "";

  while (word != NULL && line_count < max_lines) {
    char temp[1024]; // temporary buffer
    if (strlen(line) == 0) {
      snprintf(temp, sizeof(temp), "%s", word);
    } else {
      snprintf(temp, sizeof(temp), "%s %s", line, word);
    }
    if (al_get_text_width(font, temp) > safe_width) {
      // Draws current line
      al_draw_text(font, font_color, text_x, line_y + 20, 0, line);
      line_y += line_height;
      line_count++;
      snprintf(line, sizeof(line), "%s",
               word); // Starts new line with current word
    } else {
      snprintf(line, sizeof(line), "%s",
               temp); // Continues accumulating words
    }

    word = strtok(NULL, " ");
  }

  // Draws the last line
  if (line_count < max_lines && strlen(line) > 0) {
    al_draw_text(font, font_color, text_x, line_y + 20, 0, line);
  }
}

void DrawTopicMenu(NPC *npc, int selected) {
  if (npc->num_topic <= 0)
    return;

  float x = 50, y = 150;
  float box_w = 200, box_h = npc->num_topic * 30 + 20;
  ALLEGRO_COLOR color;
  ALLEGRO_FONT *font = al_load_ttf_font("frizquadrata_tt_regular.ttf", 16, 0);
  ALLEGRO_FONT *title = al_load_ttf_font("frizquadrata_tt_regular.ttf", 18, 0);

  // Topic Menu
  al_draw_filled_rectangle(x - 10, y - 20, x + box_w, y + box_h ,
                           al_map_rgba(0, 0, 150, 200));
  al_draw_text(title, al_map_rgb(255, 255, 255), x, y - 15, 0, "Ask for...");

  // Topic Loader
  for (int i = 0; i < npc->num_topic; i++) {
    color =
        (i == selected) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    al_draw_textf(font, color, x, y + i * 25, 0, "%s", npc->topics[i].topic);
  }
}

void LoadDlg(NPC *npc, const char *topic) {
  for (int i = 0; i < npc->num_topic; i++) {
    if (strcmp(npc->topics[i].topic, topic) == 0) {
      DlgBox(npc->portrait_id, npc->name, npc->topics[i].text);
      return;
    }
  }
}

//==========================================================================
//
//    ExplodeDlgBox
//
//==========================================================================

void ExplodeDlgBox(ALLEGRO_BITMAP *portrait) { al_destroy_bitmap(portrait); }
