#include "grid_objs.h"
#include "state.h"
#include "level.h"

path_t *path_new(int x, int y)
{
	grid[y][x].type = GRID_TYPE_PATH;
	grid[y][x].p.x = x * GRID_SIZE + GRID_SIZE/2;
	grid[y][x].p.y = y * GRID_SIZE + GRID_SIZE/2;
	return &(grid[y][x].p);
}

void path_destroy(int x, int y)
{
	grid[y][x].type = GRID_TYPE_NONE;
}

void path_load(state_t *state, map_t *map)
{
	int cur_x = map->x_start, cur_y = map->y_start, i;
	int new_x, new_y;
	int length = map->length;
	int *path = map->path;
	float x, y;
	state->path = path_new(map->x_start, map->y_start);
	path_t *cur = state->path;

	for(i = 0; i < length; i+=2) {
		/* do x */
		new_x = cur_x + path[i];
		if(path[i] >= 0) {
			for(; cur_x < new_x; cur_x++) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
		else {
			for(; cur_x > new_x; cur_x--) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
		if(i == length - 1)
			break;
		/* do y */
		new_y = cur_y + path[i+1];
		if(path[i+1] >= 0) {
			for(; cur_y < new_y; cur_y++) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
		else {
			for(; cur_y > new_y; cur_y--) {
				cur->next = path_new(cur_x, cur_y);
				cur = cur->next;
			}
		}
	}
	cur->next = NULL;

	cur = state->path;
	glNewList(DISPLAY_LIST_PATH, GL_COMPILE);
	glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_QUADS);
	while(cur != NULL) {
		if(cur->type != GRID_TYPE_PATH) {
			abort();
			state->path = NULL;
		}
		x = cur->x;
		y = cur->y;
		if(cur->next == NULL)
			glColor3f(0.9, 0.2, 0.2);

		glVertex2f(x - GRID_SIZE/2, y - GRID_SIZE/2);
		glVertex2f(x + GRID_SIZE/2, y - GRID_SIZE/2);
		glVertex2f(x + GRID_SIZE/2, y + GRID_SIZE/2);
		glVertex2f(x - GRID_SIZE/2, y + GRID_SIZE/2);
		cur = cur->next;

	}
	glEnd();
	glEndList();
}

void path_draw_all(state_t *state)
{
	glCallList(DISPLAY_LIST_PATH);
}
