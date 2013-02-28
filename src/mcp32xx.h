/*
 * mcp32xx.h
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

#ifndef MCP32XX_H
#define MCP32XX_H

#include "spi_base.h"

#define DEFAULT_SPIDEV_NAME "/dev/spidev0.0"
#define SPIDEV_SPEED 20000
#define MCP32XX_MAX_CHANNEL 8
#define MCP32XX_MAX_BITS 12
#define TOLERANT_THRESHOLD 50

#define START_BIT (1 << 7)
#define SINGLE_ENDED (1 << 6)
#define DIFFERENTIAL 0
#define SINGLE_CHANNEL(c) (START_BIT | SINGLE_ENDED | (c << 3))
/* a is the positive channel*/
#define DIFF_CHANNEL(a, b) (START_BIT | DIFFERENTIAL | (a << 3))

struct mcp32xx_dev {
	struct spidev_device spi_dev;
	struct spidev_config dev_conf;
};

#ifdef __cplusplus
 extern "C" {
#endif

uint32_t mcp32xx_get_val(struct mcp32xx_dev *mcp_dev, uint8_t channel_sel);
uint8_t mcp32xx_init(struct mcp32xx_dev *mcp_dev);
void mcp32xx_close(struct mcp32xx_dev *mcp_dev);

#ifdef __cplusplus
 }
#endif

#endif
