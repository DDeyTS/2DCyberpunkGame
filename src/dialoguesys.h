#ifndef DIALOGUE_SYS_H
#define DIALOGUE_SYS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <stdbool.h>
#include <stdio.h>

typedef enum {
  NPC_JEFFERSON,
  NPC_CLOWNGIRL,
  NPC_RAQUEL,
  NPC_CINDER,
  NPC_CHEEKS,
  NPC_BANGER_THE_KILLER,
  NUM_NPCS,
} NPCID;

typedef enum {
  NONE_TOPIC = -1,
  TOPIC_CORP,
  TOPIC_PRICE,
  TOPIC_KINGDOM_OF_CASH,
  TOPIC_RONALDO,
  NUM_TOPICS,
} TopicID;

typedef struct {
  const char *topic;
  const char *text;
  const char *intro_text;
  ALLEGRO_BITMAP *menu_box;
} Topic;

typedef struct {
  ALLEGRO_BITMAP *portrait_id;
  const char *name;
  int num_topic;
  Topic *topics;
} NPC;

extern NPC *npc[];
extern ALLEGRO_FONT *font_std, *font_subtitle;
extern ALLEGRO_COLOR font_color, name_color;
extern ALLEGRO_BITMAP *chatbox, *protagonist;
extern bool learned_topics[NUM_TOPICS];

void DlgBox(ALLEGRO_BITMAP *portrait, const char *name, const char *text);
NPC *CreateNpc(const char *name, int num_topic);
void FillTopic(NPC *npc, int index, const char *topic, const char *text);
void LoadDlg(NPC *npc, const char *topic);
void TopicMenu(NPC *npc, int selected);
void InitStdFont();
void FillIntro(NPC *npc, const char *text);
void ExplodeFont();

#endif
