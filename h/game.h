#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h> //needed for window and rendering, also audio
#include "typedefs.h" //typedefs
#include "scenes.h" //scenes
#include "input.h" //input system

#define WINDOW_TITLE "Yume SDL2 v0" //window title macro
#define TARGET_FPS 60 //target fps macro

#define G_ACTIVE 0 //active game state macro
#define G_TRANSITION 1 //transition game state macro
#define G_MAX_T 240 //amount of transition frames macro

#define VOLUME 60 //default audio volume

#define UNICODE_ALL "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
//string containing all printable characters, for text rendering

#define FONT_WIDTH 9
#define FONT_HEIGHT 16
//font size macros, for text rendering

#define FONT_FILE "a/fonts/ModernDOS9x16.ttf" //used font file macro

#define P_START_X 8+4*16
#define P_START_Y 8+3*16+7
//player start position macros

typedef struct //game class
{
  SDL_Window* window; //window, that the game needs
  SDL_Renderer* renderer; //rnderer to draw on the window
  SDL_Event event; //event storage
  
  SDL_Texture* fontTex; //texture containing renderable characters

  b8 isRunning; //bool to control when the game should stop

  Scene* sc; //current scene to draw and control
  Player* p; //pointer to player stored here, to make it persist between scenes
  u8 state; //gamestate
  u32 t; //frames elapsed since transition start

  Actions keybinds; //keybinds to control game
  u8* kbState; //state of the keyboard
}
Game;

Game* Game_make(); //game constructor
u0 Game_run(Game* g); //method to run game
u0 Game_update(Game* g); //method to update each frame
u0 Game_render(Game* g); //method to render after update
u0 Game_free(Game* g); //destructor

u0 Game_swapScene(Game* g); //method to switch to a new scene
u0 Game_checkSwap(Game* g); //method to check if the game should swap to a new scene
u0 Game_transition(Game* g); //progress the visual transition

u0 Game_loadKeybinds(Game* g); //method to load keybinds
u0 Game_getInput(Game* g); //method to get keybinds

u0 Game_renderBgmTitle(Game* g); //method to render the title of the scene's background music'
#endif
