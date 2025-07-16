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
        Update: the map for test was 100% loaded and flipped.
        Now I'll have to put the objects on.
        Update 2: map render is working well, though the map
        objects aren't flipping appropriately. Maybe I'll need
        to remake another tilemap to solve that.

      Written by DDey - Beginning of July 2025.
      Expanded & Edited - July 16, 2025.
*/
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/keyboard.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <tmx.h>
#define LINE_THICKNESS 2.5f
static int disp_w = 640, disp_h = 400;
static tmx_map *map = NULL;

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
//     TILED MAP SYS
//     The big part of this code is clipped from libTMX's
//     example given in their website, so the descriptions
//     ahead are poor and simple enoug just to explain as
//     short as possible.
//
//     AllegTexLoader() callback to load image through
//     the lib.
//     AllegTexFree() callback to free texture.
//
//     IntToAllegColor() convert Tiled color to Allegro.
//
//     DrawImgLayer() draw a full-screen image layer.
//     DrawTile() draw a specific one.
//     DrawTileLayer() is pretty obvious, isn't it?
//     DrawPolyline() draw lines/polygons of objects.
//     DrawPolygon() is obvious as fuck.
//     DrawObjects() is another from obvious' family.
//     DrawAllLayers() is obvious, too.
//
//     RenderMap() put all this stuff inside the buffer.
//
// ======================================================
void *AllegTexLoader(const char *path) {
  ALLEGRO_PATH *alpath = al_create_path(path);
  if (!alpath)
    return NULL;
  al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
  ALLEGRO_BITMAP *bmp =
      al_load_bitmap(al_path_cstr(alpath, ALLEGRO_NATIVE_PATH_SEP));
  al_destroy_path(alpath);
  return (void *)bmp;
}
void AllegTexFree(void *ptr) { al_destroy_bitmap((ALLEGRO_BITMAP *)ptr); }

ALLEGRO_COLOR IntToAllegColor(int color) {
  tmx_col_floats f = tmx_col_to_floats(color);
  return *((ALLEGRO_COLOR *)&f);
}

void DrawImgLayer(tmx_image *image) {
  ALLEGRO_BITMAP *bmp = (ALLEGRO_BITMAP *)image->resource_image;
  if (bmp)
    al_draw_bitmap(bmp, 0, 0, 0);
}

void DrawTile(void *image, unsigned sx, unsigned sy, unsigned sw, unsigned sh,
              unsigned dx, unsigned dy, float opacity, unsigned flags) {
  ALLEGRO_COLOR tint = al_map_rgba_f(opacity, opacity, opacity, opacity);
  al_draw_tinted_bitmap_region((ALLEGRO_BITMAP *)image, tint, sx, sy, sw, sh,
                               dx, dy, flags);
}

void DrawTileLayer(tmx_map *map, tmx_layer *layer) {
  unsigned mapw = map->width, maph = map->height;
  for (unsigned i = 0; i < maph; i++) {
    for (unsigned j = 0; j < mapw; j++) {
      unsigned long index = i * mapw + j;
      unsigned gid_raw = layer->content.gids[index];
      unsigned gid = gid_raw & TMX_FLIP_BITS_REMOVAL;
      if (map->tiles[gid]) {
        tmx_tileset *ts = map->tiles[gid]->tileset;
        tmx_image *im = map->tiles[gid]->image;
        void *img = im ? im->resource_image : ts->image->resource_image;
        unsigned sx = map->tiles[gid]->ul_x, sy = map->tiles[gid]->ul_y;
        unsigned sw = ts->tile_width, sh = ts->tile_height;
        float op = layer->opacity;
        unsigned flags = gid_raw & ~TMX_FLIP_BITS_REMOVAL;
        DrawTile(img, sx, sy, sw, sh, j * sw, i * sh, op, flags);
      }
    }
  }
}

