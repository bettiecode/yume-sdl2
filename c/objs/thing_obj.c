#include "../../h/objs.h"
#include <stdlib.h>

ThingObj* ThingObj_load(FILE* f, u32 size, char* line, SDL_Renderer* renderer)
{
    ThingObj* r = malloc(sizeof(ThingObj)); //alloc
    if(!r)
    {
        fprintf(stderr,"Failed to allocate memory for door object.\n");
        return NULL;
    }

    fgets(line,size,f); r->x=atoi(line);
    fgets(line,size,f); r->y=atoi(line);
    //get pos

    r->state = 0;
    r->t = 0;
    r->vx = 0;
    //set initially

    r->spr = Sprite_load("a/sprites/thing.png",renderer);
    if(!r->spr)
    {
        fprintf(stderr,"Failed to create sprite for door object.\n");
        return NULL;
    }
    r->spr->oy = 16;
    r->spr->ox = 8;
    r->spr->src.x=0;
    r->spr->src.y=0;
    r->spr->src.w=16;
    r->spr->src.h=16;
    //set sprite

    return r; //return successfully
}
u0 ThingObj_free(ThingObj* tO)
{
    Sprite_free(tO->spr);
    free(tO);
    //free srite then itself
}
u0 ThingObj_update(ThingObj* tO, Markmap* mm, Tileset* ts, f32 scW)
{
    const f32 spd = 0.2; //pixels/frame
    switch(tO->state) //find state
    {
        case 0: //idle state
            tO->vx = (rand() % 2) ? -1 : 1; //choose a direction randomly

            u32 tx = (i32)(tO->x/(signed)ts->tW+tO->vx+(signed)mm->w)%mm->w;
            u32 ty = (i32)((tO->y-1)/(signed)ts->tW+       (signed)mm->h)%mm->h;
            //get coords of the tile thing will be moving onto

            if(mm->markers[ty*mm->w+tx]<1) tO->vx = 0-tO->vx; //turna round if its a wall

            tO->spr->src.y = (0.5+tO->vx*0.5)*tO->spr->src.h; //set frame to match sprite's looking direction with movement direction

            tO->state = 1; //switch to move state
        break;
        case 1: //move state
            tO->x+=tO->vx*spd; //move
            tO->t++; //incerment timer
            if(tO->t>=(ts->tW)/spd) //if done
            {
                tO->state=0; //switch to idle state
                tO->x-=(tO->x>scW)*scW;
                tO->x+=(tO->x<0)*scW;
                //loop if off edge
                tO->t = 0; //reset timer
            }
        break;
        default:
        break;
    }
}
