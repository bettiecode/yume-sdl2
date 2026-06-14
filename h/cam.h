#ifndef CAM_H
#define CAM_H

#include "typedefs.h"

#define WINDOW_HEIGHT 768
#define WINDOW_WIDTH 1024
//window size macros

typedef struct //camera struct
{
  f32 x,y; //pos
  f32 zoom; //zoom
  f32 w,h; //size
  u8 ali; //alignment, meaning which of the nine parts  of the looping world does the camera see
}
Cam;

#endif
