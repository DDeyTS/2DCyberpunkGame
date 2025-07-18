//**************************************************************************
//**
//** File: collision.c (CyberSP Project)
//** Purpose: Sprite interaction logic (triggers, walls)
//** Last Update: 17-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "collision.h"

/*
      Radical change!

      My personal objective to develop an isometric game
      as a first project is far away due to my programming
      skills. Then I'm following my brother's advice of 
      working with a 2D common point-of-view. It'll be simple
      enough to build something that I can later be proud of.
      The focus now is acquiring quite gamedev knowledge because,
      first of all, I'm in that only for personal achieviement.
        Well, let's build a top-down cyberpunk game with
      a lot of CRPG inspirations!
                                            (July 13, 2025)
*/

//==========================================================================
//
//    RectSqColl
//
//    Creates either rectangular or square collision, useful to wall 
//    collisions.
//
//==========================================================================

bool RectSqColl(float px, float py, int pw, int ph, float wall_x1,
                float wall_y1, int wall_x2, int wall_y2) {
  return !(px + pw <= wall_x1 ||      // left wall
           px >= wall_x1 + wall_x2 || // right wall
           py + ph <= wall_y1 ||      // upper wall
           py >= wall_y1 + wall_y2);  // lower wall
}


//==========================================================================
//
//    CircleColl
//
//    The same above but focused on circular collision.
//
//==========================================================================

bool CircleColl(float cx1, float cy1, float r1, float cx2, float cy2,
                float r2) {
  float dx = cx1 - cx2; // player_x + circle_x
  float dy = cy1 - cy2; // player_y + circle_y
  float rsum = r1 + r2; // player_ray (aka frame_w) + circle_ray
  return (dx * dx + dy * dy) < (rsum * rsum);
}
