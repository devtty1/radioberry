/* set_volume.c
 * (c) 2013 Luotao Fu <devtty0@gmail.com>
 *
 * simple volume control using configurable interface
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libmpd-1.0/libmpd/libmpd.h>
#include <libmpd-1.0/libmpd/debug_printf.h>

#include "set_volume_mpd.h"

int init_mpd_volume(struct volume_handle *vh)
{
	int ret = 0;

	vh->raw_max = 1 << MCP32XX_MAX_BITS;

	return ret;
}

int set_mpd_volume(struct volume_handle *vh)
{
	int cur_vol;
	uint percentage;

	cur_vol = mpd_status_get_volume(vh->mh->mpd_obj);
	if (cur_vol < 0) {
		printf("failed to get current volume through mpd: %d\n", cur_vol);
		goto out;
	}

	percentage = (vh->vol_raw_val * 100 / vh->raw_max);

#if DEBUG
	printf("val %d, raw_val %u raw_max %u\n",
			percentage, vh->vol_raw_val, vh->raw_max);
#endif
	cur_vol = mpd_status_set_volume(vh->mh->mpd_obj, percentage);
	if (cur_vol != percentage)
		printf("failed to set current volume through mpd\n");

out:
	return cur_vol;
}
