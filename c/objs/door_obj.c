#include "../../h/objs.h"


DoorObj* DoorObj_load(FILE* f, u32 size, char* line, SDL_Renderer* renderer)
{
    DoorObj* r = malloc(sizeof(DoorObj));
    if(!r)
    {
        fprintf(stderr,"Failed to allocate memory for door object.\n");
        return NULL;
    }
    fgets(line,size,f);
    line[strcspn(line,"\n ")]='\0';
    //get texture's filename'
    r->spr = Sprite_load(line,renderer);
    if(!r->spr)
    {
        fprintf(stderr,"Failed to create sprite for door obj.\n");
        return NULL;
    }
    const SDL_Rect src = {0,0,16,32}; //const because sprite isnt animated !!!although animated sprites for doors could be implemented later
    r->spr->src = src;
    r->spr->ox = 8;
    r->spr->oy = 32;
    //set sprite

    fgets(line,size,f);
    r->x = atoi(line);
    fgets(line,size,f);
    r->y = atoi(line);
    //get pos
    return r; //return successfully
}
u0 DoorObj_free(DoorObj* dO)
{
    Sprite_free(dO->spr); //free texture
    free(dO); //free sprite
}
