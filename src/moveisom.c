/*

 _______   _______ _________________ _   _ _   _  _   __
/  __ \ \ / / ___ \  ___| ___ \ ___ \ | | | \ | || | / /
| /  \/\ V /| |_/ / |__ | |_/ / |_/ / | | |  \| || |/ /
| |     \ / | ___ \  __||    /|  __/| | | | . ` ||    \
| \__/\ | | | |_/ / |___| |\ \| |   | |_| | |\  || |\  \
 \____/ \_/ \____/\____/\_| \_\_|    \___/\_| \_/\_| \_/
 _____   ___  ___  ___ _____
|  __ \ / _ \ |  \/  ||  ___|
| |  \// /_\ \| .  . || |__
| | __ |  _  || |\/| ||  __|
| |_\ \| | | || |  | || |___
 \____/\_| |_/\_|  |_/\____/

      First feature: finding out an adequated way
      to make an isometric movement.
          Update: the feature successful ran.

      Second feature: loading sprite sheets
      from .txt files.
          Update: it went wrong due to complications
          to read each line which set the specific
          sprite in the sprite sheet. (Discarted.)

      Third feature: collision. I'll make two
      rectangles (invisible if possible) to draw
      the collision size.
          Update: the collision reader worked
          perfectly, but the wall (the next feature)
          is putting my brains in trouble.
          Update 2: the wall collision worked exactly
          like I'd thought... of course, after a lot of
          hours of code!
          Update 2: After struggling to reach a more
          complex collision system, I've ended up in
          a new perspective to this project: it's all
          detailed inside the Collision Sys description
          (from line 86).
      Fourth feature: making a cool and practical tilemap.

      Written by DDey - Beginning of July 2025.
      Expanded & Edited - July 13, 2025.
*/
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
// #define MAX_VERTICES 3
// #define DEG2RAD(deg) ((deg)*ALLEGRO_PI / 180.0f)

// float vx[MAX_VERTICES] = {200, 250, 300};
// float vy[MAX_VERTICES] = {200, 300, 200};
static int disp_w = 640, disp_h = 400;

// ======================================================
//
//     TYPEDEFS
//
//     SpriteSheetInfo struct stores the entities'
//     attributes to load them and let them moving
//     around as well as changing frames. It also
//     has the bitmaps' pointers of all game sprites.
//
// ======================================================
typedef struct {
  // Bandit Attributes
  int frame_w, frame_h, rows, cols;
  float px, py;
  // Rectangle Attributes
  int rw, rh, rx, ry;
  // Circle Attributes
  float cx, cy, ray;
  // Sprites
  ALLEGRO_BITMAP *sheet, *bandit;
} SpriteSheetInfo;
SpriteSheetInfo spr;
SpriteSheetInfo ent; // entity

// ======================================================
//
//     COLLISION SYS
//
//     RectSqColl() creates either rectangular or square
//     collision. It's useful to wall collisions, too.
//     CircleReaderColl() focus on circular collison.
//
//     About abandoned functions:
//       My personal objective to make an isometric game
//       as a first project is far away due to my program-
//       ming skills. Then I'm following my brother's
//       advice of working with a 2D common point-of-view.
//       It'll be simple enough to build something that I can
//       later be proud of. The focus now is acquiring quite
//       gamedev knowledge because, first of all, I'm in that
//       only for personal achieviement.
//         Well, let's build a top-down cyberpunk game with
//       a lot of CRPG inspirations.
//                                            (July 13, 2025)
//
// ======================================================
bool RectSqColl(float px, float py, int pw, int ph, float wall_x1,
                float wall_y1, int wall_x2, int wall_y2) {
  return !(px + pw <= wall_x1 ||      // left wall
           px >= wall_x1 + wall_x2 || // right wall
           py + ph <= wall_y1 ||      // upper wall
           py >= wall_y1 + wall_y2);  // lower wall
}

bool CircleColl(float cx1, float cy1, float r1, float cx2, float cy2,
                float r2) {
  float dx = cx1 - cx2; // player_x + circle_x
  float dy = cy1 - cy2; // player_y + circle_y
  float rsum = r1 + r2; // player_ray (aka frame_w) + circle_ray
  return (dx * dx + dy * dy) < (rsum * rsum);
}

