/* 
 * config_ctl.h
 * 
 * (C)2013 Luotao Fu <devtty0@gmail.com>
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

#ifndef CONFIG_CTL_H
#define CONFIG_CTL_H

/* Radioberry Main interface configuration */
/* interval(us) to go through all ADC channels and update LCD Screen */
#define UPDATE_INTERVAL 300000
/* Interval(us) to update song info */
#define SONG_UPDATE_INTERVAL 2000000

/* channel amount on the ADC and assigment of the channels */
#define CHANNEL_USED 2
#define VOLUME_CHAN 0
#define TUNER_CHAN 1

/* LCD configuration */
/*RS: Register select */
#define LCD_PIN_RS 25
/*E: "Enable" -> Clock */
#define LCD_PIN_E 24
/*DB: Data-Bit 4-7 (4-bit operation)*/
#define LCD_PIN_DB4 23
#define LCD_PIN_DB5 17
#define LCD_PIN_DB6 27
#define LCD_PIN_DB7 22

#define MAX_LINE_CHAR 16
#define MAX_LINE_BUF 80

#define GPIO_PINS_USED 6

/* maximum of supported chars outside 7bit ASCII */
#define MAX_EXT_CHARS 40
/* MPD configuration */
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 6600
#define MAX_TUNER_POS 12

/* volume control configuration */
#define MAX_VOL_STEPS 50

/* tuner control configuration */
#define DEFAULT_PLS_NAME "stations"

#endif
