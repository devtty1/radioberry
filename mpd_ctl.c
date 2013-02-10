/*
 * mpd_ctl.c
 * (c)2013 Luotao Fu <devtty0@gmail.com>
 *
 * control interface for mpd using libmpd
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

#include "mcp32xx.h"
#include "mpd_ctl.h"

void status_changed(MpdObj *mi, ChangedStatusType what)
{
	if (what & MPD_CST_PLAYLIST )
		printf("playlist changed!! \n");
}

/* compare the stored song info with the current song and update, if necessary,
 * the line buffer for the lcd display. */
void update_song_info(struct mpd_handle *mh)
{
	char old_name[MAX_LINE_BUF + 1] = {0};
	char old_title[MAX_LINE_BUF + 1] = {0};
	char new_name[MAX_LINE_BUF + 1] = {0};
	char new_title[MAX_LINE_BUF + 1] = {0};

	if (mh->mpd_song != NULL) {
		if (mh->mpd_song->name != NULL)
			strncpy(old_name, mh->mpd_song->name, MAX_LINE_BUF);

		if (mh->mpd_song->title != NULL)
			strncpy(old_title, mh->mpd_song->title, MAX_LINE_BUF);
	}

	/* mpd_playlist_get_current_song has a bug, so that in most case the
	 * title will not be fetched while play stream. To work around this we
	 * use the low-level library libmpdclient here */
	mh->mpd_song = mpd_playlist_get_song(mh->mpd_obj,
			mpd_player_get_current_song_id(mh->mpd_obj));

	if (mh->mpd_song == NULL) {
		memset(mh->lh->fline_buf, 0, MAX_LINE_BUF + 1);
		memset(mh->lh->sline_buf, 0, MAX_LINE_BUF + 1);
		return;
	}

	if (mh->mpd_song->name != NULL) {
		strncpy(new_name, mh->mpd_song->name, MAX_LINE_BUF);

		if (strlen(mh->mpd_song->name) > MAX_LINE_BUF)
				new_name[MAX_LINE_BUF] = '\0';
	}

	if (mh->mpd_song->title != NULL) {
		strncpy(new_title, mh->mpd_song->title, MAX_LINE_BUF);

		if (strlen(mh->mpd_song->title) > MAX_LINE_BUF)
				new_title[MAX_LINE_BUF] = '\0';
	}

#ifdef DEBUG
	printf("O_NAME: %s O_TITLE: %s \n"
		"N_NAME: %s N_TITLE: %s\n"
		"diff: %d, tdiff: %d\n",
		old_name, old_title, new_name, new_title,
		strcmp(old_name, new_name),
		strcmp(old_title, new_title));
#endif

	/* In Internet radio mode the name field contains the station name,
	 * while the title field contains the songtitle. (not provided by all
	 * stations). So the name goes to first line and the title goes to the
	 * second */
	if (strcmp(old_name, new_name) != 0) {
		mh->lh->fline_update = 1;
		mh->lh->fline_idx = 0;
		strncpy(mh->lh->fline_buf, new_name, MAX_LINE_BUF + 1);
	}

	if (strcmp(old_title, new_title) != 0) {
		mh->lh->sline_update = 1;
		mh->lh->sline_idx = 0;
		strncpy(mh->lh->sline_buf, new_title, MAX_LINE_BUF + 1);
	}
}

int init_mpd_handle(struct mpd_handle *m_handle)
{
	int ret = 0;

	m_handle->raw_max = 1 << MCP32XX_MAX_BITS;
	m_handle->max_tuner_pos = MAX_TUNER_POS;
	m_handle->host = DEFAULT_HOST;
	m_handle->port = DEFAULT_PORT;

	printf("host %s, port %d\n", m_handle->host, m_handle->port);

	m_handle->mpd_obj = mpd_new(m_handle->host, m_handle->port,
			m_handle->password);

	mpd_signal_connect_status_changed(m_handle->mpd_obj,
			(StatusChangedCallback)status_changed, NULL);

	ret = mpd_connect(m_handle->mpd_obj);

	return ret;
}

void close_mpd_handle(struct mpd_handle *m_handle)
{
	mpd_player_stop(m_handle->mpd_obj);
	mpd_free(m_handle->mpd_obj);
}

