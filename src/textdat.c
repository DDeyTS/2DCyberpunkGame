//**************************************************************************
//**
//** File: textdat.c (CyberSP Project)
//** Purpose: Dialogue storage
//** Last Update: 28-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "textdat.h"

//==========================================================================
//
//    NpcLoader
//
//    Each dialogue, NPC index and topics are stored here.
//
//==========================================================================

void NpcLoader(NPC *npc[]) {
  /*
    ID 0, “Jefferson”.
    Jefferson is a well-known drugdealer throughout São Paulo's slums.
    He sells enough derma to tranquilize the skinned of a neocangaceiro.
    Despite of his dirty job, he is famous for building new residences for
    low-income families, and he collects the lesser taxes in the rainy
    metroplex.
  */
  npc[0] = CreateNpc("Jefferson", 4);
  npc[0]->portrait_id = al_load_bitmap("portraits/drugdealer_portrait.png");
  FillIntro(npc[0], "What's up, bro. Are you okay?");
  FillTopic(
      npc[0], 0, "Drugs",
      "Do ya want which of 'em? Good stuff helps ya to relax the body; of "
      "course, leavin' behind the cooldown you suffers after the effect "
      "is gone. Bad stuff, however, it's like a violent punch in your "
      "pancreas.");
  FillTopic(npc[0], 1, "Dolls",
            "They're everywhere, bro. In every street. They know 'bout "
            "absolutely everything inside this district!");
  FillTopic(npc[0], 2, "Firearms",
            "If ya wanna some guns to brighten your night up, talks with "
            "|Ronaldo. He has a lot of stuff to show ya.");
  FillTopic(
      npc[0], 3, "Bettingshop",
      "Alright. I know one very close o' here. Just go down the slum and if ya "
      "were seeing some hot lights tearing up the sky, you're there.");

  /*
    ID 1, “Clowngirl”.
    Samira, also known as “Clowngirl”, comes from a circus family that
    perfomed in the northeaestern region many years ago. She is funny and crazy,
    and she claiming that a man with a wide hat and inexpressive sunglasses is
    constantly following her. She likes to entertain opharned children weekly,
    while struggling with depression.
  */
  npc[1] = CreateNpc("Clowngirl", 2);
  npc[1]->portrait_id = al_load_bitmap("portraits/clowngirl_portrait.png");
  FillIntro(npc[1], "Helloooooooo, guys!");
  FillTopic(npc[1], 0, "Who are you?",
            "Who am I? Do ya really want to ask for that? I'm a clown, dumb!");
  FillTopic(npc[1], 1, "Funny",
            "Yeah, yeah... I'm fuckin' funny. Very, very funny!");

  /*
    ID 2, “Raquel”.
    Raquel is the best call girl (aka doll) in the southern district. She is a
    trans woman who loves sex. Some clients call her nymphomaniac. However, she
    doesn't only work to please depraved people. She's usually paid to kill
    specific targets while she's with them. The opportune kis of death. She has
    cybernetic razors beneath her fake nails.
  */
  npc[2] = CreateNpc("Raquel", 1);
  npc[2]->portrait_id = al_load_bitmap("portraits/raquel_portrait.png");
  FillIntro(npc[2], "Nah-ah, babe. You'll not have me riding on ya. Take a "
                    "walk and finds another doll to play with.");
  FillTopic(npc[2], 0, "Doll",
            "They can be all you want to play with, babe. A tall man, a young "
            "woman... After that, you delight of their bodies like never did "
            "before. Of course, for a |price. Always it has a price.");

  /*
    ID 3, “Cinder”.
    Cinder run nearly all the dolls in the metroplex. She is wild and
    audacious, testing the government's limits to take back the control of her
    district that is dominated by privated forces. She also has a big appeal for
    men that has an iron fist to retain what is yours.
  */
  npc[3] = CreateNpc("Cinder", 1);
  npc[3]->portrait_id = al_load_bitmap("portraits/cinder_portrait.png");
  FillIntro(npc[3], "Hi, confuse man. Are you here to stare me like a erect "
                    "perv’ or you'll tell something? C'mon. I won't bite you.");
  FillTopic(
      npc[3], 0, "Bettingshop",
      "That cursed place? The |kingdom |of |cash. I hang out with a |corp’ once; "
      "he was disgusting, honestly. Anybody who goes there hardly can keep the "
      "bank account filled. If you're wanna step on there, take care.");

  /*
     ID 4, “Cheeks”.
     Big fan of rock ‘n’ roll, Cheeks is a good vibes singer who throw curses at
     the actual metroplex's government. Crowns of followers see him as a choisen
     one to free everyone of the megacorps' chains.
  */
  npc[4] = CreateNpc("Cheeks", 1);
  npc[4]->portrait_id = al_load_bitmap("portraits/bigcheeks_portrait.png");
  FillIntro(npc[4], "What's uuuuup, my friend! How can I help ya today?");
  FillTopic(
      npc[4], 0, "Nightclub",
      "There are a lot of over there. Follow your sixth sense, dude, follow it "
      "to the end of the horizon. The sun takes time to be up on the iron "
      "color sky. Iron sky... Wow, it could gimme an awesome song name.");
}
