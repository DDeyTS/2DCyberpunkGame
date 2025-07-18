//**************************************************************************
//**
//** File: dialogue_sys.c (CyberSP Project)
//** Purpose: NPC chat window
//** Last Update: 17-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "dialogue_sys.h"
#include <allegro5/allegro_font.h>

NpcTalk dlg;
NpcTalk clown;

/*
      INPUT: imagem do NPC, texto do dialogo, posição da caixa na tela;
      PROCESS: desenhar um retangulo, carregar o retrato no lado esquerdo, o
      texto no centro para a direita em 3 linhas de espessura.
      OUTPUT: caixa aparece desenhada com a foto do NPC e o texto de fala na
      tela.
*/

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, const char *text,
            float x, float y) {
  // Box sizes
  float box_w = 640;
  float box_h = 125;
  // Portrait attributes
  float portrait_size = 96;
  // Text attributes
  float padding = 10;
  float text_x = x + portrait_size + 2 * padding;
  float line_y = y + padding;
  float text_max_w = (x + box_w) - text_x - padding;
  float safe_width = text_max_w - 10.0f;

  ALLEGRO_FONT *font = al_load_ttf_font("frizquadrata_tt_regular.ttf", 18, 0);
  ALLEGRO_COLOR font_color = al_map_rgb(255, 255, 255);
  ALLEGRO_COLOR name_color = al_map_rgb(255, 255, 0);

  // Box background
  al_draw_filled_rectangle(x, y, x + box_w, y + box_h, al_map_rgb(0, 0, 50));
  // Border
  al_draw_rectangle(x, y, x + box_w, y + box_h, al_map_rgb(255, 255, 255), 2);

  if (portrait) {
    al_draw_scaled_bitmap(portrait, 0, 0, al_get_bitmap_width(portrait),
                          al_get_bitmap_height(portrait), x + padding,
                          y + padding, portrait_size, portrait_size, 0);
  }

  if (name) {
    al_draw_text(font, name_color, x + portrait_size + 2 * padding, y + padding,
                 0, name);
  }

  // Text buffer
  char buffer[1024];
  strncpy(buffer, text, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  // Get the first word of the text
  char *word = strtok(buffer, " ");
  char line[1024] = "";

  while (word != NULL) {
    char temp[1024]; // temporary buffer
    if (strlen(line) == 0) {
      snprintf(temp, sizeof(temp), "%s", word);
    } else {
      snprintf(temp, sizeof(temp), "%s %s", line, word);
    }

    if (al_get_text_width(font, temp) > safe_width) {
      // Draws current line
      al_draw_text(font, font_color, text_x, line_y + 20, 0, line);
      line_y += al_get_font_line_height(font);
      snprintf(line, sizeof(line), "%s",
               word); // Starts new line with current word
    } else {
      snprintf(line, sizeof(line), "%s", temp); // Continues accumulating words
    }

    word = strtok(NULL, " ");
  }

  // Draws the last line
  if (strlen(line) > 0) {
    al_draw_text(font, font_color, text_x, line_y + 20, 0, line);
  }

  // if (font) {
  //   al_draw_text(font, al_map_rgb(255, 255, 255),
  //                x + portrait_size + 2 * padding, y + padding, 0, text);
  // }
}
