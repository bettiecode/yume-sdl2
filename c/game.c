#include "../h/game.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

Game* Game_make()
{
    Game* g = malloc(sizeof(Game)); //allocate memory
    if(!g)
    {
      fprintf(stderr,"ERROR: Failed to allocate memory for game object.\n"); //log error
      return NULL; //send failiure
    }
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) //init sdl
    {
      fprintf(stderr,"ERROR: Failed to initialize SDL2 - %s\n",SDL_GetError());
      free(g);
      return NULL;
    }
    if(IMG_Init(IMG_INIT_PNG)!=IMG_INIT_PNG) //init sdl image
    {
      fprintf(stderr,"ERROR: Failed to initialize SDL2_image - %s\n",SDL_GetError());
      SDL_Quit();
      free(g);
      return NULL;
    }

    if(TTF_Init()) //init sdl ttf
    {
      fprintf(stderr,"ERROR: Failed to initialize SDL2_ttf - %s\n",SDL_GetError());
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }

    if(Mix_Init(MIX_INIT_OGG)!=MIX_INIT_OGG) //init sdl mixer
    {
      fprintf(stderr,"ERROR: Failed to initialize SDL2_mixer - %s\n",SDL_GetError());
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }

    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT ,2 ,4096); //set default audio settings
    Mix_MasterVolume(VOLUME); //set default volume


    g->window = SDL_CreateWindow(WINDOW_TITLE, 
				 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				 WINDOW_WIDTH, WINDOW_HEIGHT, 0
				 ); //create window
    if(!g->window)
    {
      fprintf(stderr,"ERROR: Failed to initialize SDL2 window - %s\n",SDL_GetError());
      Mix_Quit();
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }

    g->renderer = SDL_CreateRenderer(g->window,-1,0); //create renderer
    if(!g->renderer)
    {
      fprintf(stderr,"ERROR: Failed to initialize SDL2 renderer - %s\n",SDL_GetError());
      SDL_DestroyWindow(g->window);
      Mix_Quit();
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }

    SDL_SetRenderDrawBlendMode(g->renderer,SDL_BLENDMODE_BLEND); //set render draw mode to blend, to allow opaque overlays

    Game_loadKeybinds(g); //load keybinds

    TTF_Font* font = TTF_OpenFont(FONT_FILE,FONT_HEIGHT); //load font
    if(!font)
    {
      fprintf(stderr,"ERROR: Failed to load font (%s) - %s\n",FONT_FILE,SDL_GetError());
      SDL_DestroyRenderer(g->renderer);
      SDL_DestroyWindow(g->window);
      Mix_Quit();
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }
    const SDL_Color fontCol = {255,255,255,255}; //font color
    SDL_Surface* temp = TTF_RenderText_Blended(font,UNICODE_ALL,fontCol); //render font
    if(!temp)
    {
      fprintf(stderr,"ERROR: Failed to make rendered font surface - %s\n",SDL_GetError());
      TTF_CloseFont(font);
      SDL_DestroyRenderer(g->renderer);
      SDL_DestroyWindow(g->window);
      Mix_Quit();
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }
    g->fontTex = SDL_CreateTextureFromSurface(g->renderer,temp); //make texture
    if(!g->fontTex)
    {
      fprintf(stderr,"ERROR: Failed to make font texture - %s\n",SDL_GetError());
      SDL_FreeSurface(temp);
      TTF_CloseFont(font);
      SDL_DestroyRenderer(g->renderer);
      SDL_DestroyWindow(g->window);
      Mix_Quit();
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }

    SDL_FreeSurface(temp); //clean up temp surface
    TTF_CloseFont(font); //clean up font, sicne it is no longer needed


    g->isRunning = false; //initally game isnt running


    g->p = Player_load(g->renderer); //load player
    if(!g->p)
    {
      fprintf(stderr,"ERROR: Failed to load player.");
      SDL_DestroyRenderer(g->renderer);
      SDL_DestroyWindow(g->window);
      Mix_Quit();
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }
    g->p->x=P_START_X;
    g->p->y=P_START_Y;

    g->sc = Scene_load("a/scenes/grass.dat",g->p,g->renderer); //load inital scene
    if(!g->sc)
    {
      fprintf(stderr,"ERROR: Failed to load scene.");
      Player_free(g->p);
      SDL_DestroyRenderer(g->renderer);
      SDL_DestroyWindow(g->window);
      Mix_Quit();
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
      free(g);
      return NULL;
    }
    g->state = G_ACTIVE; //state is initially active
    g->t = 0; //t is initally 0

    return g; //return pointer to object once everything intialized successfully
}   

