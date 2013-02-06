/*
 * spi_base.h
 *
 * (C)2009 Luotao Fu <l.fu@pengutronix.de>
 *
 * (C)2013 Luotao Fu <devtty0@gmai.com>
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

#ifndef SPI_BASE_H
#define SPI_BASE_H

struct spidev_config {
	uint8_t bits_per_word;
	uint32_t speed;
	uint16_t delay;
	uint8_t mode;
	uint8_t lsb_first;
};

struct spidev_device {
	const char *dev_name;
	int fd;
	struct spidev_config *conf;
};

int spidev_dumpstat(struct spidev_device *dev);
int spidev_setstat(struct spidev_device *dev);

int spidev_do_rw(struct spidev_device *dev,
		uint8_t *tx_buf, uint8_t *rx_buf, uint32_t len);

int spidev_initdev(struct spidev_device *dev);
int spidev_removedev(struct spidev_device *dev);

#endif
