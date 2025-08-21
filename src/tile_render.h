#ifndef TILE_RENDER_H
#define TILE_RENDER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <tmx.h>

#define LINE_THICKNESS 2.5f

void *AllegTexLoader(const char *path);
void AllegTexFree(void *ptr);
ALLEGRO_COLOR IntToAllegColor(int color);
void DrawImgLayer(tmx_image *image);
void DrawTile(void *image, unsigned sx, unsigned sy, unsigned sw, unsigned sh,
              unsigned dx, unsigned dy, float opacity, unsigned flags);
void DrawTileLayer(tmx_map *map, tmx_layer *layer);
void DrawPolyline(double **points, double x, double y, int count,
                  ALLEGRO_COLOR color);
void DrawPolygon(double **points, double x, double y, int count,
                 ALLEGRO_COLOR color);
void DrawObjects(tmx_object_group *objgr);
void DrawAllLayers(tmx_map *map, tmx_layer *layers);
void RenderMap(tmx_map *m);

#endif
