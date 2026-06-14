#include "../h/tiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

b8 Mat_texLoad(Mat* dest, char* filename, SDL_Renderer* renderer)
{
    SDL_Surface* temp = IMG_Load(filename); //create temp surface
    if(!temp)
    {
      fprintf(stderr,"ERROR: Failed to create temporary surface for material - %s\n",SDL_GetError());
      return false;
    }
    dest->tex = SDL_CreateTextureFromSurface(renderer,temp); //create texture
    if(!dest->tex)
    {
      fprintf(stderr,"ERROR: Failed to create texture for material - %s\n",SDL_GetError());
      return false;
    }
    SDL_FreeSurface(temp); //clean up

    return true; //return successful
}
u0 Mat_free(Mat* m)
{
  if(!m) return; //NULL safety
  SDL_DestroyTexture(m->tex); //free texture
}

Tileset* Tileset_load(char* filename, SDL_Renderer* renderer)
{
    //see "a/tilesets/chess.dat" for file structure
    Tileset* r = malloc(sizeof(Tileset)); //allocate memory
    if(!r)
    {
      fprintf(stderr,"ERROR: Failed to allocate memory for tileset.\n");
      return NULL;
    }

    FILE* f = fopen(filename,"r"); //open file to read
    char line[64]; //buffer

    fgets(line,64,f);
    r->matC = atoi(line); //get matC
    r->mats = malloc(sizeof(Mat)*r->matC); //allocate memory
    if(!r->mats)
    {
      fprintf(stderr,"ERROR: Failed to allocate memory for tileset's materials.\n");
      return NULL;
    }

    fgets(line,64,f);
    r->tW = atoi(line);

    fgets(line,64,f);
    r->tH = atoi(line);

    for(u32 i=0; i<r->matC; i++)
    {
        fgets(line,64,f);
        line[strcspn(line,"\n")]='\0';
        if(!Mat_texLoad(&r->mats[i],line,renderer))
        {
          fprintf(stderr,"ERROR: Failed to allocate memory for tileset's material (%u).\n",i);
          return NULL;
        }
        
        fgets(line,64,f);
        r->mats[i].autoTile = atoi(line);
        //get auto tile
    }

    fclose(f); //close
    return r; //return success
}
u0 Tileset_free(Tileset* ts)
{
    if(!ts) return; //NULL safety
    for(u32 i=0; i<ts->matC; i++) //free each mat
    {
        Mat_free(&ts->mats[i]); //free
    }
    free(ts->mats); //free itself
}

Tilemap* Tilemap_load(char* filename, Tileset* ts)
{
  //see tilemap file structure in "a/tilemaps/chess.dat"
  Tilemap* r = malloc(sizeof(Tilemap)); //create object
  if(!r)
  {
    fprintf(stderr,"ERROR: Failed to allocate memory for tilemap.\n");
    return NULL;
  }
  FILE* f = fopen(filename,"r"); //open to read
  ch line[16]; //buffer

  fgets(line,16,f);
  r->w = atoi(line);
  fgets(line,16,f);
  r->h = atoi(line);
  //get sizes

  r->tiles = malloc(sizeof(Tile)*r->w*r->h); //create tiles matrix
  if(!r)
  {
    fprintf(stderr,"ERROR: Failed to allocate memory for tilemap's tiles.\n");
    return NULL;
  }

  b8 done = false; //track comand reading progress
  while(!done) //read commands
  {
    fgets(line,16,f);
    ch key = line[0]; //get command
    u32 x1,y1,x2,y2,idx; //args
    switch(key)
    {
      case '!': //stop
        done = true;
      break;
      case 'f': //fill
        fgets(line,16,f);
        x1 = atoi(line);
        fgets(line,16,f);
        y1 = atoi(line);
        fgets(line,16,f);
        x2 = atoi(line);
        fgets(line,16,f);
        y2 = atoi(line);
        fgets(line,16,f);
        idx = line[0]=='A' ? ts->matC : (u32)atoi(line); //A for air
        //get args

        for(u32 y=y1; y<=y2; y++)
        {
          for(u32 x=x1; x<=x2; x++)
          {
            r->tiles[y*r->w+x].matIdx=idx; //fill
          }
        }
      break;
      case 's': //set
        fgets(line,16,f);
        x1 = atoi(line);
        fgets(line,16,f);
        y1 = atoi(line);
        fgets(line,16,f);
        idx = line[0]=='A' ? ts->matC : (u32)atoi(line);

        r->tiles[y1*r->w+x1].matIdx = idx;
      break;
      case 'r': //randomize
      	fgets(line,16,f);
        x1 = atoi(line);
        fgets(line,16,f);
        idx = atoi(line);
        for(u32 y=0; y<r->h; y++)
        {
          for(u32 x=0; x<r->w; x++)
          {
            if(rand() % x1 == 0) r->tiles[y*r->w+x].matIdx=idx; //place specifed x upon a 1/x1 chance
          }
        }
      break;
      default:
      break;
    }
  }
  fclose(f); //close
  autotile(r,ts); //autotile
  return r; //return successfully
}

