/* 
 * set_tuner.h
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

#ifndef SET_TUNER_H
#define SET_TUNER_H

#include "mpd_ctl.h"

#define DEFAULT_PLS_NAME "stations"

int load_stations_playlist(struct mpd_handle *m_handle);
int set_tuner(struct mpd_handle *m_handle);

#endif
