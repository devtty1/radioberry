/* set_volume.c
 * (C) 2013 Luotao Fu <devtty0@gmail.com>
 *
 * simple volume control using alsa interface
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

#include <linux/types.h>

#include "set_volume_alsa.h"

#define CARD_NAME "default"
#define ELEM_NAME "PCM"

int init_alsa_control(struct volume_handle *vh)
{
	int ret = 0;

	vh->raw_max = 1 << MCP32XX_MAX_BITS;
	vh->max_vol_steps = MAX_VOL_STEPS;

	ret = snd_mixer_open(&(vh->snd_handle), 0);
	if (ret)
		goto out;

	ret = snd_mixer_attach(vh->snd_handle, CARD_NAME);
	if (ret)
		goto out;

	ret = snd_mixer_selem_register(vh->snd_handle, NULL, NULL);
	if (ret)
		goto out;

	ret = snd_mixer_load(vh->snd_handle);
	if (ret)
		goto out;

	snd_mixer_selem_id_alloca(&(vh->snd_sid));
	snd_mixer_selem_id_set_index(vh->snd_sid, 0);
	snd_mixer_selem_id_set_name(vh->snd_sid, ELEM_NAME);

	vh->snd_elem = snd_mixer_find_selem(vh->snd_handle, vh->snd_sid);
	if (vh->snd_elem == NULL) {
		printf("no snd element");
		ret = 1;
	}
out:
	if (ret)
		printf("failed to initialize alsa interface\n");
	return ret;
}

int set_alsa_master_volume(struct volume_handle *vh)
{
	int ret = 0;
	long min, max, val;

	snd_mixer_selem_get_playback_volume_range(vh->snd_elem, &min, &max);

	/*
	 * FIXME: Alsa Volume ist in dB and hence logarithm. The linear
	 * calculation works very poor, since the chip mutes at about half
	 * position of the poti.
	 *
	 * */
	uint percentage = (vh->vol_raw_val * 100 / vh->raw_max);
	val = min + (percentage * (max - min) / 100);

#ifdef DEBUG
	printf("Vol min %ld, max %ld, val %ld, raw_val %u raw_max %u\n",
			min, max, val, vh->vol_raw_val, vh->raw_max);
#endif
	ret = snd_mixer_selem_set_playback_volume_all(vh->snd_elem, val);

	return ret;
}

void close_alsa_ctl(struct volume_handle *vh)
{
	snd_mixer_close(vh->snd_handle);
}
