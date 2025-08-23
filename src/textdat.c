//**************************************************************************
//**
//** File: textdat.c (CyberSP Project)
//** Purpose: Dialogue storage
//**
//** Last Update: 22-08-2025 01:09
//** Author: DDeyTS
//**
//**************************************************************************

#include "textdat.h"
#include "dialoguesys.h"

// PUBLIC FUNCTION PROTOYPES ///////////////////////////////////////////////

void UnlockExtraTopics();

// PUBLIC DATA DECLARATIONS /////////////////////////////////////////////// 

//==========================================================================
//
//    NpcDlgStorage
//
//    Argument: NPC *npc        - specifies which NPC is talking
//    Return:   void
//
//    NOTE: everytime you'll write a dialogue, take care of the five lines limit
//    otherwise the text won't fit in the upper dialogue box.
//
//==========================================================================

void NpcDlgStorage(NPC *npc[])
{
    /*
      ID 0, “Jefferson”.
      Jefferson is a well-known drugdealer throughout São Paulo's slums.
      He sells enough derma to tranquilize the skinned of a neocangaceiro.
      Despite of his dirty job, he is famous for building new residences for
      low-income families, and he collects the lesser taxes in the rainy
      metroplex.
    */
    npc[NPC_JEFFERSON] = CreateNpc("Jefferson", 4);
    npc[NPC_JEFFERSON]->portrait_id =
        al_load_bitmap("portraits/drugdealer_portrait.png");
    FillIntro(npc[NPC_JEFFERSON], "What's up, bro. Are you okay?");
    FillTopic(npc[NPC_JEFFERSON], 0, "Drugs",
              "Do ya want which of 'em? Good stuff helps ya to relax the body; of "
              "course, leavin' behind the cooldown you suffers after the effect "
              "is gone. Bad stuff, however, it's like a violent punch in your "
              "pancreas.");
    FillTopic(npc[NPC_JEFFERSON], 1, "Dolls",
              "They're everywhere, bro. In every street. They know 'bout "
              "absolutely everything inside this district!");
    FillTopic(npc[NPC_JEFFERSON], 2, "Firearms",
              "If ya wanna some guns to brighten your night up, talks with "
              "|Ronaldo. He has a lot of stuff to show ya.");
    FillTopic(npc[NPC_JEFFERSON], 3, "Bettingshop",
              "Alright. I know one very close o' here. Just go down the slum "
              "and if ya "
              "were seeing some hot lights tearing up the sky, you're there.");

    /*
      ID 1, “Clowngirl”.
      Samira, also known as “Clowngirl”, comes from a circus family that
      perfomed in the northeaestern region many years ago. She is funny and
      crazy, and she claiming that a man with a wide hat and inexpressive
      sunglasses is constantly following her. She likes to entertain opharned
      children weekly, while struggling with depression.
    */
    npc[NPC_CLOWNGIRL] = CreateNpc("Clowngirl", 2);
    npc[NPC_CLOWNGIRL]->portrait_id =
        al_load_bitmap("portraits/clowngirl_portrait.png");
    FillIntro(npc[NPC_CLOWNGIRL], "Helloooooooo, guys!");
    FillTopic(npc[NPC_CLOWNGIRL], 0, "Who are you?",
              "Who am I? Do ya really want to ask for that? I'm a clown, dumb!");
    FillTopic(npc[NPC_CLOWNGIRL], 1, "Funny",
              "Yeah, yeah... I'm fuckin' funny. Very, very funny!");

    /*
      ID 2, “Raquel”.
      Raquel is the best call girl (aka doll) in the southern district. She is a
      trans woman who loves sex. Some clients call her nymphomaniac. However,
      she doesn't only work to please depraved people. She's usually paid to
      kill specific targets while she's with them. The opportune kis of death.
      She has cybernetic razors beneath her fake nails.
    */
    npc[NPC_RAQUEL]              = CreateNpc("Raquel", 1);
    npc[NPC_RAQUEL]->portrait_id = al_load_bitmap("portraits/raquel_portrait.png");
    FillIntro(npc[NPC_RAQUEL],
              "Nah-ah, babe. You'll not have me riding on ya. Take a "
              "walk and finds another doll to play with.");
    FillTopic(npc[NPC_RAQUEL], 0, "Doll",
              "They can be all you want to play with, babe. A tall man, a young "
              "woman... After that, you delight of their bodies like never did "
              "before. Of course, for a |price. Always it has a price.");

    /*
      ID 3, “Cinder”.
      Cinder run nearly all the dolls in the metroplex. She is wild and
      audacious, testing the government's limits to take back the control of her
      district that is dominated by privated forces. She also has a big appeal
      for men that has an iron fist to retain what is yours.
    */
    npc[NPC_CINDER]              = CreateNpc("Cinder", 1);
    npc[NPC_CINDER]->portrait_id = al_load_bitmap("portraits/cinder_portrait.png");
    FillIntro(npc[NPC_CINDER],
              "Hi, confuse man. Are you here to stare me like a erect "
              "perv’ or you'll tell something? C'mon. I won't bite you.");
    FillTopic(npc[NPC_CINDER], 0, "Bettingshop",
              "That cursed place? The |kingdom |of |cash. I hang out with a |corp "
              "once; he was disgusting, honestly. Anybody who goes there hardly can "
              "keep the bank account filled. If you're wanna step on there, take "
              "care.");

    /*
       ID 4, “Cheeks”.
       Big fan of rock ‘n’ roll, Cheeks is a good vibes singer who throw curses
       at the actual metroplex's government. Crowns of followers see him as a
       choisen one to free everyone of the megacorps' chains.
    */
    npc[NPC_CHEEKS] = CreateNpc("Cheeks", 1);
    npc[NPC_CHEEKS]->portrait_id =
        al_load_bitmap("portraits/bigcheeks_portrait.png");
    FillIntro(npc[NPC_CHEEKS], "What's uuuuup, my friend! How can I help ya today?");
    FillTopic(
        npc[NPC_CHEEKS], 0, "Nightclub",
        "There are a lot of over there. Follow your sixth sense, dude, follow "
        "it to the end of the horizon. The sun takes time to be up on the iron "
        "color sky. Iron sky... Wow, it could gimme an awesome song name. "
        "Anyway, I can't help ya now, my friend.");

    /*
      ID 5, “Banger the Killer”.
      COMMENTARY: He is a boss and one of the great villain's goons.
    */
    npc[NPC_BANGER_THE_KILLER] = CreateNpc("Banger the Killer", 2);
    npc[NPC_BANGER_THE_KILLER]->portrait_id =
        al_load_bitmap("portraits/latavelha_portrait.png");
    FillIntro(npc[NPC_BANGER_THE_KILLER], "Prepare to die, mother fucker!");
    FillTopic(npc[NPC_BANGER_THE_KILLER], 0, "Victims",
              "No one could run from me neither from my heavy, chromed arms. "
              "Prepare to cry like a little bitch 'til your blood dryin' out!");
    FillTopic(npc[NPC_BANGER_THE_KILLER], 1, "Boss",
              "Where's my boss? I... I cannot remember. He had only gave me an "
              "order: kill that gunman. For my boss I could do anything! Even "
              "terminating a rat like ya!!! ");

    npc[NPC_NICE_CORP] = CreateNpc("Nice Corp", 0);
    npc[NPC_NICE_CORP]->portrait_id =
        al_load_bitmap("portraits/nice_corpguy_face.png");
    FillIntro(npc[NPC_NICE_CORP], "I do not have cash for you, sir. Go away.");

    npc[NPC_ANAHI]              = CreateNpc("Anahi", 0);
    npc[NPC_ANAHI]->portrait_id = al_load_bitmap("portraits/anahi_face.png");
    FillIntro(npc[NPC_ANAHI], "Hi. How can I help you, dear soul?");

    npc[NPC_BOZZO]              = CreateNpc("Bozzo", 0);
    npc[NPC_BOZZO]->portrait_id = al_load_bitmap("portraits/bozzo_face.png");
    FillIntro(npc[NPC_BOZZO], "You shouldn't be here...");
}

