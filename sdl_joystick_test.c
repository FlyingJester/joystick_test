#include <SDL2/SDL.h>
#include <stdio.h>
#include "sdl_joystick_bind.h"
int main(int argc, char *argv[]){
    int f;
    SDL_Init(SDL_INIT_JOYSTICK);
    BMOP_EnumerateJoysticks(NULL, NULL, NULL);
    do{
        f = BMOP_WaitJoyEvent(NULL, NULL, NULL, NULL);
    
    }while(f>=0);
    
    if(argc || argv){}
}
