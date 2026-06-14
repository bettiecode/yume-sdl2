#ifndef SPRITES_H
#define SPRITES_H

#include "typedefs.h"
#include <SDL2/SDL.h> //needed for rendering
#include <SDL2/SDL_image.h> //needed for textures
#include "cam.h" //needed for rendering

typedef struct //sprite class
{
  SDL_Texture* sheet; //spritesheet, all frames laid aout in a varying order
  SDL_Rect src; //src, current frame's rectangle on the sheet'
  i32 ox; //origin x
  i32 oy; //origin y
}
Sprite;

Sprite* Sprite_load(char* filename,SDL_Renderer* renderer); //constructor
u0 Sprite_free(Sprite* spr); //destructor

u0 Sprite_render(Sprite* spr, f32 x, f32 y,SDL_Renderer* renderer, Cam* cam); //method to render

#endif
