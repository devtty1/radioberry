#ifndef SET_VOLUME_H
#define SET_VOLUME_H

#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include "config_ctl.h"
#include "mpd_ctl.h"

struct volume_handle {
	snd_mixer_t *snd_handle;
	snd_mixer_selem_id_t *snd_sid;
	snd_mixer_elem_t* snd_elem;

	struct mpd_handle *mh;

	uint32_t vol_raw_val;
	uint32_t raw_max;
	uint8_t max_vol_steps;
};

#ifdef __cplusplus
 extern "C" {
#endif

int init_vol_control(struct volume_handle *vh);
void close_vol_ctl(struct volume_handle *vh);
int set_volume(struct volume_handle *vh);

#ifdef __cplusplus
 }
#endif

#endif
