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
  ALLEGRO_BITMAP *portrait_id;
  char *name;
  char **dialog;
  int num_dlg;
  int current_dlg;
  bool is_talking;
  // const char *npc_id;
  // const char *context_id;
} NpcTalk;

extern NpcTalk dlg;
extern NpcTalk npc;
extern NpcTalk clown;
extern ALLEGRO_FONT *font;

// static void GetSub(char *s, char *ss, int pos, int l);

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, const char *text);
// void ExplodeDlgBox(NpcTalk *npc);

#endif
