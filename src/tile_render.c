//**************************************************************************
//**
//** File: tile_render.c (CyberSP Project)
//** Purpose: Tiled Map Editor render
//** Last Update: 17-07-2025
//** Author: DDeyTS
//**
//**************************************************************************

#include "tile_render.h"

/*
      This entire code is clipped from libTMX tutorial found in their
      website (link below).
      https://libtmx.readthedocs.io/en/latest/renderer-from-scratch.html
*/

//==========================================================================
//
//    *AllegTexLoader
//
//    Callback to load image through the Allegro.
//
//==========================================================================

void *AllegTexLoader(const char *path)
{
    // printf("[AllegTexLoader] Tentando carregar imagem: %s\n", path);
    ALLEGRO_PATH *alpath = al_create_path(path);
    if (!alpath) return NULL;
    al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
    ALLEGRO_BITMAP *bmp =
        al_load_bitmap(al_path_cstr(alpath, ALLEGRO_NATIVE_PATH_SEP));
    al_destroy_path(alpath);
    return (void *)bmp;
}

// void *AllegTexLoader(const char *path) {
//   char fixed_path[256];
//   snprintf(fixed_path, sizeof(fixed_path), "tiles/%s", path);
//   printf("Carregando imagem: [%s]\n", fixed_path);
//   ALLEGRO_BITMAP *bmp = al_load_bitmap(fixed_path);
//   if (!bmp) fprintf(stderr, "Erro ao carregar imagem: %s\n", fixed_path);
//   return bmp;
// }

//==========================================================================
//
//    AllegTexFree
//
//    Callback to free texture.
//==========================================================================

void AllegTexFree(void *ptr)
{
    al_destroy_bitmap((ALLEGRO_BITMAP *)ptr);
}

//==========================================================================
//
//    InToAllegColor
//
//    Converts Tiled color to Allegro color.
//==========================================================================

ALLEGRO_COLOR IntToAllegColor(int color)
{
    tmx_col_floats f = tmx_col_to_floats(color);
    // return *((ALLEGRO_COLOR *)&f);
    return al_map_rgba_f(f.r, f.g, f.b, f.a);
}

//==========================================================================
//
//    DrawImgLayer
//
//    Draws a full-screen image layer.
//==========================================================================

void DrawImgLayer(tmx_image *image)
{
    ALLEGRO_BITMAP *bmp = (ALLEGRO_BITMAP *)image->resource_image;
    if (bmp) al_draw_bitmap(bmp, 0, 0, 0);
}

//==========================================================================
//
//    DrawTile
//
//    Draws a specific tile.
//
//==========================================================================

void DrawTile(void *image, unsigned sx, unsigned sy, unsigned sw, unsigned sh,
              unsigned dx, unsigned dy, float opacity, unsigned flags)
{
    ALLEGRO_COLOR tint = al_map_rgba_f(opacity, opacity, opacity, opacity);
    al_draw_tinted_bitmap_region((ALLEGRO_BITMAP *)image, tint, sx, sy, sw, sh, dx,
                                 dy, flags);
}

//==========================================================================
//
//    DrawTileLayer
//
//==========================================================================

void DrawTileLayer(tmx_map *map, tmx_layer *layer)
{
    unsigned mapw = map->width, maph = map->height;
    for (unsigned i = 0; i < maph; i++) {
        for (unsigned j = 0; j < mapw; j++) {
            unsigned long index = i * mapw + j;
            unsigned gid_raw    = layer->content.gids[index];
            unsigned gid        = gid_raw & TMX_FLIP_BITS_REMOVAL;
            if (map->tiles[gid]) {
                tmx_tileset *ts = map->tiles[gid]->tileset;
                tmx_image *im   = map->tiles[gid]->image;
                void *img   = im ? im->resource_image : ts->image->resource_image;
                unsigned sx = map->tiles[gid]->ul_x, sy = map->tiles[gid]->ul_y;
                unsigned sw = ts->tile_width, sh = ts->tile_height;
                float op       = layer->opacity;
                unsigned flags = gid_raw & ~TMX_FLIP_BITS_REMOVAL;
                DrawTile(img, sx, sy, sw, sh, j * sw, i * sh, op, flags);
            }
        }
    }
}

//==========================================================================
//
//    DrawPolyLine
//
//    Draws lines/poligons of objects.
//
//==========================================================================

void DrawPolyline(double **points, double x, double y, int count,
                  ALLEGRO_COLOR color)
{
    for (int i = 1; i < count; i++) {
        al_draw_line(x + points[i - 1][0], y + points[i - 1][1], x + points[i][0],
                     y + points[i][1], color, LINE_THICKNESS);
    }
}

//==========================================================================
//
//    DrawPolygon
//
//==========================================================================

void DrawPolygon(double **points, double x, double y, int count, ALLEGRO_COLOR color)
{
    DrawPolyline(points, x, y, count, color);
    if (count > 2)
        al_draw_line(x + points[0][0], y + points[0][1], x + points[count - 1][0],
                     y + points[count - 1][1], color, LINE_THICKNESS);
}

//==========================================================================
//
//    DrawObjects
//
//==========================================================================

void DrawObjects(tmx_object_group *objgr)
{
    ALLEGRO_COLOR color = IntToAllegColor(objgr->color);
    tmx_object *obj     = objgr->head;
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
                al_draw_ellipse(obj->x + obj->width / 2.0,
                                obj->y + obj->height / 2.0, obj->width / 2.0,
                                obj->height / 2.0, color, LINE_THICKNESS);
                break;
            default:
                break;
            }
        }
        obj = obj->next;
    }
}

//==========================================================================
//
//    DrawAllLayers
//
//==========================================================================

void DrawAllLayers(tmx_map *map, tmx_layer *layers)
{
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

//==========================================================================
//
//    RenderMap
//
//    Put all the game map inside the buffer.
//
//==========================================================================

void RenderMap(tmx_map *m)
{
    al_clear_to_color(IntToAllegColor(m->backgroundcolor));
    DrawAllLayers(m, m->ly_head);
}
