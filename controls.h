#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "globals.h"

#define BUTTONS_X 160
#define BUTTONS_Y (YRES-160)

void controls_init(void);
void controls_draw(int x, int y, state_t *state);
void controls_click(SDL_MouseButtonEvent *ev, state_t *state);

#endif

