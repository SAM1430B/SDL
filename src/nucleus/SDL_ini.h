// Nucleus Co-Op INI header file
#ifndef SDL_ini_h_
#define SDL_ini_h_

#include "SDL_internal.h"
#include "../joystick/SDL_joystick_c.h"

/* * Parses SDL3.ini to determine the enforced player index based on chronological connection order.
 * Returns:
 * >= 0 : Enforced player index
 * -1   : Restrict/ignore joystick
 * -2   : Auto-assign (fallback behavior)
 */
extern int SDL_GetPlayerIndexFromINI(SDL_JoystickID instance_id);

#endif