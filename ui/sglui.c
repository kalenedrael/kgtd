/* @file sglui.c
 * @brief SDL/GL UI functions
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "sglui.h"

static sglui_obj head;

int sglui_ixn_always(sglui_obj *obj, int x, int y)
{
	return 1;
}

int sglui_ixn_never(sglui_obj *obj, int x, int y)
{
	return 0;
}

int sglui_init(sglui_ixn_fn head_fn)
{
	/* set all fields to NULL or 0 */
	memset(&head, 0, sizeof(sglui_obj));
	head.is_within = head_fn;

	return 0;
}

int sglui_obj_init(sglui_obj *obj, sglui_obj *parent,
                   sglui_draw_fn draw_cb, sglui_ixn_fn ixn_cb,
                   sglui_click_fn click_cb, sglui_move_fn move_cb)
{
	/* set fields */
	obj->draw = draw_cb;
	obj->is_within = ixn_cb;
	obj->click = click_cb;
	obj->move = move_cb;
	obj->parent = NULL;

	Q_INIT_HEAD(&obj->children);
	Q_INIT_ELEM(obj, list);

	sglui_link(obj, parent);

	return 0;
}

void sglui_link(sglui_obj *obj, sglui_obj *parent)
{
	sglui_unlink(obj);
	if(parent == NULL)
		parent = &head;

	Q_INSERT_TAIL(&parent->children, obj, list);
	obj->parent = parent;
}

void sglui_unlink(sglui_obj *child)
{
	sglui_obj *parent = child->parent;
	if(!parent)
		return;

	Q_REMOVE(&parent->children, child, list);
	child->parent = NULL;
}

static void traverse_draw(sglui_obj *obj)
{
	sglui_obj *cur;

	if(obj->draw && obj->draw(obj))
		return;

	Q_FOREACH(cur, &obj->children, list)
		traverse_draw(cur);
}

static int traverse_click(sglui_obj *obj, SDL_MouseButtonEvent *ev)
{
	int ret = 0, in;
	sglui_obj *cur;

	if((in = obj->is_within(obj, ev->x, ev->y)) > 0 && obj->click) {
		ret = obj->click(obj, ev);
		if(ret)
			return ret == SGLUI_BLOCK_ALL;
	}
	if(in < 0 || in > 1)
		return 0;

	Q_FOREACH(cur, &obj->children, list) {
		if(traverse_click(cur, ev))
			return 1;
	}

	return 0;
}

static int traverse_move(sglui_obj *obj, SDL_MouseMotionEvent *ev)
{
	int ret = 0, in1, in2;
	sglui_obj *cur;

	in1 = obj->is_within(obj, ev->x, ev->y);
	in2 = obj->is_within(obj, ev->x - ev->xrel, ev->y - ev->yrel);

	if((in1 > 0 || in2 > 0) && obj->move) {
		ret = obj->move(obj, ev, in1 > 0 ? (in2 > 0 ? SGLUI_MOVE : SGLUI_ON) : SGLUI_OFF);
		if(ret)
			return ret == SGLUI_BLOCK_ALL;
	}
	if(in1 < 0 || in1 > 1)
		return 0;

	Q_FOREACH(cur, &obj->children, list) {
		if(traverse_move(cur, ev))
			return 1;
	}

	return 0;
}

void sglui_handle_ev(SDL_Event *ev)
{
	switch(ev->type) {
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
		traverse_click(&head, &ev->button);
	case SDL_MOUSEMOTION:
		traverse_move(&head, &ev->motion);
	default:
		return;
	}
}

void sglui_draw_all()
{
	traverse_draw(&head);
}
