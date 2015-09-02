#include <SDL2/SDL.h>
#include <stdio.h>
#include "sdl_joystick_bind.h"

void BMOP_DefaultJoystickEnumerator(void *arg, const char *name, unsigned num_buttons, unsigned num_axes, unsigned num_hats){
    if(arg){}

    fputs("Detected joystick ", stdout);
    puts(name);
    printf("\nHas %i buttons, %i axes, and %i hats\n", num_buttons, num_axes, num_hats);
}

void BMOP_DefaultNumJoysticks(void * arg, unsigned n){
    if(arg){}
    printf("Detected %i joysticks.\n", n);
}

void BMOP_DefaultJoyButton(void *arg, unsigned which, unsigned button, int pressed){
    printf("Joystick %i\t Button %i\t%s\n", (int)which, (int)button, pressed?"pressed":"released");
    if(arg){}
}

/* dir is a bitmask of BMOP_JOYHAT_LEFT, BMOP_JOYHAT_RIGHT, BMOP_JOYHAT_UP, and BMOP_JOYHAT_DOWN */
void BMOP_DefaultJoyHat(void *arg, unsigned which, unsigned hat, unsigned dir){
    int x = 0;
    
    if(arg){}

    printf("Joystick %i hat %i is now ", (int)which, (int)hat);
#define BMOP_JOYHAT_DIR_M(X, S)\
    if(dir & X){\
        if(x++)\
            fputs(" + ", stdout);\
        fputs(S, stdout);\
    }
    
    BMOP_JOYHAT_DIR_M(BMOP_JOYHAT_LEFT, "Left")
    BMOP_JOYHAT_DIR_M(BMOP_JOYHAT_RIGHT, "Right")
    BMOP_JOYHAT_DIR_M(BMOP_JOYHAT_UP, "Up")
    BMOP_JOYHAT_DIR_M(BMOP_JOYHAT_DOWN, "Down")
    
#undef BMOP_JOYHAT_DIR_M

    if(dir==0)
        fputs("Centered", stdout);

    fputc('\n', stdout);

}

/* Val is clamped from -1.0f to 1.0f */
void BMOP_DefaultJoyAxis(void *arg, unsigned which, unsigned axis, float val){
    int n = val*8, i = 0;
    n += 8;

    printf("Joystick %i\t Axis %i\t <", (int)which, (int)axis);

    while(i++<n) putchar('=');
    putchar('|');
    while(i++<16) putchar('=');
    putchar('>');
    putchar('\n');

    if(arg){}

}

void BMOP_EnumerateJoysticks(void *arg, void (*AnnounceNumJoysticks)(void *, unsigned), BMOP_JoystickEnumerator_t enumerator){
    unsigned joystick_num = 0;
    SDL_Joystick *stick = NULL;
    
    if(!AnnounceNumJoysticks)
        AnnounceNumJoysticks = BMOP_DefaultNumJoysticks;
    if(!enumerator)
        enumerator = BMOP_DefaultJoystickEnumerator;

    AnnounceNumJoysticks(arg, SDL_NumJoysticks());
    while((stick = SDL_JoystickOpen(joystick_num++)))
        enumerator(arg, SDL_JoystickNameForIndex(joystick_num-1), 
            SDL_JoystickNumButtons(stick),
            SDL_JoystickNumAxes(stick),
            SDL_JoystickNumHats(stick));
}

/* Z must operate on an SDL_Event called e (by val, not pointer) */
#define BMOP_JOYEVENT(NAME, Z, E_REP, E_RET)\
int NAME(void *arg, BMOP_JoyButton_t b, BMOP_JoyHat_t h, BMOP_JoyAxis_t a){\
    SDL_Event e;\
    int f;\
    if(!b) b = BMOP_DefaultJoyButton;\
    if(!h) h = BMOP_DefaultJoyHat;\
    if(!a) a = BMOP_DefaultJoyAxis;\
    f = Z;\
    if(f==0) return 0;\
    switch(e.type){\
        case SDL_JOYBUTTONDOWN:\
        case SDL_JOYBUTTONUP:\
        b(arg, e.jbutton.which, e.jbutton.button, e.type==SDL_JOYBUTTONDOWN);\
        break;\
        case SDL_JOYHATMOTION:\
	switch(e.jhat.value){\
            case SDL_HAT_LEFTUP: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_LEFT|BMOP_JOYHAT_UP); break;\
            case SDL_HAT_LEFT: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_LEFT); break;\
            case SDL_HAT_LEFTDOWN: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_LEFT|BMOP_JOYHAT_DOWN); break;\
            case SDL_HAT_DOWN: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_DOWN); break;\
            case SDL_HAT_RIGHTDOWN: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_RIGHT|BMOP_JOYHAT_DOWN); break;\
            case SDL_HAT_RIGHT: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_RIGHT); break;\
            case SDL_HAT_RIGHTUP: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_RIGHT|BMOP_JOYHAT_UP); break;\
            case SDL_HAT_UP: h(arg, e.jhat.which, e.jhat.hat, BMOP_JOYHAT_UP); break;\
            case SDL_HAT_CENTERED:  h(arg, e.jhat.which, e.jhat.hat, 0); break;\
        }\
        break;\
        case SDL_JOYAXISMOTION:\
        a(arg, e.jaxis.which, e.jaxis.axis, (((float)e.jaxis.value)+0.5f)/32767.5f);\
        break;\
        case SDL_QUIT:\
        return -1;\
    }\
    return E_RET(f, e);\
}

#define BMOP_JOY_POLL_REP(a, b) 0
#define BMOP_JOY_POLL_RET(f, e) f
#define BMOP_JOY_WAIT_REP(f, e) e.type==SDL_JOYBUTTONDOWN||e.type==SDL_JOYHATMOTION||e.type==SDL_JOYAXISMOTION
#define BMOP_JOY_WAIT_RET(f, e) 1

BMOP_JOYEVENT(BMOP_PollJoyEvent, SDL_PollEvent(&e), BMOP_JOY_POLL_REP, BMOP_JOY_POLL_RET)
BMOP_JOYEVENT(BMOP_WaitJoyEvent, SDL_PollEvent(&e), BMOP_JOY_WAIT_REP, BMOP_JOY_WAIT_RET)