//==========================================================================
//
//    UnlockExtraTopics
//
//    Argument: void
//    Return:   void
//
//    NOTE: this function allows the player to unlock new topics (as seen in both
//    GetTopicID and LearnTopic functions) and their respective dialogues.
//
//==========================================================================

void UnlockExtraTopics()
{
    // NOTE: each block of code below is so deep because to avoid declaring a lot of
    // pointers at the same time

    {
        NPC *jeff = npc[NPC_JEFFERSON];
        if (learned_topics[TOPIC_RONALDO]) {
            jeff->topics =
                realloc(jeff->topics, sizeof(Topic) * (jeff->num_topic + 1));
            FillTopic(jeff, jeff->num_topic, "Ronaldo",
                      "Just get the hell o' here!");
            jeff->num_topic++; // adds early number of topics by 1
        }
    }

    {
        NPC *raquel = npc[NPC_RAQUEL];
        if (learned_topics[TOPIC_PRICE]) {
            raquel->topics =
                realloc(raquel->topics, sizeof(Topic) * (raquel->num_topic + 1));
            FillTopic(raquel, raquel->num_topic, "Price",
                      "Gimme 500 reais and the brunette girl here will give "
                      "you the best "
                      "night o' your life.");
            raquel->num_topic++;
        }
    }
}

// void DescStorage(DescriptionObj *desc[])
// {
//     FillDesc(desc[0], "A lone bus stop.");
// }
