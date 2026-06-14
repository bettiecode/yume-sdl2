
#include "../sprites.h"

typedef struct //door class
{
    i32 x,y; //position
    Sprite* spr; //sprite
}
DoorObj;

DoorObj* DoorObj_load(FILE* f, u32 size, char* line, SDL_Renderer* renderer); //constructor
u0 DoorObj_free(DoorObj* d); //destructor
