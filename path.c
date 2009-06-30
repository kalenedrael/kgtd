#include "grid_objs.h"
#include "state.h"
#include "level.h"

enum {
	DIR_UP = 0,
	DIR_LEFT = 1,
	DIR_DOWN = 2,
	DIR_RIGHT = 3,
	DIR_NONE = 4
};

path_t *path_new(int x, int y)
{
	grid[y][x].type = GRID_TYPE_PATH;
	grid[y][x].p.pos.x = x * GRID_SIZE + GRID_SIZE/2;
	grid[y][x].p.pos.y = y * GRID_SIZE + GRID_SIZE/2;
	return &(grid[y][x].p);
}

void path_destroy(int x, int y)
{
	grid[y][x].type = GRID_TYPE_NONE;
}

static int rel_dir(int x, int y, path_t *rel)
{
	if(rel == &(grid[y + 1][x].p)) {
		return DIR_UP;
	}
	if(rel == &(grid[y - 1][x].p)) {
		return DIR_DOWN;
	}
	if(rel == &(grid[y][x + 1].p)) {
		return DIR_RIGHT;
	}
	if(rel == &(grid[y][x - 1].p)) {
		return DIR_LEFT;
	}
	return DIR_NONE;
}

/* XXX lame - alternate algorithms gladly accepted */
static void draw_one(int cx, int cy, path_t *cur, path_t *prev)
{
	int rel_prev, rel_next;
	float x = cur->pos.x, y = cur->pos.y;

	rel_prev = rel_dir(cx, cy, prev);
	rel_next = rel_dir(cx, cy, cur->next);

	if(rel_prev != DIR_UP) {
		if(rel_next == DIR_UP)
			glColor3f(0.1, 0.1, 0.1);
		else
			glColor3f(1.0, 1.0, 1.0);

		glVertex2f(x + GRID_SIZE/2, y + GRID_SIZE/2);
		glVertex2f(x - GRID_SIZE/2, y + GRID_SIZE/2);
	}

	if(rel_prev != DIR_LEFT) {
		if(rel_next == DIR_LEFT)
			glColor3f(0.1, 0.1, 0.1);
		else
			glColor3f(1.0, 1.0, 1.0);

		glVertex2f(x - GRID_SIZE/2, y + GRID_SIZE/2);
		glVertex2f(x - GRID_SIZE/2, y - GRID_SIZE/2);
	}

	if(rel_prev != DIR_DOWN) {
		if(rel_next == DIR_DOWN)
			glColor3f(0.1, 0.1, 0.1);
		else
			glColor3f(1.0, 1.0, 1.0);

		glVertex2f(x - GRID_SIZE/2, y - GRID_SIZE/2);
		glVertex2f(x + GRID_SIZE/2, y - GRID_SIZE/2);
	}

	if(rel_prev != DIR_RIGHT) {
		if(rel_next == DIR_RIGHT)
			glColor3f(0.1, 0.1, 0.1);
		else
			glColor3f(1.0, 1.0, 1.0);

		glVertex2f(x + GRID_SIZE/2, y - GRID_SIZE/2);
		glVertex2f(x + GRID_SIZE/2, y + GRID_SIZE/2);
	}
}

void path_load(state_t *state, map_t *map)
{
	int cur_x = map->x_start, cur_y = map->y_start, i;
	int new_x, new_y;
	int length = map->length;
	int *path = map->path;
	float x, y;
	path_t *cur, *prev = NULL;

	cur = state->path = path_new(map->x_start, map->y_start);

	for(i = 0; i < length; i+=2) {
		/* do x */
		new_x = cur_x + path[i];
		if(path[i] > 0) {
			for(; cur_x < new_x; cur_x++) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
		else if(path[i] < 0) {
			for(; cur_x > new_x; cur_x--) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
		if(i == length - 1)
			break;
		/* do y */
		new_y = cur_y + path[i+1];
		if(path[i + 1] > 0) {
			for(; cur_y < new_y; cur_y++) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
		else if(path[i + 1] < 0) {
			for(; cur_y > new_y; cur_y--) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
	}
	cur->next = NULL;

	glNewList(DISPLAY_LIST_PATH, GL_COMPILE);
	glBegin(GL_QUADS);
	for(cur = state->path; cur != NULL; cur = cur->next) {
		x = cur->pos.x;
		y = cur->pos.y;
		if(cur->next == NULL)
			glColor3f(0.9, 0.2, 0.2);
		else if(cur == state->path)
			glColor3f(0.1, 0.5, 0.1);
		else
			glColor3f(0.2, 0.2, 0.2);

		glVertex2f(x - GRID_SIZE/2, y - GRID_SIZE/2);
		glVertex2f(x + GRID_SIZE/2, y - GRID_SIZE/2);
		glVertex2f(x + GRID_SIZE/2, y + GRID_SIZE/2);
		glVertex2f(x - GRID_SIZE/2, y + GRID_SIZE/2);
	}
	glEnd();

	cur = state->path;
	cur_x = map->x_start;
	cur_y = map->y_start;
	glBegin(GL_LINES);
	for(i = 0; i < length; i+=2) {
		new_x = cur_x + path[i];
		if(path[i] > 0) {
			for(; cur_x < new_x; cur_x++) {
				draw_one(cur_x, cur_y, cur, prev);
				prev = cur;
				cur = cur->next;
			}
		}
		else if(path[i] < 0) {
			for(; cur_x > new_x; cur_x--) {
				draw_one(cur_x, cur_y, cur, prev);
				prev = cur;
				cur = cur->next;
			}
		}
		if(i == length - 1)
			break;
		new_y = cur_y + path[i+1];
		if(path[i + 1] > 0) {
			for(; cur_y < new_y; cur_y++) {
				draw_one(cur_x, cur_y, cur, prev);
				prev = cur;
				cur = cur->next;
			}
		}
		else if(path[i + 1] < 0) {
			for(; cur_y > new_y; cur_y--) {
				draw_one(cur_x, cur_y, cur, prev);
				prev = cur;
				cur = cur->next;
			}
		}
	}
	glEnd();
	glEndList();
}

void path_draw_all(state_t *state)
{
	glCallList(DISPLAY_LIST_PATH);
}
