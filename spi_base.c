/*
 * spi_base.c
 *
 * (C)2009 Luotao Fu <l.fu@pengutronix.de>
 *
 * (C)2013 Luotao Fu <devtty0@gmai.com>
 *
 * spidev userspace access callbacks
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
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "spi_base.h"

int spidev_initdev(struct spidev_device *dev)
{
	/* set some dummy values */
	dev->conf->mode = 4;
	dev->conf->lsb_first = 2;

	dev->fd = open(dev->dev_name, O_RDWR);
	if (dev->fd < 0) {
		perror("error while opening spi device");
		return 1;
	}
	return 0;
}

int spidev_removedev(struct spidev_device *dev)
{
	close(dev->fd);
	return 0;
}

int spidev_setstat(struct spidev_device *dev)
{

	if (dev->conf->bits_per_word > 0) {
		if (ioctl(dev->fd, SPI_IOC_WR_BITS_PER_WORD,
					&dev->conf->bits_per_word) < 0) {
			perror("SPI_IOC_WR_BITS_PER_WORD");
			return 1;
		}
	}

	if (dev->conf->speed > 0) {
		if (ioctl(dev->fd, SPI_IOC_WR_MAX_SPEED_HZ,
					&dev->conf->speed) < 0) {
			perror("SPI_IOC_WR_MAX_SPEED_HZ");
			return 1;
		}
	}

	if (dev->conf->mode < 4) {
		if (ioctl(dev->fd, SPI_IOC_WR_MODE,
					dev->conf->mode) < 0) {
			perror("SPI_IOC_WR_MODE");
			return 1;
		}
	}

	if (dev->conf->lsb_first < 2) {
		if (ioctl(dev->fd, SPI_IOC_WR_LSB_FIRST,
				dev->conf->lsb_first) < 0) {
			perror("SPI_IOC_WR_LSB_FIRST");
			return 1;
		}
	}

	return 0;
}

int spidev_dumpstat(struct spidev_device *dev)
{
	__u8	mode, lsb, bits;
	__u32	speed;

	if (ioctl(dev->fd, SPI_IOC_RD_MODE, &mode) < 0) {
		perror("SPI rd_mode");
		return 1;
	}

	if (ioctl(dev->fd, SPI_IOC_RD_LSB_FIRST, &lsb) < 0) {
		perror("SPI rd_lsb_fist");
		return 1;
	}

	if (ioctl(dev->fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0) {
		perror("SPI bits_per_word");
		return 1;
	}

	if (ioctl(dev->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
		perror("SPI max_speed_hz");
		return 1;
	}

	printf("%s: spi mode %d, %d bits %sper word, %d Hz max\n",
		dev->dev_name, mode, bits, lsb ? "(lsb first) " : "", speed);

	return 0;
}

int spidev_do_rw(struct spidev_device *dev,
		uint8_t *tx, uint8_t *rx, uint32_t len)
{
	int ret;

	if (rx != NULL)
		memset(rx, 0, len);

	struct spi_ioc_transfer tr = {
		.tx_buf = (__u64)((unsigned long)tx),
		.rx_buf = (__u64)((unsigned long)rx),
		.len = len,
		.delay_usecs = dev->conf->delay,
	};

	ret = ioctl(dev->fd, SPI_IOC_MESSAGE(1), &tr);

	if (ret == 1)
		perror("error while sending spi message");

	return ret;
}
