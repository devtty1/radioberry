/* set_volume.c
 * simple volume control using configurable interface
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "set_volume_alsa.h"
#include "set_volume_mpd.h"
#include "set_volume.h"

static void update_vol_display(struct lcd_handle *lh, int cur_vol_perc)
{
	uint count;
	uint num_blocks = cur_vol_perc * lh->max_line_char/ 100;
	char blocks[lh->max_line_char + 1];

	memset(blocks, 0, (lh->max_line_char + 1) * sizeof(char));

	for (count = 0; count < num_blocks; count++)
		blocks[count] = 0xff;

	snprintf(lh->fline_buf, lh->max_line_char, "VOLUME");
	snprintf(lh->sline_buf, lh->max_line_char, blocks);

	lh->fline_update = 1;
	lh->sline_update = 1;

	/* hold volume display for 4 cycles. Holding can only be cancelled if
	 * new volume is set. */
	lh->wait_cycle = 4;
	lh->cancel_wait = 1;
}

int init_vol_control(struct volume_handle *vh)
{
	int ret = 0;

//	ret = init_alsa_control(vh);

	ret = init_mpd_volume(vh);

	return ret;
}

int set_volume(struct volume_handle *vh)
{
	int ret = 0, cur_vol_perc;

	cur_vol_perc = set_mpd_volume(vh);
//	ret = set_alsa_master_volume(vh);

	if (cur_vol_perc >= 0)
		update_vol_display(vh->mh->lh, cur_vol_perc);

	return ret;
}

void close_vol_ctl(struct volume_handle *vh)
{
//	close_alsa_ctl(vh);
}
