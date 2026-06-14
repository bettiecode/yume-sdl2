#include "../h/markmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

Markmap* Markmap_load(char* filename)
{
  //see markmap file structure in "a/markmaps/chess.dat"
  Markmap* r = malloc(sizeof(Markmap)); //allocate memory
  if(!r)
  {
    fprintf(stderr,"ERROR: Failed to allaocte memory for markmap.\n");
    return NULL;
  }

  FILE* f = fopen(filename,"r"); //open file to read
  ch line[16]; //buffer

  fgets(line,16,f);
  r->w = atoi(line);
  fgets(line,16,f);
  r->h = atoi(line);
  //get sizes

  r->markers = malloc(sizeof(u32)*r->w*r->h); //allocate memory
  if(!r->markers)
  {
    fprintf(stderr,"ERROR: failed to allocate memory for markmap.\n");
    return NULL;
  }


  b8 done = false; //bool to store command reading process
  while(!done) // read commands
  {
    fgets(line,16,f); //read line
    ch key = line[0]; //command key is the first character
    u32 x1,y1,x2,y2,val; //command arguments
    switch(key)
    {
      case '!': //end of commands
        done = true;
      break;
      case 'f': //fill rectangle
        fgets(line,16,f);
        x1 = atoi(line);
        fgets(line,16,f);
        y1 = atoi(line);
        fgets(line,16,f);
        x2 = atoi(line);
        fgets(line,16,f);
        y2 = atoi(line);
        fgets(line,16,f);
        val = atoi(line);
        //get args
        for(u32 y=y1; y<=y2; y++)
        {
          for(u32 x=x1; x<=x2; x++)
          {
            r->markers[y*r->w+x]=val; //fill with val
          }
        }
      break;
      case 's': //set one tile
        fgets(line,16,f);
        x1 = atoi(line);
        fgets(line,16,f);
        y1 = atoi(line);
        fgets(line,16,f);
        val = atoi(line);
        //get args
        r->markers[y1*r->w+x1] = val; //set to val
      break;
    }
  }
  fclose(f);
  return r; 
}

u0 Markmap_free(Markmap* mm)
{
  if(!mm) return; //so its safe if NULL
  free(mm->markers); //free
}

u0 Markmap_render(Markmap* mm, Tileset* ts, SDL_Renderer* renderer, Cam* cam) //HERE
{
  SDL_Rect dest = {0,0,ts->tW*cam->zoom,ts->tH*cam->zoom}; //dest

  const SDL_Color colors[3] =
  {
    {0,0,255,100}, //wall
    {0,0,0,100}, //free
    {0,255,0,100} //door
  }; //marker colors

  i32 sx = cam->x/ts->tW-1;
  i32 sy = cam->y/ts->tH-1;
  //camera's top left's tile coordinates are where whe start

  for(i32 ry=sy; ry<(i32)(mm->h+sy); ry++) //real y
  {
    for(i32 rx=sx; rx<(i32)(mm->w+sx); rx++) //real x
    {
      i32 x = ((rx+mm->w)%mm->w); //x on tilemap
      i32 y = ((ry+mm->h)%mm->h); //y on tilemap
      u32 val = mm->markers[y*mm->w+x]; //value of marker
      dest.x = ((rx)*(i32)ts->tW-cam->x)*cam->zoom;
      dest.y = ((ry)*(i32)ts->tH-cam->y)*cam->zoom;
      //render position
      
      SDL_Color c = val < 3 ? colors[val] : colors[2]; //get color

      SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a); //set color
      SDL_RenderDrawRect(renderer,&dest); //draw
      

    }
  }
}


