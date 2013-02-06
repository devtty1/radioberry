/* set_tuner.c
 * simple tuner control using mpd interface
 *
 * (C) 2013 Luotao Fu <devtty0@gmail.com>
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libmpd-1.0/libmpd/libmpd.h>
#include <libmpd-1.0/libmpd/debug_printf.h>

#include "set_tuner.h"

int load_stations_playlist(struct mpd_handle *m_handle)
{
	int ret = 0;
	const char* path = DEFAULT_PLS_NAME;

	ret = mpd_playlist_queue_load(m_handle->mpd_obj, path);

	if (!ret)
		ret = mpd_playlist_queue_commit(m_handle->mpd_obj);

	return ret;
}

int set_tuner(struct mpd_handle *m_handle)
{
	int ret = 0;

	int station_pos = m_handle->tuner_raw_val /
		(m_handle->raw_max / m_handle->max_tuner_pos);

	mpd_Song *mpd_song = NULL;

	mpd_song = mpd_playlist_get_song_from_pos
		(m_handle->mpd_obj, station_pos);

#ifdef DEBUG
	printf("raw_val: %d, raw_max: %d, max tuner pos: %d,"
			" tuner position: %d, id: %d\n",
			m_handle->tuner_raw_val, m_handle->raw_max,
			m_handle->max_tuner_pos, station_pos,
			m_handle->mpd_song->id);
#endif

	if (mpd_song != NULL)
		ret = mpd_player_play_id(m_handle->mpd_obj,
				mpd_song->id);
	else {
		printf("No stations found at %d\n", station_pos);
		ret = 1;
	}

	return ret;
}
