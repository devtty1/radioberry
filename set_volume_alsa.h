/*
 * set_volume_alsa.h
 * (c) 2013 Luotao Fu <devtty0@gmail.com>
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

#ifndef SET_VOLUME_ALSA_H
#define SET_VOLUME_ALSA_H

#include "mcp32xx.h"
#include "set_volume.h"

#ifdef __cplusplus
 extern "C" {
#endif

int init_alsa_control(struct volume_handle *vh);
int set_alsa_master_volume(struct volume_handle *vh);
void close_alsa_ctl(struct volume_handle *vh);

#ifdef __cplusplus
 }
#endif

#endif