// bool PolygonColl(float px, float py, float *vx, float *vy, int nverts) {
//   bool inside = false;
//
//   // It range each poligon's side (linking point i to the point j,
//   // the previous one)
//   for (int i = 0, j = nverts - 1; i < nverts; j = i++) {
//     // It check if the horizontal line which pass through px & py
//     // will intersect the poligon's i-j side
//     bool intersect =
//         ((vy[i] > py) != (vy[j] > py)) &&
//         (px < (vx[j] - vx[i]) * (py - vy[i]) / (vy[j] - vy[i]) + vx[i]);
//     // If it has intersected, invert the state (in/out)
//     if (intersect)
//       inside = !inside;
//   }
//   return inside;
// }

// bool RotatedRectColl(float px, float py, float cx, float cy, // Rectangle
// center
//                      float w, float h, // Rectangle width and height
//                      float angle_rad) {
//   // Translate the point to the rectangle's local space
//   float dx = px - cx;
//   float dy = py - cy;
//
//   // Applies reverse rotation in the point, transforming it
//   // to the rectangle's space non-rotated
//   float local_x = dx * cos(-angle_rad) - dy * sin(-angle_rad);
//   float local_y = dx * sin(-angle_rad) + dy * cos(-angle_rad);
//
//   // Check if it's inside rectangle aligned local (aka AABB)
//   return fabs(local_x) <= w / 2 && fabs(local_y) <= h / 2;
// }
//
// void DrawRotatedRect(float cx, float cy, float w, float h, float angle_rad,
//                      ALLEGRO_COLOR color) {
//   float hw = w / 2.0f;
//   float hh = h / 2.0f;
//
//   float cos_a = cos(angle_rad);
//   float sin_a = sin(angle_rad);
//
//   // Define the 4 rotated vertices around the center
//   float x1 = cx + (-hw * cos_a - -hh * sin_a);
//   float y1 = cy + (-hw * sin_a + -hh * cos_a);
//
//   float x2 = cx + (hw * cos_a - -hh * sin_a);
//   float y2 = cy + (hw * sin_a + -hh * cos_a);
//
//   float x3 = cx + (hw * cos_a - hh * sin_a);
//   float y3 = cy + (hw * sin_a + hh * cos_a);
//
//   float x4 = cx + (-hw * cos_a - hh * sin_a);
//   float y4 = cy + (-hw * sin_a + hh * cos_a);
//
//   al_draw_line(x1, y1, x2, y2, color, 2.0);
//   al_draw_line(x2, y2, x3, y3, color, 2.0);
//   al_draw_line(x3, y3, x4, y4, color, 2.0);
//   al_draw_line(x4, y4, x1, y1, color, 2.0);
// }

// ======================================================
//
//    SPRITESHEET
//
//    BitmapGrab() prepares all the bitmaps in the program.
//    BitmapInit() obviously activate them, loading the
//    spritesheet and each sprite.
//    BitmapExplode() destroys everything loaded!
//
// ======================================================
ALLEGRO_BITMAP *BitmapGrab(int x, int y, int w, int h) {
  ALLEGRO_BITMAP *sprite = al_create_sub_bitmap(spr.sheet, x, y, w, h);
  return sprite;
}

void BitmapInit() {
  spr.sheet = al_load_bitmap("bandit_still_sheet.png");
  if (!spr.sheet) {
    printf("Erro: spr.bandit não carregou\n");
  }

  spr.bandit = BitmapGrab(0, 0, 16, 24);
  if (!spr.bandit) {
    printf("Erro: BitmapGrab retornou NULL\n");
    exit(1);
  }
}

void BitmapExplode() { al_destroy_bitmap(spr.sheet); }

// ======================================================
//
//    BANDIT SPRITE & ANIMATION
//
//    BanditDraw() is obvious as fuck, right? However,
//    there's a warning: if the sprite is scaled, take
//    care of remembering to adapt the code for its new
//    size.
//    BanditMove() straight & diagonal movement logic and
//    collision interaction trigger.
//    BanditDirection() move the character toward the side
//    he/she's facing.
//
// ======================================================
void BanditDraw() {
  al_draw_scaled_bitmap(spr.bandit, spr.frame_w, spr.frame_h, 16, 24, spr.px,
                        spr.py, 32, 48, 0);
}

