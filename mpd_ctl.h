/* 
 * mpd_ctl.h
 * 
 * (C)2013 Luotao Fu <devtty0@gmail.com>
 *
 * This file is part of radioberry.
 * radioberry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * radioberry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with radioberry.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SET_MPD_CTL_H
#define SET_MPD_CTL_H

#include <libmpd-1.0/libmpd/libmpd.h>
#include <libmpd-1.0/libmpd/debug_printf.h>
#include "config_ctl.h"
#include "lcd_ctl.h"

struct mpd_handle {
	char* host;
	int port;
	char* password;
	MpdObj *mpd_obj;
	mpd_Song *mpd_song;
	struct lcd_handle *lh;

	uint32_t tuner_raw_val;
	uint32_t raw_max;
	uint8_t max_tuner_pos;
};

int init_mpd_handle(struct mpd_handle *m_handle);
void close_mpd_handle(struct mpd_handle *m_handle);
void update_song_info(struct mpd_handle *mh);

#endif
