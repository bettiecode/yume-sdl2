#include "../h/objs.h"

#include "../h/player.h"

ObjLayer* ObjLayer_load(char* filename, SDL_Renderer* renderer, Player* p)
{
    //see objlayer file structure "a/objlayers/chess.dat"
    ObjLayer* ol = malloc(sizeof(ObjLayer)); //allocate memory
    if(!ol)
    {
        fprintf(stderr,"Failed to allocate memory for object layer.\n");
        return NULL;
    }

    FILE* f = fopen(filename,"r"); //open to read
    char line[64]; //buffer

    fgets(line,64,f);
    ol->itemC = atoi(line);
    //get length
    ol->items = malloc(sizeof(u0*)*(ol->itemC+(p!=NULL)));
    if(!ol->items)
    {
        fprintf(stderr,"Failed to allocate memory for object layer's items.\n");
        return NULL;
    }
    ol->types = malloc(sizeof(u32)*(ol->itemC+(p!=NULL)));
    if(!ol->types)
    {
        fprintf(stderr,"Failed to allocate memory for object layer's types.\n");
        return NULL;
    }
    //allocate arrays, +1 space for the player if the layer includes it
    for(u32 i=0; i<ol->itemC; i++) //read objs,
    {
        fgets(line,64,f);
        ol->types[i] = atoi(line);
        //get type
        switch(ol->types[i]) //object constructors continue reading the obj layer's file'
        {
            case OBJ_DOOR:
                ol->items[i] = DoorObj_load(f,64,line,renderer);
            break;
            case OBJ_THING:
                ol->items[i] = ThingObj_load(f,64,line,renderer);
            break;
            default:
                ol->items[i] = NULL;
            break;
        }
        if(!ol->items[i])
        {
            fprintf(stderr,"Failed to create object layer's item (%u).\n",i);
            return NULL;
        }
    }
    if(p!=NULL) //if layer contains the player
    {
        ol->types[ol->itemC] = OBJ_PLAYER;
        ol->items[ol->itemC] = p;
        //the +1 is occupied by the player
        ol->drawOrder = malloc(sizeof(u32)*ol->itemC+1); //make draw order array
        if(!ol->drawOrder)
        {
            fprintf(stderr,"Failed to allocate memory for object layer's draw order.\n");
            return NULL;
        }
        for(u32 i=0; i<ol->itemC+1; i++) ol->drawOrder[i]=i; //set inital values
    }
    else
        ol->drawOrder = NULL; //if no player, then skip everything related to depth sorting
    fclose(f); //close
    return ol; //return successfully
}

u0 ObjLayer_free(ObjLayer* ol) //destructor
{
    if(!ol) return; //NULL safety
    for(u32 i=0; i<ol->itemC; i++) //free each obj
    {
        switch(ol->types[i])
        {
            case OBJ_DOOR:
                DoorObj_free(ol->items[i]);
            break;
            case OBJ_THING:
                ThingObj_free(ol->items[i]);
            break;
            default:
            break;
        }
    }
    free(ol->types);
    free(ol->items);
    if(!ol->drawOrder) free(ol->drawOrder);
    free(ol);
    //free allocations
}

u0 ObjLayer_update(ObjLayer* ol,Markmap* mm, Tileset* ts, f32 scW, f32 scH) //update, move objs
{
    for(u32 i=0; i<ol->itemC; i++) //loop through objs
    {
        switch(ol->types[i]) //find type
        {
            case OBJ_THING:
                ThingObj_update(ol->items[i],mm,ts,scW); //call its update function
            break;
            default:
            break;
            //player is updated in Scene_playerUpdate, see "c/scenes.c"
        }
    }
}

