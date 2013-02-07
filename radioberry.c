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

#include "mcp32xx.h"
#include "set_volume.h"
#include "mpd_ctl.h"
#include "set_tuner.h"
#include "lcd_ctl.h"

#include "radioberry.h"

/* interval(us) to go through all ADC channels and update LCD Screen */
#define UPDATE_INTERVAL 300000
/* Interval(us) to update song info */
#define SONG_UPDATE_INTERVAL 2000000

static uint8_t running;
static uint8_t vol_control_enabled = 1;
static uint8_t tuner_control_enabled = 1;
static uint8_t lcd_control_enabled = 1;

void  sigint_handler(int sig)
{
	printf("\n\n\nCtrl-C received, exiting test\n");
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

	switch (adc_chan) {
	case VOLUME_CHAN:
		if (! vol_control_enabled) {
			ret = 1;
			goto out;
		}

		vh->vol_raw_val = val;
		set_volume(vh);

		break;

	case TUNER_CHAN:
		if (! tuner_control_enabled) {
			ret = 1;
			goto out;
		}

		mh->tuner_raw_val = val;
		set_tuner(mh);

		break;

	default:
		break;
	}

out:
	return ret;
}

int main(int argc, const char *argv[])
{
	uint8_t ret;
	uint8_t init = 1, channel_used = CHANNEL_USED;

       	int32_t adc_channel = -1;
	uint32_t val_cur[MCP32XX_MAX_CHANNEL], val_old[MCP32XX_MAX_CHANNEL];
	int32_t diff;

	struct mcp32xx_dev mcp_dev;
	struct volume_handle v_handle = {0};
	struct mpd_handle m_handle = {0};
	struct lcd_handle l_handle = { .fline_buf = {0}, .sline_buf = {0} };

	useconds_t sleep_usec = UPDATE_INTERVAL / channel_used;
	uint8_t song_update_count = 0;
	uint8_t song_update_count_max = SONG_UPDATE_INTERVAL / sleep_usec;

	m_handle.lh = &l_handle;
	v_handle.mh = &m_handle;

	signal(SIGINT, sigint_handler);

	ret = lcd_init();
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
		printf("failed to initialize spi device. Exiting\n");
		goto err_init;
	}

	ret = init_mpd_handle(&m_handle);
	if (ret) {
		printf("could not initialize mpd client,"
				" tuner control was disabled\n");
		tuner_control_enabled = 0;
	}

	if (tuner_control_enabled) {
		ret = load_stations_playlist(&m_handle);

		if (ret) {
			printf("could not load radio stations list,"
				" tuner control was disabled\n");
		tuner_control_enabled = 0;
		}
	}

	ret = init_vol_control(&v_handle);
	if (ret) {
		printf("could not initialize volume control,"
				" this function was disabled\n");
		vol_control_enabled = 0;
	}

	running = 1;

	while ( running ) {
		usleep(sleep_usec);

		if (adc_channel + 1 == CHANNEL_USED) {
			adc_channel = 0;

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

			process_value(&v_handle, &m_handle,
					val_cur[adc_channel], adc_channel);
			continue;
		}

		diff = val_old[adc_channel] - val_cur[adc_channel];

#if DEBUG
		printf("adc_channel: %u, old %u, new %u, diff %d\n",
				adc_channel, val_cur[adc_channel],
			       	val_old[adc_channel], diff);
#endif

		if (abs(diff) < TOLERANT_THRESHOLD)
			continue;
#if DEBUG
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

	lcd_clear_screen();
	lcd_close();
	mcp32xx_close(&mcp_dev);
	close_vol_ctl(&v_handle);
	close_mpd_handle(&m_handle);

err_init:
	exit(ret);
}

