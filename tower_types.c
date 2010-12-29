/* @file tower_types.c
 * @brief tower/bullet properties by type
 */
 
#include "tower_types.h"
#include "tower_types_internal.h"

struct tt_data_t tt_data[TT_NUM] = {
	[TT_PLASMA] = {
		.color = {0.5, 0.5, 1.0},
		.dscr = "plasma",
		.tower_upd = tower_upd_cw,
		.bullet = {bullet_upd_cw   , bullet_draw_beam, BULLET_MAX_AGE},
	},
	[TT_LTNG] = {
		.color = {0.5, 0.5, 1.0},
		.dscr = "lightning",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_pulse, bullet_draw_beam, BULLET_PULSE_MAX_AGE},
	},
	[TT_CW] = {
		.color = {1.0, 0.0, 0.0},
		.dscr = "continuous laser",
		.tower_upd = tower_upd_cw,
		.bullet = {bullet_upd_cw   , bullet_draw_beam, BULLET_MAX_AGE},
	},
	[TT_PULSE] = {
		.color = {1.0, 0.0, 0.0},
		.dscr = "pulsed laser",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_pulse, bullet_draw_beam, BULLET_PULSE_MAX_AGE},
	},
	[TT_APCR] = {
		.color = {0.4, 0.8, 0.4},
		.dscr = "AP gun",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_MAX_AGE},
	},
	[TT_APFSDS] = {
		.color = {0.4, 0.8, 0.4},
		.dscr = "APFSDS gun",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_MAX_AGE},
	},
	[TT_DU] = {
		.color = {0.4, 0.8, 0.4},
		.dscr = "APFSDS-DU gun",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_MAX_AGE},
	},
	[TT_HE] = {
		.color = {1.0, 0.5, 0.0},
		.dscr = "high explosive gun",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_MAX_AGE},
	},
	[TT_HEAT] = {
		.color = {1.0, 0.5, 0.0},
		.dscr = "HEAT projectile gun",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_MAX_AGE},
	},
	[TT_AREA] = {
		.color = {1.0, 0.5, 0.0},
		.dscr = "damage field",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_MAX_AGE},
	},
	[TT_BASIC] = {
		.color = {0.6, 0.6, 0.6},
		.dscr = "basic gun",
		.tower_upd = tower_upd_normal,
		.bullet = {bullet_upd_proj , bullet_draw_proj, BULLET_MAX_AGE},
	},
};