u0 ObjLayer_renderObjsOffset(ObjLayer* ol, SDL_Renderer* renderer, Cam* cam, i32 offX, i32 offY)
{
    for(u32 i=0; i<ol->itemC+(ol->drawOrder!=NULL); i++) //loop thorough ibjs
    {
        u32 j = (ol->drawOrder!=NULL) ? ol->drawOrder[i] : i; //index of obj to draw on ol->items
        u0*  o = ol->items[j]; //pointer t obj
        //to shorten
        switch(ol->types[j]) //find type
        {
            case OBJ_PLAYER:
                Sprite_render
                (
                    ((Player*)o)->spr,
                    ((Player*)o)->x+offX,
                    ((Player*)o)->y+offY,
                    renderer,cam
                );
            break;
            case OBJ_DOOR:
                Sprite_render
                (
                    ((DoorObj*)o)->spr,
                    ((DoorObj*)o)->x+offX,
                    ((DoorObj*)o)->y+offY,
                 renderer,cam
                );
            break;
            case OBJ_THING:
                Sprite_render
                (
                    ((ThingObj*)o)->spr,
                    ((ThingObj*)o)->x+offX,
                    ((ThingObj*)o)->y+offY,
                    renderer,cam
                );
            break;
            default:

                break;
        }
    }
}

u0 ObjLayer_render(ObjLayer* ol, SDL_Renderer* renderer, Cam* cam, f32 scW, f32 scH)
{
    if(ol->drawOrder!=NULL) ObjLayer_depthSort(ol); //depth sort if needed

    switch(cam->ali) //find aligment, and render segments
    {
        case 0:
            ObjLayer_renderObjsOffset(ol, renderer, cam, -scW, -scH);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, -scH);
            ObjLayer_renderObjsOffset(ol, renderer, cam, -scW, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
        break;
        case 1:
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, -scH);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
        break;
        case 2:
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, -scH);
            ObjLayer_renderObjsOffset(ol, renderer, cam, scW, -scH);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, scW, 0);
        break;
        case 3:
            ObjLayer_renderObjsOffset(ol, renderer, cam, -scW, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
        break;
        case 4:
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
        break;
        case 5:
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, scW, 0);
        break;
        case 6:
            ObjLayer_renderObjsOffset(ol, renderer, cam, -scW, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, -scW, scH);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, scH);
        break;
        case 7:
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, scH);
        break;
        case 8:
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, scW, 0);
            ObjLayer_renderObjsOffset(ol, renderer, cam, 0, scH);
            ObjLayer_renderObjsOffset(ol, renderer, cam, scW, scH);
        break;
        default:
        break;
        //order fucntion call order matters, because of painter's algorithm, which is what is used for rendering'
    }
}

u0 ObjLayer_depthSort(ObjLayer* ol) //dept sort
{
    b8 sorted = true; //sorted until proven otherwise
    for(i32 i=0; i<(i32)ol->itemC; i++) //look for unsortedness
    {
        if(ObjLayer_getY(ol,ol->drawOrder[i])>ObjLayer_getY(ol,ol->drawOrder[i+1])) //if found
        {
            sorted = false; //log
            break; //exit
        }
    }
    if(sorted) return; //if soted then we are done
    for(i32 i=1; i<(i32)ol->itemC+1; i++) //if not, then sort the lements of drawOrder, using insertion sort
    {
        i32 valI = ol->drawOrder[i];
        f32 valY = ObjLayer_getY(ol,valI);
        i32 j = i-1;
        while(j>=0 && (ObjLayer_getY(ol,ol->drawOrder[j])>=valY))
        {
            ol->drawOrder[j+1] = ol->drawOrder[j];
            j--;
        }
        ol->drawOrder[j+1] = valI;
    }
    //insertion sort using getY
}

f32 ObjLayer_getY(ObjLayer* ol, u32 i)
{
    switch(ol->types[i]) //find type
    {
        case OBJ_PLAYER: return ((Player*)(ol->items[i]))->y;
        case OBJ_DOOR: return ((DoorObj*)(ol->items[i]))->y;
        case OBJ_THING: return ((ThingObj*)(ol->items[i]))->y;
        default: return 0;
        //return the y coord
    }
}

