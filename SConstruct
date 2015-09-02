import os
import sys

environment = Environment(ENV = os.environ)

libjoybind = environment.StaticLibrary("joybind", ["sdl_joystick_bind.c"])

make_fltk_test = False
AddOption('--fltk',
          nargs=1,
          help='Whether or not to build the FLTK components, such as the FLTK-based joystick configuration utility')

configure = Configure(environment)
if configure.CheckCXXHeader('FL/fl.H') or GetOption('fltk'):
    make_fltk_test = True
environment = configure.Finish()

if sys.platform=='darwin':
    environment.Append(FRAMEWORKS = ["Foundation", "Cocoa"])

if make_fltk_test:
    fltkjoytest = environment.Program("fltkjoytest", ["fltk_joystick_test.cpp"], LIBS = ["SDL2", "fltk", libjoybind])
    
joytest = environment.Program("joytest", ["sdl_joystick_test.c"], LIBS = ["SDL2", "fltk", libjoybind])