void BanditDirection(bool keys[], int *fx, int *fy) {
  int cols = spr.cols = 16;
  int rows = spr.rows = 24;
  spr.frame_w = *fx;
  spr.frame_h = *fy;

  // Diagonal Movement
  if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_D]) {
    // Up-right
    *fx = cols, *fy = rows * 2;
  } else if (keys[ALLEGRO_KEY_W] && keys[ALLEGRO_KEY_A]) {
    // Up-left
    *fx = cols - 16, *fy = rows * 2;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_D]) {
    // Down-right
    *fx = cols, *fy = rows;
  } else if (keys[ALLEGRO_KEY_S] && keys[ALLEGRO_KEY_A]) {
    // Down-left
    *fx = cols, *fy = cols - 16;
    // Straight Movement
  } else if (keys[ALLEGRO_KEY_D]) {
    *fx = cols - 16, *fy = rows;
  } else if (keys[ALLEGRO_KEY_A]) {
    *fx = cols - 16, *fy = rows - 24;
  } else if (keys[ALLEGRO_KEY_S]) {
    *fx = cols * 2, *fy = rows - 24;
  } else if (keys[ALLEGRO_KEY_W]) {
    *fx = cols * 2, *fy = rows;
  }
}

void BanditMove(bool keys[], float *px, float *py, float sp) {
  int dx = 0, dy = 0;
  spr.px = *px;
  spr.py = *py;

  // Pressed directions
  if (keys[ALLEGRO_KEY_D])
    dx += 1;
  if (keys[ALLEGRO_KEY_A])
    dx -= 1;
  if (keys[ALLEGRO_KEY_S])
    dy += 1;
  if (keys[ALLEGRO_KEY_W])
    dy -= 1;

  // Movement Values
  float mov_x = dx * sp, mov_y = dy * sp;
  // Adjust the speed
  if (dx != 0 && dy != 0) {
    float adj = 1 / sqrt(2); // = 0.707f
    mov_x *= adj;
    mov_y *= adj;
  }

  // Rectangle Collison
  SpriteSheetInfo next = spr;
  next.px = *px + mov_x;
  next.py = *py + mov_y;
  bool rectangle_x = RectSqColl(next.px, *py, 32, 48, 300, 100, 200, 50);
  bool rectangle_y = RectSqColl(*px, next.py, 32, 48, 300, 100, 200, 50);

  // Circle Collison
  ent.cx = 100;
  ent.cy = 150;
  ent.ray = 80;
  float spr_r = 16;
  float circle_x = next.px + 32 / 2.0;
  float circle_y = next.py + 48 / 2.0;
  bool circle = CircleColl(circle_x, circle_y, spr_r, ent.cx, ent.cy, ent.ray);

  // Collision Triggers
  if (!rectangle_x && !circle) {
    *px = next.px;
  }
  if (!rectangle_y && !circle) {
    *py = next.py;
  }
  spr.px = *px;
  spr.py = *py;
}

// ======================================================
//
//     MAIN
//
//     All initializations.
//     Display, timer and event queue.
//     Common variables.
//     Main loop.
//     Data crusher (al_destroy_...()).
//
// ======================================================
int main() {
  al_init();
  al_init_image_addon();
  al_install_keyboard();
  al_init_primitives_addon();
  BitmapInit();

  ALLEGRO_DISPLAY *disp = al_create_display(disp_w, disp_h);
  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_start_timer(timer);

  // Bandit Position
  spr.px = 320;
  spr.py = 200;
  // Frames (and the initial frame when the program begins)
  spr.frame_w = 32;
  spr.frame_h = 0;
  // Movement Speed
  int sp = 3;

  // Rectangle Position
  ent.rx = 300;
  ent.ry = 100;
  // Edges
  ent.rw = 200;
  ent.rh = 50;

  // Circle Position
  ent.cx = 100;
  ent.cy = 150;
  ent.ray = 80;

  // Loop and Bitmap Redrawing
  bool running = 1;
  bool redraw;

  bool keys[ALLEGRO_KEY_MAX];
  memset(keys, 0, sizeof(keys)); // reset array

  while (running) {
    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      keys[ev.keyboard.keycode] = true;
    } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
      keys[ev.keyboard.keycode] = false;
    }

    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
      running = 0;

    if (ev.type == ALLEGRO_EVENT_TIMER) {
      redraw = true;
    }

    BanditMove(keys, &spr.px, &spr.py, sp);
    BanditDirection(keys, &spr.frame_w, &spr.frame_h);

    if (redraw && al_is_event_queue_empty(queue)) {
      al_set_target_backbuffer(disp);
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_draw_rectangle(ent.rx, ent.ry, ent.rw + ent.rx, ent.rh + ent.ry,
                        al_map_rgb(255, 0, 0), 5.0);
      al_draw_circle(ent.cx, ent.cy, ent.ray, al_map_rgb(0, 255, 0), 5);
      BanditDraw();
      al_flip_display();
      redraw = false;
    }
  }

  al_destroy_display(disp);
  BitmapExplode();
  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  return 0;
}
