#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Positioner.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include "sdl_joystick_bind.h"
#include <SDL2/SDL.h>

namespace bmop{
namespace joy{

/* Same as an Fl_Positioner, but does not take input from the mouse. */
class Positioner : public Fl_Positioner{
public:
    Positioner(int Ax, int Ay, int Aw, int Ah, const char *Acaption = nullptr)
      : Fl_Positioner(Ax, Ay, Aw, Ah, Acaption){}

    int handle(int e) override{
        if(e){}
        return 0;
    }
};

class Joystick {
    static int num_joysticks;

    Fl_Window window;
    Fl_Box title;
    Fl_Pack holder;
    std::vector<Positioner *> axes, hats;
    std::vector<Fl_Box *> buttons;
public:

    Joystick(const char *name, unsigned num_buttons, unsigned num_axes, unsigned num_hats)
      : window((num_buttons * 32) + ((num_axes>>1) * 64) + 8, 96, name)
      , title(4, 4, (num_buttons * 32) + ((num_axes>>1) * 64), 10)
      , holder(4, 16, (num_buttons * 32) + ((num_axes>>1) * 64), 64){
        holder.type(Fl_Pack::HORIZONTAL);
        window.add(holder);
        holder.begin();
            /* MSVC _STILL_ doesn't have std::to_string ;_; Even Solaris Studio has it by now.
             * Because to Microsoft, it's more a general guideline than a specification.
             */
             
             /* MSVC is also missing snprintf. Because there should be no way to safely put
              * a number into an array of chars without involving UCS chars or .NET
              */
        {
            char buffer[80] = {0};
            sprintf(buffer, "Joystick %i", ++num_joysticks);
            title.copy_label(buffer);
        }
        for(unsigned i = 0; i<(num_axes>>1); i++){
            char buffer[80] = {0};
            sprintf(buffer, "%i, %i", (i<<1), (i<<1) + 1);
            axes.push_back(new Positioner(4, 4, 64, 64));
            axes.back()->copy_label(buffer);
            axes.back()->xbounds(-1.0, 1.0);
            axes.back()->ybounds(-1.0, 1.0);
            axes.back()->xstep(0.01);
            axes.back()->ystep(0.01);
            
        }
        
        for(unsigned i = 0; i<num_hats; i++){
        }
        
        for(unsigned i = 0; i<num_buttons; i++){
            char buffer[80] = {0};
            sprintf(buffer, "%i", i);
            buttons.push_back(new Fl_Box(4, 4, 32, 32));
            buttons.back()->copy_label(buffer);
            buttons.back()->box(FL_ROUND_UP_BOX);
        }
        holder.end();
        window.end();
    }

    static void Create(void *a, const char *name, unsigned num_buttons, unsigned num_axes, unsigned num_hats){
        static_cast<std::vector<Joystick *>*>(a)->push_back(
            Create(name, num_buttons, num_axes, num_hats)
        );
    }

    static Joystick *Create(const char *name, unsigned num_buttons, unsigned num_axes, unsigned num_hats){
        Joystick *that = new Joystick(name, num_buttons, num_axes, num_hats);
        that->window.show();
        return that;
    }
    
    static void JoyButton(void *a, unsigned which, unsigned b, int pressed){
        BMOP_DefaultJoyButton(a, which, b, pressed);
        std::vector<Joystick *> &vector = *static_cast<std::vector<Joystick *>*>(a);
        if(which>vector.size())
            return;
        if(b > vector[which]->buttons.size())
            return;
        puts("Worked");
        
        if(pressed){
            vector[which]->buttons[b]->box(FL_ROUND_DOWN_BOX);
            vector[which]->buttons[b]->color(0xff000000); 
        }
        else{
            vector[which]->buttons[b]->box(FL_ROUND_UP_BOX);
            vector[which]->buttons[b]->color(FL_BACKGROUND_COLOR);
            
        }
        vector[which]->buttons[b]->redraw();

    }

    /* dir is a bitmask of BMOP_JOYHAT_LEFT, BMOP_JOYHAT_RIGHT, BMOP_JOYHAT_UP, and BMOP_JOYHAT_DOWN */
    static void JoyHat(void *a, unsigned which, unsigned hat, unsigned dir){
        BMOP_DefaultJoyHat(a, which, hat, dir);
        if(a || which || hat || dir){}
    }

    /* Val is clamped from -1.0f to 1.0f */
    static void JoyAxis(void *a, unsigned which, unsigned ax, float val){
        BMOP_DefaultJoyAxis(a, which, ax, val);
        std::vector<Joystick *> &vector = *static_cast<std::vector<Joystick *>*>(a);
        if(which>vector.size())
            return;
        if(ax>>1 > vector[which]->axes.size())
            return;

        if(ax%2){
            vector[which]->axes[ax>>1]->yvalue(val);
        }
        else{
            vector[which]->axes[ax>>1]->xvalue(val);
        }
    }
};


int Joystick::num_joysticks = 0;

} //namespace joy
} //namespace bmop

using bmop::joy::Joystick;

int main(int argc, char *argv[]){
    if(argc || argv){}
    
    std::vector<Joystick *> joysticks;
    
    SDL_Init(SDL_INIT_JOYSTICK);
    BMOP_EnumerateJoysticks(&joysticks, nullptr, Joystick::Create);
    
    int x = 0;
    
    while(BMOP_PollJoyEvent(&joysticks, Joystick::JoyButton, Joystick::JoyHat, Joystick::JoyAxis)!=-1){
        if(++x%0x100 == 0){
            x = 0;
            if(!Fl::check())
                break;
        }
    }

}

//chelsea
