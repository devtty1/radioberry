/*
 * radioberry.c
 * main interface
 * (C) 2013 Luotao Fu (devtty0@gmail.com)
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
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>

#include <confuse.h>

#include "mcp32xx.h"
#include "set_volume.h"
#include "mpd_ctl.h"
#include "set_tuner.h"
#include "lcd_ctl.h"

#include "radioberry.h"

static uint8_t running;
static uint8_t vol_control_enabled = 1;
static uint8_t tuner_control_enabled = 1;
static uint8_t lcd_control_enabled = 1;
static uint8_t volume_chan;
static uint8_t tuner_chan;
static uint8_t channel_used;

void  sigintterm_handler(int sig)
{
	printf("\n\n\nExiting Radioberry\n");
	running = 0;
}

static int process_value(struct volume_handle *vh, struct mpd_handle *mh,
		uint32_t val, uint8_t adc_chan)
{
	int8_t ret = 0, count = 0;

	/* mpd connection is sometimes invalid (especially while loading first
	 * time after start up) at this point, though the first mpd_connect
	 * reports sucess. Appears to be a bug in libmpd. Hence we check the
	 * connection again here and do reconnecting if necessary. */
	while (!mpd_check_connected(mh->mpd_obj)) {
		mpd_connect(mh->mpd_obj);
		usleep(500000);

		count++;
		if (count > 10) {
			printf("time out trying reconnecting to mpd\n");
			ret = 1;
			goto out;
		}
	}

	if (adc_chan == volume_chan) {
		if (! vol_control_enabled)
			goto out;

		vh->vol_raw_val = val;
		set_volume(vh);
	} else if (adc_chan == tuner_chan) {
		if (! tuner_control_enabled)
			goto out;

		mh->tuner_raw_val = val;
		/* check result of set_tuner. If set_tuner failed, we have NULL
		 * pointers to mpd_song and hence might have segfaults. */
		ret = set_tuner(mh);
	}

out:
	return ret;
}

