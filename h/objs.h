#ifndef OBJS_H
#define OBJS_H

#include "typedefs.h"
#include "cam.h" //needed for rendering
#include "player.h" //needed as player object
#include <SDL2/SDL.h> //needed for rendering
#include "markmap.h" //needed for collison and other interactions

typedef struct //objlayer class
{
  u32* types; //array for obj types
  u0** items; //array for pointers to each object
  u32* drawOrder; //array holding draw order, or NULL if depth sorting isnt enabled for the layer

  u32 itemC; //length of arrays
}
ObjLayer;

ObjLayer* ObjLayer_load(char* filename, SDL_Renderer* renderer, Player* p); //constructor
u0 ObjLayer_free(ObjLayer* ol); //destructor

u0 ObjLayer_update(ObjLayer* ol, Markmap* mm, Tileset* ts, f32 scW, f32 scH); //method to update, move objs

u0 ObjLayer_renderObjsOffset(ObjLayer* ol, SDL_Renderer* renderer, Cam* cam, i32 offX, i32 offY); //method to rener elements of the array
u0 ObjLayer_render(ObjLayer* ol, SDL_Renderer* renderer, Cam* cam,  f32 scW, f32 scH); //method collecting togehter all the rendering functions needed to get all visible objects on screen

u0 ObjLayer_depthSort(ObjLayer* ol); //method to sort objs by their y

f32 ObjLayer_getY(ObjLayer* ol, u32 i); //method to get y of a ceratain obj

#define OBJ_PLAYER 0 //player type

#include "objs/door_obj.h" //door obj
#define OBJ_DOOR 1 //door type
#include "objs/thing_obj.h" //thing obj
#define OBJ_THING 2 //thing type

#endif
