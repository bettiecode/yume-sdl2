#include "../h/scenes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

Scene* Scene_load(ch* filename, Player* p, SDL_Renderer* renderer)
{ 
  //see file structure in "a/scenes/chess.dat"
  Scene* r = malloc(sizeof(Scene)); //allocate memory
  if(!r)
  {
    fprintf(stderr,"ERROR: Failed to allocate memory for scene (%s).\n",filename);
    return NULL;
  }
  
  r->renderer = renderer; //passed in renderer, persists between scenes

  FILE* f = fopen(filename,"r"); //open file to read
  ch line[64]; //line buffer

  fgets(line,64,f);
  r->w = atoi(line);
  fgets(line,64,f);
  r->h = atoi(line);
  //get scene sizes

  fgets(line,64,f);
  r->cam.zoom = atof(line);
  //get zoom

  fgets(line,64,f);
  line[strcspn(line,"\n ")]='\0';
  r->bgm = Mix_LoadMUS(line);
  //get bgm


  fgets(r->bgmTitle,64,f);
  r->bgmTitle[strcspn(r->bgmTitle,"\n")]='\0';
  //get bgm title

  fgets(line,64,f);
  r->playerZ = atoi(line);
  //get playerZ
  r->player = p; //keep player

  r->cam.w = WINDOW_WIDTH/r->cam.zoom;
  r->cam.h = WINDOW_HEIGHT/r->cam.zoom;
  //assign cam sizes

  fgets(line,64,f);
  r->bgColor.r = atoi(line);
  fgets(line,64,f);
  r->bgColor.g = atoi(line);
  fgets(line,64,f);
  r->bgColor.b = atoi(line); 
  r->bgColor.a = 255;
  //get bg color

  fgets(line,64,f);
  line[strcspn(line,"\n ")]='\0';
  r->ts = Tileset_load(line,r->renderer);
  //get tileset

  fgets(line,64,f);
  r->layerC = atoi(line);
  //get layerC
  r->layers = malloc(sizeof(Layer)*r->layerC); //allocate memory
  if(!r->layers)
  {
    fprintf(stderr,"ERROR: Failed to allocate memory for scene's layers (%s).\n",filename);
    return NULL;
  }
  for(u32 i=0; i<r->layerC; i++) //load each layer
  {
    fgets(line,64,f);
    r->layers[i].type = atoi(line);
    //get type
    fgets(line,64,f);
    line[strcspn(line,"\n ")]='\0';
    //get file with data
    switch(r->layers[i].type)
    {
      case LAYER_TILEMAP:
        r->layers[i].dat = Tilemap_load(line,r->ts);
      break;
      case LAYER_MARKMAP:
        r->layers[i].dat = Markmap_load(line);
      break;
      case LAYER_OBJS:
        r->layers[i].dat = ObjLayer_load(line,r->renderer,(i==r->playerZ) ? r->player : NULL);
      break;
      case LAYER_BACKDROP:
        r->layers[i].dat = Backdrop_load(line,r->renderer);
      break;
    }
    if(!r->layers[i].dat)
    {
      fprintf(stderr,"ERROR: Failed to allocate memory for scene's layer %u (%s).\n",i,line);
      return NULL;
    }
  }

  fgets(line,64,f);
  r->doorC = atoi(line); 
  //get doorC
  r->doors = malloc(sizeof(Door)*r->doorC); //get doors
  if(!r->doors)
  {
    fprintf(stderr,"ERROR: Failed to allocate memory for scene's doors (%s).\n",filename);
    return NULL;
  }
  for(u32 i=0; i<r->doorC; i++) //load doors
  {
    fgets(line,64,f);
    r->doors[i].px = atoi(line);
    //get px
    fgets(line,64,f);
    r->doors[i].py = atoi(line);
    fgets(line,64,f);
    //get py
    r->doors[i].pFace = atoi(line);
    fgets(line,64,f);
    //get face
    r->doors[i].scIdx = atoi(line);
    //get the scene it is leading ot
  }
  r->swapTo = NULL; //initally null

  fclose(f); //close file
  Mix_PlayMusic(r->bgm,-1); //start music
  return r; //return successfully
}

u0 Scene_free(Scene* sc)
{
    if(!sc) return; //so it is safe to call it with NULL passed in
    Mix_FreeMusic(sc->bgm); //stop and unload bg music
    Tileset_free(sc->ts); //free tileset
    for(u32 i=0; i<sc->layerC; i++) //free each layer
    {
      switch(sc->layers[i].type)
      {
        case LAYER_TILEMAP:
          Tilemap_free(sc->layers[i].dat);
        break;
        case LAYER_MARKMAP:
          Markmap_free(sc->layers[i].dat);
        break;
        case LAYER_OBJS:
          ObjLayer_free(sc->layers[i].dat);
        break;
        case LAYER_BACKDROP:
          Backdrop_free(sc->layers[i].dat);
        break;
      }
    }
    free(sc->layers);
    free(sc->doors);
    //free arrays
    free(sc);
    //free scene itself
}

