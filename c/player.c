#include "../h/player.h"
#include <stdlib.h>

Player* Player_load(SDL_Renderer* renderer)
{

  const u32 SPD = 32;
  const u32 SPR_OX = 8;
  const u32 SPR_OY = 27;
  const u32 SPR_W = 16;
  const u32 SPR_H = 28;
  //settings

  Player* r = malloc(sizeof(Player)); //create player
  if(!r)
  {
    fprintf(stderr,"ERROR: Failed to allocate memory for player.\n");
    return NULL;
  }
  
  r->x = 0;
  r->y = 0;

  r->spd = SPD;

  r->spr = Sprite_load("a/sprites/elucia.png",renderer);
  if(!r->spr)
  {
    fprintf(stderr,"ERROR: Failed to create sprite for player.\n");
    return NULL;
  }

  r->spr->ox = SPR_OX;
  r->spr->oy = SPR_OY;

  r->spr->src.w = SPR_W;
  r->spr->src.h = SPR_H;
  //apply settings
  
  //pos set by Game_make in "c/game.c"
  r->t=0;
  r->face = DOWN;
  r->state = IDLE;
  //initalize values

  r->stepSFX = Mix_LoadWAV("a/sfx/step.ogg");
  r->doorSFX = Mix_LoadWAV("a/sfx/door.ogg");
  //load sfx

  return r;
}

u0 Player_free(Player* p)
{
  if(!p) return; //NULL safety
  Sprite_free(p->spr);
  Mix_FreeChunk(p->stepSFX);
  Mix_FreeChunk(p->doorSFX);
  free(p);
  //free all
}
