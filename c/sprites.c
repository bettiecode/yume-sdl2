#include "../h/sprites.h"

Sprite* Sprite_load(char* filename, SDL_Renderer* renderer)
{
  Sprite* r = malloc(sizeof(Sprite)); //alloc
  if(!r)
  {
    fprintf(stderr,"ERROR: Failed to allocate memory for sprite.\n");
    return NULL;
  }
  SDL_Surface* temp = IMG_Load(filename);
  if(!temp)
  {
    fprintf(stderr,"ERROR: Failed to create temporary surface for sprite. - %s\n",SDL_GetError());
    return NULL;
  }
  r->sheet = SDL_CreateTextureFromSurface(renderer,temp);
  if(!temp)
  {
    fprintf(stderr,"ERROR: Failed to create texture for sprite. - %s\n",SDL_GetError());
    return NULL;
  }
  SDL_FreeSurface(temp);
  //load texture
  return r;
}
u0 Sprite_free(Sprite* spr)
{
  SDL_DestroyTexture(spr->sheet);
  free(spr);
}

u0 Sprite_render(Sprite* spr, f32 x, f32 y, SDL_Renderer* renderer, Cam* cam)
{
  SDL_Rect dest =
  {
    (x-spr->ox-cam->x)*cam->zoom,
    (y-spr->oy-cam->y)*cam->zoom,
    spr->src.w*cam->zoom,
    spr->src.h*cam->zoom
  };
  //where to draw
  SDL_RenderCopy(renderer,spr->sheet,&spr->src,&dest); //draw
}