u0 Tilemap_free(Tilemap* tm)
{
  if(!tm) return; //NULL safety
  free(tm->tiles); //free tiles
  free(tm); //free itself
}

u0 Tilemap_render(Tilemap* tm, Tileset* ts, SDL_Renderer* renderer, Cam* cam) //HERE
{
  //logic is same as Markmap_render in "c/markmap.c"
  SDL_Rect dest = {0,0,ts->tW*cam->zoom,ts->tH*cam->zoom}; //dest

  i32 sx = cam->x/ts->tW-1;
  i32 sy = cam->y/ts->tH-1;

  for(i32 ry=sy; ry<(i32)(tm->h+sy); ry++)
  {
    for(i32 rx=sx; rx<(i32)(tm->w+sx); rx++)
    {
      i32 x = ((rx+tm->w)%tm->w);
      i32 y = ((ry+tm->h)%tm->h);
      //printf("%d - %d\n",rx,ry);
      u32 idx = tm->tiles[y*tm->w+x].matIdx; //get matIdx
      if(idx == ts->matC) continue;
      dest.x = ((rx)*(i32)ts->tW-cam->x)*cam->zoom;
      dest.y = ((ry)*(i32)ts->tH-cam->y)*cam->zoom;
      SDL_RenderCopy
      (
        renderer,
        ts->mats[idx].tex,
        &tm->tiles[y*tm->w+x].src,
        &dest
      );
      //draw
    }
  }
}
u0 Tilemap_print(Tilemap* tm) //old, not sure if it still works
{
  for(u32 y=0; y<tm->h; y++)
  {
    for(u32 x=0; x<tm->w; x++)
    {
      printf("%d-%d ",(int)tm->tiles[y*tm->w+x].src.x,(int)tm->tiles[y*tm->w+x].src.y);
    }
    printf("\n");
  }
}

