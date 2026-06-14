#ifndef PLAYER_H
#define PLAYER_H

#include "typedefs.h"
#include "cam.h" //for rendering
#include "sprites.h" //player sprite
#include <SDL2/SDL_mixer.h> //needed for

enum Face //face enum !!!i tried using enums, but i think iprefer macros
{
  DOWN,
  LEFT,
  UP,
  RIGHT,

  FACES
};
  
enum PlayerState //statete enum
{
  IDLE,
  MOVE,

  STATES
};

typedef struct //player class, rpg-like movement
{
  f32 x,y; //position
  i8 vx,vy; //velocity, but really just the movement direction
  Sprite* spr; //sprite

  u32 t; //frames elapsed, used do track progress of movementa nimation
  u32 spd; //frames it should teake for the player to move one tile

  enum Face face; //face
  enum PlayerState state; //state

  Mix_Chunk* stepSFX; //footstep sound effect
  Mix_Chunk* doorSFX; //door interaction sound effect

}
Player; 

Player* Player_load(SDL_Renderer* renderer); //constructor
u0 Player_free(Player* p); //destructor
//player update is handeld inside a method of Scene, Scene_playerUpdate, see "c/scenes.c"

#endif
