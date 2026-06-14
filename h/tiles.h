#ifndef TILES_H
#define TILES_H

#include "typedefs.h"
#include <SDL2/SDL.h> //needed for rendering
#include <SDL2/SDL_image.h> //needed for textures
#include "cam.h"

typedef struct //tile struct
{
    u32 matIdx; //material of  tile, =matC if air
    SDL_Rect src; //src on mat's texture, which type of tile it is, its orientation'
}
Tile;

typedef struct //material class
{
    SDL_Texture* tex; //texture all tile orientations
    b8 autoTile; //if the tile auto tiles or not
}
Mat;

typedef struct //tileset class
{
    u32 matC; //array length of...
    Mat* mats; //... materails array
    u32 tW, tH; //tile sizes
}
Tileset;

typedef struct //tilemap class
{
    Tile* tiles; //tiles matrix
    u32 w,h; //matrix sizes
}
Tilemap;

b8 Mat_texLoad(Mat* dest, char* filename, SDL_Renderer* renderer); //method to load texture of a material
u0 Mat_free(Mat* m); //method to unload a material

Tileset* Tileset_load(char* filename, SDL_Renderer* renderer); //methos to laod tielset from file, constructor
u0 Tileset_free(Tileset* ts); //destructor

Tilemap* Tilemap_load(char* filename, Tileset* ts); //contructor. method to load from file
u0 Tilemap_free(Tilemap* tm); //destructor
u0 Tilemap_print(Tilemap* tm); //method to print for debugging
u0 Tilemap_render(Tilemap* tm, Tileset* ts, SDL_Renderer* renderer, Cam* cam); //method to draw

u0 autotile(Tilemap* tm, Tileset*ts); //autotile function
#endif
