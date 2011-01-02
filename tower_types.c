/* @file tower_types.c
 * @brief tower/bullet properties by type
 */
 
#include "tower_types.h"
#include "tower_types_internal.h"

struct tt_data_t tt_data[TT_NUM] = {
	[TT_PLASMA] = {
		.color = {0.5, 0.5, 1.0},
		.dscr = "plasma",
		.tower = {tower_upd_cw, 100.0},
		.bullet = {bullet_upd_cw   , bullet_draw_beam, BULLET_MAX_AGE},
	},
	[TT_LTNG] = {
		.color = {0.5, 0.5, 1.0},
		.dscr = "lightning",
		.tower = {tower_upd_normal, 80.0},
		.bullet = {bullet_upd_pulse, bullet_draw_beam, BULLET_PULSE_MAX_AGE},
	},
	[TT_CW] = {
		.color = {1.0, 0.0, 0.0},
		.dscr = "continuous laser",
		.tower = {tower_upd_cw, 160.0},
		.bullet = {bullet_upd_cw   , bullet_draw_beam, BULLET_MAX_AGE},
	},
	[TT_PULSE] = {
		.color = {1.0, 0.0, 0.0},
		.dscr = "pulsed laser",
		.tower = {tower_upd_normal, 160.0},
		.bullet = {bullet_upd_pulse, bullet_draw_beam, BULLET_PULSE_MAX_AGE},
	},
	[TT_APCR] = {
		.color = {0.4, 0.8, 0.4},
		.dscr = "AP gun",
		.tower = {tower_upd_normal, 240.0},
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_HIT_MAX_AGE},
	},
	[TT_APFSDS] = {
		.color = {0.4, 0.8, 0.4},
		.dscr = "APFSDS gun",
		.tower = {tower_upd_normal, 120.0},
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_HIT_MAX_AGE},
	},
	[TT_DU] = {
		.color = {0.4, 0.8, 0.4},
		.dscr = "APFSDS-DU gun",
		.tower = {tower_upd_normal, 120.0},
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_HIT_MAX_AGE},
	},
	[TT_HE] = {
		.color = {1.0, 0.5, 0.0},
		.dscr = "high explosive gun",
		.tower = {tower_upd_normal, 120.0},
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_HIT_MAX_AGE},
	},
	[TT_HEAT] = {
		.color = {1.0, 0.5, 0.0},
		.dscr = "HEAT projectile gun",
		.tower = {tower_upd_normal, 120.0},
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_HIT_MAX_AGE},
	},
	[TT_AREA] = {
		.color = {1.0, 0.5, 0.0},
		.dscr = "damage field",
		.tower = {tower_upd_normal, 80.0},
		.bullet = {bullet_upd_pulse, bullet_draw_area, 500},
	},
	[TT_BASIC] = {
		.color = {0.6, 0.6, 0.6},
		.dscr = "basic gun",
		.tower = {tower_upd_normal, 120.0},
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_HIT_MAX_AGE},
	},
};