u0 Scene_update(Scene* sc, u8* keys, Actions* kb)
{
  for(u32 i=0; i<sc->layerC; i++) //update each layer
  {    
    switch(sc->layers[i].type)
    {
      case LAYER_OBJS: //only obj layers are changing
        ObjLayer_update(sc->layers[i].dat,sc->layers[sc->layerC-1].dat,sc->ts,sc->w,sc->h); //updatae objlayer
        if(i == sc->playerZ) Scene_playerUpdate(sc, keys, kb); //update player
      break;
      default:
      break;
    }
  }

  Scene_camUpdate(sc); //update cam at the end
}


u0 Scene_playerUpdate(Scene* sc, u8* keys, Actions* kb)
{   
  u8 inpWeight; //number of movement keys down
  u32 tx,ty; //tile x, tile y
  Markmap* mm; //markmap with colliders
  
  Player* p = sc->player; //to shorten

  switch(p->state) //state machine
  {
    case IDLE: //idle state
      p->vx = keys[kb->movRight]-keys[kb->movLeft]; //x spd
      p->vy = keys[kb->movDown]-keys[kb->movUp]; //y spd
      inpWeight = abs(p->vx)+abs(p->vy); //count weight
      if(p->vx!=0 || p->vy!=0)
      {
        p->face = (inpWeight==2)*(2*(0.5-p->vy*0.5))+
                  (inpWeight==1)*((p->vx!=0)*(1+2*(0.5-p->vx*0.5))+
				  (p->vy!=0)*(  2*(0.5-p->vy*0.5)));
        //calculate face from input

        p->vx=p->vx*(p->vy==0); //prioritize y movement if both are pressed

        mm = sc->layers[sc->layerC-1].dat; //get markmap

        tx = (i32)(p->x/(signed)sc->ts->tW+p->vx+(signed)mm->w)%mm->w;
        ty = (i32)(p->y/(signed)sc->ts->tH+p->vy+(signed)mm->h)%mm->h;
        //get tile coords of next tile the player will be on
        if(mm->markers[ty*mm->w+tx]>MARK_WALL) //if its not a wall
        {
          if(keys[kb->movDown] && mm->markers[ty*mm->w+tx]>MARK_FREE) break;
          //make it so you cant walk through doors the other way
          tx = (i32)(p->x/(signed)sc->ts->tW+(signed)mm->w)%mm->w;
          ty = (i32)(p->y/(signed)sc->ts->tH+(signed)mm->h)%mm->h;
          //get current tile coords
          if((keys[kb->movUp]) && mm->markers[ty*mm->w+tx]>MARK_FREE) //if moving up into a door
          {
            sc->swapTo = &sc->doors[mm->markers[ty*mm->w+tx]-2]; //swap to door's scene
            Mix_PlayChannel(-1,p->doorSFX,0); //play sfx
            break;
          }
          else
          {
            p->state = MOVE; //move
            Mix_PlayChannel(-1,p->stepSFX,0); //play sfx
          };
        }
        else
        {
          break; //if its a wall then dont start moving
        }
      }
      else
        break;
    case MOVE: //move
      p->x+=p->vx*(float)sc->ts->tW/p->spd; //move x
      p->y+=p->vy*(float)sc->ts->tH/p->spd; //move y
      p->t++; //incerement timer
      if(p->t>=p->spd) //if deistance is tracversed, meaning the timer has reached the max
      {
        p->t=0; //reset timet
        p->x=round(p->x);
        p->y=round(p->y);
        //round pos, to account for float imprescion
        p->x+= ((p->x<0)-(p->x>sc->w))*sc->w;
        p->y+= ((p->y<0)-(p->y>sc->h))*sc->h;
        //loop player if off the scene's edge
        p->state = IDLE; //switch back
      };
    break;
    default:
    break;
  }

  p->spr->src.x = floor(4.0*p->t/p->spd)*p->spr->src.w;
  p->spr->src.y = p->face*p->spr->src.h;
  //get correct frame of player's animation'
}

u0 Scene_camUpdate(Scene* sc)
{
  sc->cam.x = sc->player->x-sc->cam.w/2;
  sc->cam.y = sc->player->y-sc->cam.h/2-7; //-7 to not have the player's feet in the center'
  //update cam pos

  sc->cam.ali=
  (
    (((sc->cam.y+sc->cam.h)>sc->h)-(sc->cam.y<0)+1)*3
    +
    (((sc->cam.x+sc->cam.w)>sc->w)-(sc->cam.x<0)+1)
  );
  //update cam aligment
}

u0 Scene_render(Scene* sc)
{  
  for(u32 i=0; i<sc->layerC-1; i++) //render each layer
  {
    switch(sc->layers[i].type)
    {
      case LAYER_TILEMAP:
          Tilemap_render(sc->layers[i].dat,sc->ts,sc->renderer,&sc->cam);
      break;
      case LAYER_MARKMAP:
          Markmap_render(sc->layers[i].dat,sc->ts,sc->renderer,&sc->cam);
      break;
      case LAYER_OBJS:
	      ObjLayer_render(sc->layers[i].dat,sc->renderer,&sc->cam,sc->w,sc->h);
      break;
      case LAYER_BACKDROP:
          Backdrop_render(sc->layers[i].dat,sc->renderer,&sc->cam,sc->w,sc->h);
      break;
    }
  }
}
