# FILES TREE

A map to travel through my own game files.

```
CyberSP_Project/
├── sprites
│   └── bandit_sprites
├── npc_portraits
│   └── random_npcs
├── tiles
├── src/
│   ├── bitmap.c        # sprites domain
│   ├── bitmap.h
│   ├── collision.c     # walls & triggers logic
│   ├── collision.h
│   ├── main.c          # game loop
|   ├── main.h
│   ├── tile_render.c   # libTMX map render
│   └── tile_render.h
├── Makefile
└── README.md
```
# Features List
**First feature**: finding out an adequated way to make an isometric movement. Update: the feature successful ran.
**Second feature**: loading sprite sheets from .txt files. (Discarted.)
> **Update**: it went wrong due to complications to read each line which set the specific sprite in the sprite sheet.
**Third feature**: collision. I'll make two rectangles (invisible if possible) to draw the collision size.
> **Update**: the collision reader worked perfectly, but the wall (the next feature) is putting my brains in trouble.
> **Update 2**: the wall collision worked exactly like I'd thought... of course, after a lot of hours of code!
> Update 3: After struggling to reach a more complex collision system, I've ended up in a new perspective to this project: it's all detailed inside "collision.c".
**Fourth feature**: making a cool and practical tilemap.
> **Update**: the map for test was 100% loaded and flipped. Now I'll have to put the objects on.
> **Update 2**: map render is working well, though the map objects aren't flipping appropriately. Maybe I'll need to remake another tilemap to solve that.
**Fifth feature**: a dialogue box.
> **Update**: it ran stupidly well!
> **Update 2**: now the dialogue box disappears when the text is over.

_First Feature Update - Beginning of July 2025._
_Last Feature Update - July 19, 2025._
