#include "../h/game.h" //needed to run game
#include <time.h> //needed to seed rng

int main() //main function
{
    srand(time(NULL)); //seed rng
    Game* g = Game_make(); //make game object
    if(!g) //if ocnstruction failed
    {
        fprintf(stderr,"ERROR: Failed to construct Game object.\n"); //log error
        return 1; //return error
    }

    Game_run(g); //run game

    return 0;
}
