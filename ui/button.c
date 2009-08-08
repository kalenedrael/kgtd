/* A hierarchial button type, for the button tree used by the tower selector. */

#include "button.h"

static int click_fn(sglui_obj *obj, SDL_MouseButtonEvent *ev)
{
	if(ev->button == SDL_BUTTON_LEFT)
		((sglui_btn*)obj)->btn_cb();
	return SGLUI_BLOCK_CHILDREN;
}

static int ixn_fn(sglui_obj *obj, int x, int y)
{
	sglui_btn *btn = (sglui_btn*)obj;
	return x > btn->x && x < btn->x + btn->w &&
	       y > btn->y && y < btn->y + btn->h;
}

int sglui_button_init(sglui_btn *btn, sglui_obj *parent, int x, int y, int w,
                      int h, sglui_draw_fn draw_fn, void(*btn_cb)(void))
{
	btn->x = x;
	btn->y = y;
	btn->w = w;
	btn->h = h;
	btn->btn_cb = btn_cb;
	return sglui_obj_init(&btn->obj, parent, draw_fn, ixn_fn, click_fn, NULL);
}
