#ifndef DIALOGUE_SYS_H
#define DIALOGUE_SYS_H

#include "bitmap.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/bitmap.h>
#include <stdbool.h>
#include <stdio.h>

// #define MAX_DIALOG_LEN 256

typedef struct {
  const char *topic;
  const char *text;
} Topic;

typedef struct {
  ALLEGRO_BITMAP *portrait_id;
  const char *name;
  char **dialog;
  int num_topic;
  Topic *topics;
} NPC;



extern NPC dlg;
extern NPC npc;
extern NPC clown;
extern ALLEGRO_FONT *font;

// static void GetSub(char *s, char *ss, int pos, int l);

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, const char *text);
// void ExplodeDlgBox(NpcTalk *npc);
NPC *CreateNpc(const char *name, int num_topic);
void FillTopic(NPC *npc, int index, const char *topic, const char *text);
void LoadDlg(NPC *npc, const char *topic);
void ExplodeDlgBox(ALLEGRO_BITMAP *portrait);

#endif
