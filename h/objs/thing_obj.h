#include "../typedefs.h"
#include "../sprites.h"
#include "../markmap.h"

typedef struct //thing class
{
    f32 x,y; //pos
    Sprite* spr; //sprite
    u8 state; //state
    u8 t; //frames elapsed in movement animation
    i8 vx; //x speed, direction
}
ThingObj;

ThingObj* ThingObj_load(FILE* f, u32 size, char* line, SDL_Renderer* renderer); //constructor
u0 ThingObj_free(ThingObj* tO); //destructor
u0 ThingObj_update(ThingObj* tO, Markmap* mm, Tileset* ts, f32 scW); //method t move, update
