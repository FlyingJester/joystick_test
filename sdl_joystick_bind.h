#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define BMOP_JOYHAT_LEFT  (1<<0)
#define BMOP_JOYHAT_RIGHT (1<<1)
#define BMOP_JOYHAT_UP    (1<<2)
#define BMOP_JOYHAT_DOWN  (1<<3)

void BMOP_DefaultJoystickEnumerator(void *arg, const char *name, unsigned num_buttons, unsigned num_axes, unsigned num_hats);
void BMOP_DefaultNumJoysticks(void *, unsigned);

void BMOP_DefaultJoyButton(void *, unsigned which, unsigned button, int pressed);

/* dir is a bitmask of BMOP_JOYHAT_LEFT, BMOP_JOYHAT_RIGHT, BMOP_JOYHAT_UP, and BMOP_JOYHAT_DOWN */
void BMOP_DefaultJoyHat(void *, unsigned which, unsigned hat, unsigned dir);

/* Val is clamped from -1.0f to 1.0f */
void BMOP_DefaultJoyAxis(void *, unsigned which, unsigned axis, float val);

typedef void (*BMOP_JoystickEnumerator_t)(void *arg, const char *name, unsigned num_buttons, unsigned num_axes, unsigned num_hats);
typedef void (*BMOP_NumJoysticks_t)(void *, unsigned);
typedef void (*BMOP_JoyButton_t)(void *, unsigned joy, unsigned, int pressed);
typedef void (*BMOP_JoyHat_t)(void *, unsigned which, unsigned hat, unsigned dir);
typedef void (*BMOP_JoyAxis_t)(void *, unsigned which, unsigned axis, float val);

void BMOP_EnumerateJoysticks(void *arg, void (*AnnounceNumJoysticks)(void *, unsigned), BMOP_JoystickEnumerator_t);

/* Returns 1 if a useful event occured, 0 normally, and -1 on a quit event. */
int BMOP_WaitJoyEvent(void *arg, BMOP_JoyButton_t b, BMOP_JoyHat_t h, BMOP_JoyAxis_t a);
int BMOP_PollJoyEvent(void *arg, BMOP_JoyButton_t b, BMOP_JoyHat_t h, BMOP_JoyAxis_t a);

#ifdef __cplusplus
}
#endif

