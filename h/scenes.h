#ifndef SCENES_H
#define SCENES_H

#include "typedefs.h" //typedefs
#include "tiles.h" //tilemaps, tilesets
#include "player.h" //player object
#include "markmap.h" //markmaps
#include "doors.h" //doors
#include "input.h" //input reading
#include "objs.h" //objlayers
#include "backdrops.h" //backdrops

#include <SDL2/SDL.h> //needed for renderer access
#include <SDL2/SDL_mixer.h> //needed toplay music and sfx

#define LAYER_TILEMAP 0
#define LAYER_MARKMAP 1
#define LAYER_OBJS 2
#define LAYER_BACKDROP 3
//layer type macros

typedef struct //layer, item of a polymorphic array
{
   u8 type; //layer type, to ecide how to read the data...
   u0* dat; //...that this is pointing to
}
Layer;

typedef struct //scene class, game's world'
{
    SDL_Renderer* renderer; //game classes rendere put here for easier access

    Mix_Music* bgm; //background music
    char bgmTitle[80];//buffer fro bgm title !!!i hate using char[<const>] for strings

    SDL_Color bgColor; //background color
    u32 layerC; //amount of layers
    Layer* layers; //polymorhic array storing the layers

    Tileset* ts; //tileset tilemaps use

    Cam cam; //camera, that views the scene
    
    i32 w,h; //width and height of scene of scene
    
    Player* player; //scene's player, put here so it's input can affect swapTo and the camera more easily
    u32 playerZ; //the layer on which the player is, this one must be an obj layer

    Door* doors; //doors to other scenes inside the scene
    u32 doorC; //their amount

    Door* swapTo; //swap inidicator
}
Scene;

Scene* Scene_load(ch* filename,Player* p,SDL_Renderer* renderer); //method to load scene from a dat file, keeping the player and the renderer, constructor
u0 Scene_free(Scene* sc); //destructor

u0 Scene_update(Scene* sc,u8* keys, Actions* kb); //method to update scene
u0 Scene_playerUpdate(Scene* sc, u8* keys, Actions* kb); //method to update the player, and other things accroding to the input
u0 Scene_render(Scene* sc); //method to render scene

u0 Scene_camUpdate(Scene* sc); //method to update camera

#endif
