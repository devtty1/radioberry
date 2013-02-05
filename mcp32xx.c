/* 
 * mcp32xx.c
 *
 * simple control interface for MCP32XX ADC in userspace using spidev 
 * (C) 2013 Luotao Fu (devtty0@gmail.com)
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
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "mcp32xx.h"

uint8_t mcp32xx_init(struct mcp32xx_dev *mcp_dev)
{
	uint8_t ret = 0;

	mcp_dev->spi_dev.dev_name = DEFAULT_SPIDEV_NAME; 
	mcp_dev->dev_conf.bits_per_word = 8;
	mcp_dev->dev_conf.speed = SPIDEV_SPEED;
	mcp_dev->dev_conf.lsb_first = 2;

	mcp_dev->spi_dev.conf = &(mcp_dev->dev_conf);


	ret = spidev_initdev(&(mcp_dev->spi_dev));

	if (ret)
		goto err_out;

	spidev_setstat(&(mcp_dev->spi_dev));
	spidev_dumpstat(&(mcp_dev->spi_dev));
	
err_out:
	return ret;
}

void mcp32xx_close(struct mcp32xx_dev *mcp_dev)
{
	spidev_removedev(&(mcp_dev->spi_dev));
}

uint32_t mcp32xx_get_val(struct mcp32xx_dev *mcp_dev, uint8_t channel_sel)
{
	/*
	 * MCP32XX has 4 bits Control sequence (MCP3204 uses only 3 of them).
	 * 1.5 Cycles (makes 2 bits) sample time. and 12 bits output. Makes 18
	 * bits all together. Hence we use 24 bits for the R/W buffer.
	 */
	uint8_t tx_buf[4], rx_buf[4];
	uint32_t output_val;
	struct spidev_device *dev = &(mcp_dev->spi_dev);

	tx_buf[0] = SINGLE_CHANNEL(channel_sel);

	spidev_do_rw(dev, tx_buf, rx_buf, 4);

	/*
	 * RX buffer would look like this
	 * |D|D|D|D|D|D|D|B11|B10|B9|B8|B7|B6|B5|B4|B3|B2|B1|B0|D|D|D|D|D|
	 * |-----rx_buf[0]---|------rx_buf[1]---------|-----rx_buf[2]----|
	 */

	output_val = ((rx_buf[0] & 0x1) << 16 | rx_buf[1] << 8 |
			(rx_buf[2] & 0xe0)) >> 5;
#if 0	
	printf("raw0: 0x%x, raw1: 0x%x, raw2: 0x%x, raw3: 0x%x\n", rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3]);
#endif
	return output_val;
}

