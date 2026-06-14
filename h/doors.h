#ifndef DOORS_H
#define DOORS_H

#include "typedefs.h"

typedef struct //door struct
{
  i32 px, py; //position to put the player in after swap
  u8 pFace; //face of the player after swap
  u32 scIdx; //index of scene the door leads to
}
Door;

#endif
