#ifndef DIALOGUE_SYS_H
#define DIALOGUE_SYS_H

#include "bitmap.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <stdio.h>

// #define MAX_DIALOG_LEN 256

typedef struct {
  ALLEGRO_BITMAP *portrait;

  char name[32];
  char *dialog;
} NpcTalk;

extern NpcTalk dlg;
extern NpcTalk clown;
extern ALLEGRO_FONT *font;

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, const char *text, float x, float y);

#endif