u0 Game_run(Game* g)
{
  g->isRunning = true; //dignal running
  while(g->isRunning) //stop once the bool is set to false
  {
    Game_getInput(g); //log input
    switch(g->state) //state machine to choose between different gamestates
    {
      case G_ACTIVE:
        Game_update(g); //update game
        Game_checkSwap(g); //check if a scen swap should happen
      break;
      case G_TRANSITION:
        Game_transition(g); //progress animation
      break;
    }
    Game_render(g); //render in all states

    SDL_Delay(1000/TARGET_FPS); //delay to mathc fps
    //!!! no accurate timer implemented
  }
  Game_free(g); //destruct game after it has finished running
}
u0 Game_update(Game* g)
{
    Scene_update(g->sc,g->kbState,&g->keybinds); //update scene
}
u0 Game_render(Game* g) //render everything
{
  SDL_Color bgC = g->sc->bgColor; //clear colro is defined inside the scene
  SDL_SetRenderDrawColor(g->renderer,bgC.r,bgC.g,bgC.b,bgC.a); //set
  SDL_RenderClear(g->renderer); //clear
  Scene_render(g->sc); //render scene
  Game_renderBgmTitle(g); //render bgm title
  if(g->state==G_TRANSITION) //render transition animation
  {
    const SDL_Rect dest = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT}; //rect covering the whole window
    SDL_SetRenderDrawColor(g->renderer,0,0,0,255*(1-fabs(((f32)G_MAX_T/2-g->t)/((f32)G_MAX_T/2))));
    //black with its opacity fading out a=255*(1-|(T/2-t)/(T/2)|) !!!perhaps this formula could be simplifeid but the compiler probably does that anyways
    SDL_RenderFillRect(g->renderer,&dest); //draw overlay

  }

  SDL_RenderPresent(g->renderer); //finish rendering
}
u0 Game_free(Game* g)
{
    if(!g) return;
    //LIFO, last loaded, first unloaded
    Scene_free(g->sc);  
    Player_free(g->p);
     
    SDL_DestroyTexture(g->fontTex);

    SDL_DestroyRenderer(g->renderer);
    SDL_DestroyWindow(g->window);

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    free(g);
}

u0 Game_checkSwap(Game* g)
{
  if(!g->sc->swapTo) return; //if NULL then no swap
  g->state = G_TRANSITION; //otherwise, transition, depending on the dat inside the door object that swapTo is pointing to

}

u0 Game_swapScene(Game* g)
{
  ch* scFns[3]; //array containing constant strings, to find filenames by idx

  scFns[0] = "a/scenes/snow.dat";
  scFns[1] = "a/scenes/grass.dat";
  scFns[2] = "a/scenes/chess.dat";

  Door* d = g->sc->swapTo; //to shorten code
  g->p->x = d->px; //set player x
  g->p->y = d->py; //set player y
  g->p->face = DOWN; //set player face
  Scene* nx = Scene_load(scFns[d->scIdx], g->p, g->renderer); //load next scene
  Scene_free(g->sc);
  if(!nx) //if failed
  {
    fprintf(stderr,"ERROR: Failed to swap scenes.\n");
    exit(2);
  }
  g->sc = nx; //assign next scene
  Scene_update(g->sc,g->kbState,&g->keybinds); //update once to make player face changes visible
}

u0 Game_transition(Game* g)
{
  u8 vol = VOLUME*fabs(((f32)G_MAX_T/2-g->t)/((f32)G_MAX_T/2)); //fade vcolume out
  //v=V*|(T/2-t)/(T/2)|
  Mix_VolumeMusic(vol); //update volume
  if(g->t==G_MAX_T/2)
  {
    Game_swapScene(g); //swap at halfway point
  }
  if(g->t==G_MAX_T) //end transition
  {
    g->state = G_ACTIVE; //switch state
    g->t = 0; //rest timer
  }
  g->t++; //inceremnt timer
}


u0 Game_loadKeybinds(Game* g)
{
  g->keybinds.movUp = SDL_SCANCODE_W;
  g->keybinds.movDown = SDL_SCANCODE_S;
  g->keybinds.movLeft = SDL_SCANCODE_A;
  g->keybinds.movRight = SDL_SCANCODE_D;

  g->keybinds.interact = SDL_SCANCODE_SPACE;
  //assing keybinds
  //!!! I didnt find a better way to do this
  
  g->kbState = SDL_GetKeyboardState(NULL); //get pointer
  //!!!compiler warns here, but it deosn cause any issues, and i couldnt find a way around this
}
u0 Game_getInput(Game* g)
{
  while(SDL_PollEvent(&g->event)) //loop through events
  {
    switch(g->event.type) //event types
    {
      case SDL_QUIT: //window closed
        g->isRunning = false; //stop
      break;
      case SDL_KEYDOWN: //keypressed
        switch(g->event.key.keysym.scancode)
        {
          case SDL_SCANCODE_ESCAPE: //if escape
            g->isRunning = false; //stop
          break;
          default:
          break;
        }
      break;
      default: break;
    }
  }
  SDL_PumpEvents(); //get events again !!! im not sure if there's a way to get the keyboard state and poll events with only one pump'
  SDL_GetKeyboardState(NULL); //update keyboard state
}

u0 Game_renderBgmTitle(Game* g)
{
  const u32 padding = 4; //padding
  const f32 scale = 1; //scale
  SDL_Rect src = {0,0,FONT_WIDTH,FONT_HEIGHT}; //area to snip
  SDL_Rect dest = {padding,padding,FONT_WIDTH*scale,FONT_HEIGHT*scale}; //where to place
  u32 i=0; //start at 0
  while(g->sc->bgmTitle[i]!='\0') //loop through chars on the string
  {
    src.x = (g->sc->bgmTitle[i]-'!')*FONT_WIDTH; //find snip x
    SDL_SetRenderDrawColor(g->renderer,0,0,0,100); //set color of text's background'
    SDL_RenderFillRect(g->renderer,&dest); //draw char bacground
    SDL_RenderCopy(g->renderer,g->fontTex,&src,&dest); //draw char
    dest.x+=FONT_WIDTH*scale; //move to next's place'
    i++; //move to next char
  }
}