u0 autotile(Tilemap*tm,Tileset* ts)
{
  SDL_Rect rect = {0,0,ts->tW,ts->tH}; //src rect ehich position will be determined
  for(u32 y=0; y<tm->h; y++) //for each row
  {
    for(u32 x=0; x<tm->w; x++) // for each coloumn
    {
      if(tm->tiles[y*tm->w+x].matIdx == ts->matC) continue; //if air, then skip
      if(!ts->mats[tm->tiles[y*tm->w+x].matIdx].autoTile) //if it no auto tiling, then set to deafulat src and skip
      {
        tm->tiles[y*tm->w+x].src = rect;
        continue;
      }
      u8 nb = 0; //byte storing 8 bools for the 8 neighbors
      const i8 dx[8] = { 0, 1, 0,-1,-1, 1, 1,-1};
      const i8 dy[8] = {-1, 0, 1, 0,-1,-1, 1, 1};
      //delta coords, to make looping around and checking neighbors easier
      for(u8 i = 0; i<8; i++) //loop around
      {
        u8 other = 0; //is neigbor different
        u32 rx = (x+dx[i]+tm->w)%tm->w;
        u32 ry = (y+dy[i]+tm->h)%tm->h;
        //coords
        other = (tm->tiles[ry*tm->w+rx].matIdx!=tm->tiles[y*tm->w+x].matIdx); //get if different
        nb = nb | (other<<i); //write resulting bit to bit i of byte
      }

      u8 s = (nb%16); //sides
      u8 c = nb-s; //corners

      const u8 p[16] ={ //bool arrays saying which corners should be counted for each combination of sides
      15, //0:  1111
      12, //1:  1100
      9 , //2:  1001
      8 , //3:  1000
      3 , //4:  0011
      0 , //5:  0000
      1 , //6:  0001
      0 , //7:  0000
      6 , //8:  0110
      4 , //9:  0100
      0 , //10: 0000
      0 , //11: 0000
      2 , //12: 0010
      0 , //13: 0000
      0 , //14: 0000
      0   //15: 0000
      }; 

      u8 n =  s+(c & p[s]<<4); //formula to get number of the tile variant

      switch(n) //map number of variant to position on texture
      {
        case 9  : rect.x=0; rect.y=0; break;
        case 1  : rect.x=1; rect.y=0; break;
        case 3  : rect.x=2; rect.y=0; break;
        case 11 : rect.x=3; rect.y=0; break;
        case 64 : rect.x=4; rect.y=0; break;
        case 128: rect.x=5; rect.y=0; break;
        case 144: rect.x=6; rect.y=0; break;
        case 8  : rect.x=0; rect.y=1; break;
        case 0  : rect.x=1; rect.y=1; break;
        case 2  : rect.x=2; rect.y=1; break;
        case 10 : rect.x=3; rect.y=1; break;
        case 32 : rect.x=4; rect.y=1; break;
        case 16 : rect.x=5; rect.y=1; break;
        case 96 : rect.x=6; rect.y=1; break;
        case 12 : rect.x=0; rect.y=2; break;
        case 4  : rect.x=1; rect.y=2; break;
        case 6  : rect.x=2; rect.y=2; break;
        case 14 : rect.x=3; rect.y=2; break;
        case 176: rect.x=4; rect.y=2; break;
        case 48 : rect.x=5; rect.y=2; break;
        case 112: rect.x=6; rect.y=2; break;
        case 13 : rect.x=0; rect.y=3; break;
        case 5  : rect.x=1; rect.y=3; break;
        case 7  : rect.x=2; rect.y=3; break;
        case 15 : rect.x=3; rect.y=3; break;
        case 208: rect.x=4; rect.y=3; break;
        case 192: rect.x=5; rect.y=3; break;
        case 224: rect.x=6; rect.y=3; break;
        case 72 : rect.x=0; rect.y=4; break;
        case 130: rect.x=1; rect.y=4; break;
        case 65 : rect.x=2; rect.y=4; break;
        case 125: rect.x=3; rect.y=4; break;
        case 73 : rect.x=4; rect.y=4; break;
        case 193: rect.x=5; rect.y=4; break;
        case 131: rect.x=6; rect.y=4; break;
        case 40 : rect.x=0; rect.y=5; break;
        case 18 : rect.x=1; rect.y=5; break;
        case 36 : rect.x=2; rect.y=5; break;
        case 20 : rect.x=3; rect.y=5; break;
        case 104: rect.x=4; rect.y=5; break;
        case 240: rect.x=5; rect.y=5; break;
        case 146: rect.x=6; rect.y=5; break;
		//   42 ; //empty
		//   43 ; //empty
        case 160: rect.x=2; rect.y=6; break;
        case 80 : rect.x=3; rect.y=6; break;
        case 44 : rect.x=4; rect.y=6; break;
        case 52 : rect.x=5; rect.y=6; break;
        case 22 : rect.x=6; rect.y=6; break;
        case 129: rect.x=3; rect.y=4; break;
		
        default : rect.x=0; rect.y=6; printf("Could not find tile orientation at %u-%u: %u\n",x,y,n); break; //error tile
      }
      rect.x*=ts->tW;
      rect.y*=ts->tH;
      //cscale coords
      tm->tiles[y*tm->w+x].src = rect;
      //set src to calculated rect
    }
  } 
}
