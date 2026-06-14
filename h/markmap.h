#ifndef MARKMAP_H
#define MARKMAP_H

#include "typedefs.h"
#include "cam.h" //needed for rendering
#include "tiles.h" //needed for tile size
#include <SDL2/SDL.h>

#define MARK_WALL 0 //marker holding collider
#define MARK_FREE 1 //marker holding free
                    //markers above one hold door indices

typedef struct //markmap class
{
  u32* markers; //matrix of markers
  u32 w, h; //sizes of matrix
}
Markmap;

Markmap* Markmap_load(ch* filename); //constructor, method to load from file
u0 Markmap_free(Markmap* mm); //destructor
u0 Markmap_render(Markmap* mm, Tileset* ts, SDL_Renderer* renderer, Cam* cam); //method to render, used for debugging

#endif
