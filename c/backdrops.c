#include "../h/backdrops.h"
#include <stdio.h>

Backdrop* Backdrop_load(ch* filename, SDL_Renderer* renderer)
{
    //see backdrop file structure in "a/backdrops/grass.dat"
    Backdrop* r = malloc(sizeof(Backdrop)); //allocate memory
    if(!r)
    {
        fprintf(stderr,"ERROR: Failed to allocate memory for backdrop.\n");
        return NULL;
    }
    FILE* f = fopen(filename,"r");
    ch line[64];
    fgets(line,64,f); r->w = atoi(line);
    fgets(line,64,f); r->h = atoi(line);
    //get sizes
    fgets(line,64,f); r->plx = atof(line);
    //get plx

    fgets(line,64,f); line[strcspn(line,"\n ")]='\0'; //get texture
    SDL_Surface* temp = IMG_Load(line); //make temp surface
    if(!temp)
    {
        fprintf(stderr,"ERROR: Failed to allocate memory for backdrop's temporary surface - %s \n",SDL_GetError());
        return NULL;
    }
    r->tex = SDL_CreateTextureFromSurface(renderer,temp); //make texture
    if(!r->tex)
    {
        fprintf(stderr,"ERROR: Failed to allocate memory for backdrop's temporary surface - %s \n",SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(temp); //cleanup

    fclose(f); //close file

    return r; //return successfully
}
u0 Backdrop_free(Backdrop* bd)
{
    if(!bd) return; //NULL safety
    SDL_DestroyTexture(bd->tex); //free texture
    free(bd); //free itself
}
u0 Backdrop_renderOffset(Backdrop* bd, f32 x, f32 y, SDL_Renderer* renderer, Cam* cam)
{
    SDL_Rect src={0,0,bd->w,bd->h}; //src
    SDL_Rect dest=
    {
        (x-bd->plx*cam->x)*cam->zoom,
        (y-bd->plx*cam->y)*cam->zoom,
        bd->w*cam->zoom,
        bd->h*cam->zoom
    }; //dest

    SDL_RenderCopy(renderer,bd->tex,&src,&dest); //draw
}

u0 Backdrop_render(Backdrop* bd,SDL_Renderer* renderer, Cam* cam, f32 scW, f32 scH)
{
    switch(cam->ali) //render only the segment seen by cam in its current aligment
    {
        case 0:
            Backdrop_renderOffset(bd,-scW,-scH,renderer,cam);
            Backdrop_renderOffset(bd,0,-scH,renderer,cam);
            Backdrop_renderOffset(bd,-scW,0,renderer,cam);
            Backdrop_renderOffset(bd,0,0,renderer,cam);
        break;
        case 1:
            Backdrop_renderOffset(bd,0,-scH,renderer,cam);
            Backdrop_renderOffset(bd,0,0,renderer,cam);
        break;
        case 2:
            Backdrop_renderOffset(bd,0,-scH,renderer,cam);
            Backdrop_renderOffset(bd,scW,-scH,renderer,cam);
            Backdrop_renderOffset(bd,0,0,renderer,cam);
            Backdrop_renderOffset(bd,scW,0,renderer,cam);
        break;
        case 3:
            Backdrop_renderOffset(bd,-scW,0,renderer,cam);
            Backdrop_renderOffset(bd,0,0,renderer,cam);
        break;
        case 4:
            Backdrop_renderOffset(bd,0,0,renderer,cam);
        break;
        case 5:
            Backdrop_renderOffset(bd,0,0,renderer,cam);
            Backdrop_renderOffset(bd,scW,0,renderer,cam);
        break;
        case 6:
            Backdrop_renderOffset(bd,-scW,0,renderer,cam);
            Backdrop_renderOffset(bd,0,0,renderer,cam);
            Backdrop_renderOffset(bd,-scW,scH,renderer,cam);
            Backdrop_renderOffset(bd,0,scH,renderer,cam);
        break;
        case 7:
            Backdrop_renderOffset(bd,0,0,renderer,cam);
            Backdrop_renderOffset(bd,0,scH,renderer,cam);
        break;
        case 8:
            Backdrop_renderOffset(bd,0,0,renderer,cam);
            Backdrop_renderOffset(bd,scW,0,renderer,cam);
            Backdrop_renderOffset(bd,0,scH,renderer,cam);
            Backdrop_renderOffset(bd,scW,scH,renderer,cam);
        break;
        default:
        break;
    }
}
