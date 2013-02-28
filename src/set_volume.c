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

int init_vol_control(struct volume_handle *vh)
{
	int ret = 0;

//	ret = init_alsa_control(vh);

	ret = init_mpd_volume(vh);

	return ret;
}

int set_volume(struct volume_handle *vh)
{
	int ret = 0;

	set_mpd_volume(vh);
//	ret = set_alsa_master_volume(vh);

	return ret;
}

void close_vol_ctl(struct volume_handle *vh)
{
//	close_alsa_ctl(vh);
}