int main(int argc, const char *argv[])
{
	uint8_t ret;
	uint8_t init = 1;
	cfg_t *cfg;

       	int32_t adc_channel = -1;
	uint32_t val_cur[MCP32XX_MAX_CHANNEL], val_old[MCP32XX_MAX_CHANNEL];
	int32_t diff;

	struct mcp32xx_dev mcp_dev;
	struct volume_handle v_handle = {0};
	struct mpd_handle m_handle = {0};
	struct lcd_handle l_handle = {0};

	useconds_t sleep_usec;
	uint8_t song_update_count_max;
	uint8_t song_update_count = 0;

	cfg_opt_t opts[] = {
		CFG_INT("UpdateInterval", 300000, CFGF_NONE),
		CFG_INT("SongUpdateInterval", 2000000, CFGF_NONE),
		CFG_INT("ChannelUsed", 2, CFGF_NONE),
		CFG_INT("VolumeChannel", 0, CFGF_NONE),
		CFG_INT("TunerChannel", 1, CFGF_NONE),
		CFG_INT("MaxLineChar", 16, CFGF_NONE),
		CFG_INT("MaxLineBuf", 80, CFGF_NONE),
		CFG_STR("HostName", "localhost", CFGF_NONE),
		CFG_INT("Port", 6600, CFGF_NONE),
		CFG_INT("MaxTunerPos", 12, CFGF_NONE),
		CFG_INT("MaxVolSteps", 50, CFGF_NONE),
		CFG_STR("RadioPlaylistName", "stations", CFGF_NONE),
		CFG_INT("LCD-Pin_RS", 25, CFGF_NONE),
		CFG_INT("LCD-Pin_E", 24, CFGF_NONE),
		CFG_INT("LCD-Pin_DB4", 23, CFGF_NONE),
		CFG_INT("LCD-Pin_DB5", 17, CFGF_NONE),
		CFG_INT("LCD-Pin_DB6", 27, CFGF_NONE),
		CFG_INT("LCD-Pin_DB7", 22, CFGF_NONE),
		CFG_STR("FillPatternFirstLine", " ++ ", CFGF_NONE),
		CFG_STR("FillPatternSecondLine", " ** ", CFGF_NONE),
		CFG_INT("MaxLineLength", 16, CFGF_NONE),
		CFG_INT("MaxLineBufferLength", 80, CFGF_NONE),
        	CFG_END()
	};

	cfg = cfg_init(opts, CFGF_NONE);

	ret = cfg_parse(cfg, "/etc/radioberry.conf");
	if(ret == CFG_FILE_ERROR) {
		perror("/etc/radioberry.conf");
		printf("WARN: could not read /etc/radioberry.conf,"
				" process with default values\n");
	} else if (ret == CFG_PARSE_ERROR) {
		printf("configuration parse error\n");
		goto out;
	}

	/* get configuration values */
	channel_used = (uint8_t)cfg_getint(cfg, "ChannelUsed");
	volume_chan = (uint8_t)cfg_getint(cfg, "VolumeChannel");
	tuner_chan = (uint8_t)cfg_getint(cfg, "TunerChannel");

	sleep_usec = cfg_getint(cfg, "UpdateInterval") / channel_used;
	printf("sleep_usec %d\n", sleep_usec);
	song_update_count_max = cfg_getint(cfg, "SongUpdateInterval")
		/ sleep_usec;
	/* pass over pointer to handles of mpd, lcd and volume */
	m_handle.lh = &l_handle;
	v_handle.mh = &m_handle;

	m_handle.cfg = cfg;
	l_handle.cfg = cfg;

	signal(SIGINT, sigintterm_handler);
	signal(SIGTERM, sigintterm_handler);

	ret = lcd_init(&l_handle);
	if (ret) {
		printf("failed to initialize lcd, lcd control disabled \n");
		lcd_control_enabled = 0;
	}

	if (lcd_control_enabled) {
		lcd_print_string("Radioberry");
		lcd_move_cursor_down();
		lcd_print_string("=== Welcome ===");
	}

	ret = mcp32xx_init(&mcp_dev);
	if (ret) {
		printf("failed to initialize spi device, analog control"
				" devices disabled\n");
		tuner_control_enabled = 0;
		vol_control_enabled = 0;
	}

	ret = init_mpd_handle(&m_handle);
	if (ret) {
		printf("could not initialize mpd client,"
				" tuner control disabled\n");
		tuner_control_enabled = 0;
	}

	/* disable tuner control if we cannot get any station at all. If one
	 * station fails, let's process and wait for tuner changes */
	ret = load_stations_playlist(&m_handle);
	if (ret == 1) {
		printf("could not load radio stations list,"
				" tuner control disabled\n");
		tuner_control_enabled = 0;
	}

	ret = init_vol_control(&v_handle);
	if (ret) {
		printf("could not initialize volume control,"
				" volumen control disabled\n");
		vol_control_enabled = 0;
	}

	running = 1;

	while ( running ) {
		usleep(sleep_usec);

		if (adc_channel + 1 == channel_used) {
			adc_channel = 0;
			mpd_status_update(m_handle.mpd_obj);
			/* detect if initial loop is done. Start update lcd
			 * after initial loop */
			if (init)
				init = 0;
			else {
				if (lcd_control_enabled)
					lcd_update_screen(&l_handle);
			}
		} else
			adc_channel++;

		if (song_update_count == song_update_count_max) {
			update_song_info(&m_handle);
			song_update_count = 0;
		} else
			song_update_count++;

		/* loop over all channels in singel ended mode
		 * and reset counter in the last loop. */
		val_cur[adc_channel] = mcp32xx_get_val(&mcp_dev, adc_channel);

		/* fill the old value buffer and intialize the player with
		 * current settings*/
		if (init) {
			val_old[adc_channel] = val_cur[adc_channel];

			ret = process_value(&v_handle, &m_handle,
					val_cur[adc_channel], adc_channel);

			/* skip comparing old/new values while intializing */
			if (ret) {
				printf("unrecoverable error, quit now\n");
				break;
			}
			else
				continue;
		}

		diff = val_old[adc_channel] - val_cur[adc_channel];

#ifdef DEBUG
		printf("adc_channel: %u, old %u, new %u, diff %d\n",
				adc_channel, val_cur[adc_channel],
			       	val_old[adc_channel], diff);
#endif

		if (abs(diff) < TOLERANT_THRESHOLD)
			continue;
#ifdef DEBUG
		if (diff > 0)
			printf("Poti %u was turned left, current val: %d\n",
				adc_channel + 1, val_cur[adc_channel]);
		else
			printf("Poti %u was turned right, current Pos: %d\n",
				adc_channel + 1, val_cur[adc_channel]);
#endif
		process_value(&v_handle, &m_handle, val_cur[adc_channel],
				adc_channel);
		val_old[adc_channel] = val_cur[adc_channel];
	}
out:
	lcd_clear_screen();
	lcd_close(&l_handle);
	mcp32xx_close(&mcp_dev);
	close_vol_ctl(&v_handle);
	close_mpd_handle(&m_handle);

	exit(ret);
}

