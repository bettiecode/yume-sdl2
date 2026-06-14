#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

typedef struct //struct to store which scancode to chekc for which action
{
  SDL_Scancode movUp;
  SDL_Scancode movDown;
  SDL_Scancode movLeft;
  SDL_Scancode movRight;

  SDL_Scancode interact;
}
Actions;

#endif
