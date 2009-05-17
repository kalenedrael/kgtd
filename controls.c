#include "controls.h"
#include "state.h"

#define SPACING 40

TTF_Font *display_font;

static inline unsigned int nextpoweroftwo(unsigned int x)
{
	x |= x >> 16;
	x |= x >> 8;
	x |= x >> 4;
	x |= x >> 2;
	x |= x >> 1;
	return x + 1;
}

void SDL_GL_RenderText(char *text, 
                      SDL_Color color,
                      SDL_Rect *location)
{
	SDL_Surface *initial;
	SDL_Surface *intermediary;
	int w,h;
	unsigned int texture;

	/* Use SDL_TTF to render our text */
	initial = TTF_RenderText_Blended(display_font, text, color);

	/* Convert the rendered text to a known format */
	w = nextpoweroftwo(initial->w);
	h = nextpoweroftwo(initial->h);

	intermediary = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0);

	SDL_BlitSurface(initial, 0, intermediary, 0);
	
	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, 
			GL_UNSIGNED_BYTE, intermediary->pixels );
	
	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	/* Draw a quad at location */
	glBlendFunc(GL_ONE, GL_ONE);
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0, 0.0); 
		glVertex2f(location->x , location->y);
		glTexCoord2f(1.0, 0.0); 
		glVertex2f(location->x + w, location->y);
		glTexCoord2f(1.0, 1.0);
		glVertex2f(location->x + w, location->y + h);
		glTexCoord2f(0.0, 1.0); 
		glVertex2f(location->x , location->y + h);
	glEnd();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Bad things happen if we delete the texture before it finishes */
	glFinish();
	
	/* return the deltas in the unused w,h part of the rect */
	location->w = initial->w;
	location->h = initial->h;
	
	/* Clean up */
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}

static void draw_scores(state_t *state)
{
	char buf[256];
	SDL_Rect rect;

	rect.x = 10;
	rect.y = 500;
	SDL_Color color = {255,255,255};

	sprintf(buf, "score: %d", state->score);
	SDL_GL_RenderText(buf, color, &rect);
	rect.y += rect.h;
	sprintf(buf, "spawns: %d", state->total_noobs);
	SDL_GL_RenderText(buf, color, &rect);
	rect.y += rect.h;
	sprintf(buf, "kills: %d", state->kills);
	SDL_GL_RenderText(buf, color, &rect);
	rect.y += rect.h;
	sprintf(buf, "leaks: %d", state->leaks);
	SDL_GL_RenderText(buf, color, &rect);
}

static void draw_buttons(state_t *state)
{
	int i;

	glBegin(GL_QUADS);
	glColor3f(0.1, 0.1, 0.1);
	glVertex2f(0.0, 0.0);
	glVertex2f(SIDEBAR_SIZE, 0.0);
	glVertex2f(SIDEBAR_SIZE, YRES);
	glVertex2f(0.0, YRES);
	glEnd();

	for(i = 0; i < ATTR_NUM; i++) {
		float offset = i * SPACING;
		glBegin(GL_QUADS);
		glColor3fv(attr_colors[i]);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glEnd();

		glBegin(GL_LINE_STRIP);
		if(i == state->type_selected)
			glColor3f(1.0, 1.0, 1.0);
		else
			glColor3f(0.4, 0.4, 0.4);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glEnd();
	}
}

void controls_init(void)
{
	TTF_Init();
	atexit(TTF_Quit);
	if((display_font = TTF_OpenFont("arial.ttf", 12)) == NULL) {
		printf("failed to open font\n");
		exit(0);
	}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void controls_draw(state_t *state)
{
	draw_buttons(state);
	draw_scores(state);
}

void controls_click(int x, int y, state_t *state)
{
	if(x > SIDEBAR_SIZE/2 - GRID_SIZE/2 && x < SIDEBAR_SIZE/2 + GRID_SIZE/2) {
		int ynorm, yoff, ypos;
		ynorm = y - SIDEBAR_SIZE/2 + GRID_SIZE/2;
		yoff = ynorm % SPACING;
		ypos = ynorm / SPACING;
		if(yoff <= GRID_SIZE && ypos < ATTR_NUM) {
			state->type_selected = ypos;
		}
	}
}