void DrawPolyline(double **points, double x, double y, int count,
                  ALLEGRO_COLOR color) {
  for (int i = 1; i < count; i++) {
    al_draw_line(x + points[i - 1][0], y + points[i - 1][1], x + points[i][0],
                 y + points[i][1], color, LINE_THICKNESS);
  }
}
void DrawPolygon(double **points, double x, double y, int count,
                 ALLEGRO_COLOR color) {
  DrawPolyline(points, x, y, count, color);
  if (count > 2)
    al_draw_line(x + points[0][0], y + points[0][1], x + points[count - 1][0],
                 y + points[count - 1][1], color, LINE_THICKNESS);
}

void DrawObjects(tmx_object_group *objgr) {
  ALLEGRO_COLOR color = IntToAllegColor(objgr->color);
  tmx_object *obj = objgr->head;
  while (obj) {
    if (obj->visible) {
      switch (obj->obj_type) {
      case OT_SQUARE:
        al_draw_rectangle(obj->x, obj->y, obj->x + obj->width,
                          obj->y + obj->height, color, LINE_THICKNESS);
        break;
      case OT_POLYGON:
        DrawPolygon(obj->content.shape->points, obj->x, obj->y,
                    obj->content.shape->points_len, color);
        break;
      case OT_POLYLINE:
        DrawPolyline(obj->content.shape->points, obj->x, obj->y,
                     obj->content.shape->points_len, color);
        break;
      case OT_ELLIPSE:
        al_draw_ellipse(obj->x + obj->width / 2.0, obj->y + obj->height / 2.0,
                        obj->width / 2.0, obj->height / 2.0, color,
                        LINE_THICKNESS);
        break;
      default:
        break;
      }
    }
    obj = obj->next;
  }
}

void DrawAllLayers(tmx_map *map, tmx_layer *layers) {
  while (layers) {
    if (layers->visible) {
      if (layers->type == L_GROUP) {
        DrawAllLayers(map, layers->content.group_head);
      } else if (layers->type == L_IMAGE) {
        DrawImgLayer(layers->content.image);
      } else if (layers->type == L_LAYER) {
        DrawTileLayer(map, layers);
      } else if (layers->type == L_OBJGR) {
        DrawObjects(layers->content.objgr);
      }
    }
    layers = layers->next;
  }
}

void RenderMap(tmx_map *m) {
  al_clear_to_color(IntToAllegColor(m->backgroundcolor));
  DrawAllLayers(m, m->ly_head);
}

// ======================================================
//
//     COLLISION SYS
//
//     RectSqColl() creates either rectangular or square
//     collision. It's useful to wall collisions, too.
//
//     CircleReaderColl() focus on circular collison.
//
//     About absence of functions for polygonal collision:
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

// ======================================================
//
//    SPRITESHEET
//
//    BitmapGrab() prepares all the bitmaps in the program.
//
//    BitmapInit() obviously activate them, loading the
//    spritesheet and each sprite.
//
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
//
//    BanditMove() straight & diagonal movement logic and
//    collision interaction trigger.
//
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
int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Uso: %s arquivo_map.tmx\n", argv[0]);
    return 1;
  }
  if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon() ||
      !al_install_keyboard()) {
    fprintf(stderr, "Falha ao inicializar Allegro\n");
    return 1;
  }
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

    // define callbacks para libTMX
    tmx_img_load_func = AllegTexLoader;
    tmx_img_free_func = AllegTexFree;

    map = tmx_load(argv[1]);
    if (!map) {
      tmx_perror("Cannot load map");
      return 1;
    }

    BanditMove(keys, &spr.px, &spr.py, sp);
    BanditDirection(keys, &spr.frame_w, &spr.frame_h);

    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    if (redraw && al_is_event_queue_empty(queue)) {
      al_set_target_backbuffer(disp);
      al_clear_to_color(al_map_rgb(0, 0, 0));
      RenderMap(map);
      al_draw_rectangle(ent.rx, ent.ry, ent.rw + ent.rx, ent.rh + ent.ry,
                        al_map_rgb(255, 0, 0), 5.0);
      al_draw_circle(ent.cx, ent.cy, ent.ray, al_map_rgb(0, 255, 0), 5);
      BanditDraw();
      al_flip_display();
      redraw = false;
    }
  }

  al_destroy_display(disp);
  tmx_map_free(map);
  BitmapExplode();
  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  return 0;
}
