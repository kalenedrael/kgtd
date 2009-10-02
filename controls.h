#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "globals.h"

#define SEL_SIZE 160
#define SEL_X SEL_SIZE
#define SEL_Y (YRES - SEL_SIZE)
#define SEL_BUFFER 32
#define BTN_SIZE (SEL_SIZE / 4)
#define BTN_OFFSET (BTN_SIZE - TOWER_SIZE)
#define BOT_BAR (YRES - 80)
#define BOT_BAR_BUFFER 16

void controls_init(void);
void controls_draw(state_t *state);
void controls_update(int dt, state_t *state);
void controls_click(SDL_MouseButtonEvent *ev, state_t *state);

#endif

