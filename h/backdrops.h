#include <SDL2/SDL.h> //needed for rendering
#include <SDL2/SDL_image.h> //needed for texture loading
#include "typedefs.h"
#include "cam.h"

typedef struct //backdrop class
{
    u32 w,h; //size, to crop from image
    f32 plx; //parallacx factor, only those backdrops can parallax, which belong to scenes where the player cant loop around, if it can, it should always be 1
    SDL_Texture* tex; //texture
}
Backdrop;

Backdrop* Backdrop_load(ch* filename, SDL_Renderer* renderer); //constructor, method to load from file
u0 Backdrop_free(Backdrop* bd); //destructor
u0 Backdrop_renderOffset(Backdrop* bd, f32 x, f32 y, SDL_Renderer* renderer, Cam* cam);  //method to called by render method to draw it looping
u0 Backdrop_render(Backdrop* bd, SDL_Renderer* renderer, Cam* cam, f32 scW, f32 scH); //method to render backdrop
