/* gpio_base.c
 * (c)2013 Luotao Fu <devtty0@gmail.com>
 *
 * basic callbacks to access gpio in userspace using sysfs interface
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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gpio_base.h"

int init_gpio(uint32_t gpio)
{
	int fd, ret = 0;
	char buf[4] = {0};
	char f_name[40] = {0};

	snprintf(f_name, sizeof(f_name), "/sys/class/gpio/gpio%d/direction",
			gpio);

	/* return if already exported and ready to be set */
	if (! access(f_name, W_OK))
		goto out;

	fd = open("/sys/class/gpio/export", O_WRONLY);

	if (fd < 0) {
		printf("%d: ", gpio);
		perror("failed to initialize gpio");
		ret = 1;
		goto out;
	}

	snprintf(buf, sizeof(buf), "%d", gpio);

	if (write(fd, buf, strlen(buf)) < 0) {
		printf("%d: ", gpio);
		perror("failed to initialize gpio");
		ret = 1;
	}

	close(fd);
out:
	return ret;
}

int close_gpio(uint32_t gpio)
{
	int fd, ret = 0;
	char buf[4] = {0};

	fd = open("/sys/class/gpio/unexport", O_WRONLY);

	if (fd < 0) {
		printf("%d: ", gpio);
		perror("failed to close gpio");
		ret = 1;
		goto out;
	}

	snprintf(buf, 4, "%d", gpio);

	if (write(fd, buf, strlen(buf)) < 0) {
		printf("%d: ", gpio);
		perror("failed to close gpio");
		ret = 1;
	}

	close(fd);
out:
	return ret;
}

int set_gpio_dir(uint32_t gpio, uint8_t in_out)
{
	int fd, ret = 0, w_res = 0;
	char f_name[40] = {0};

	snprintf(f_name, sizeof(f_name), "/sys/class/gpio/gpio%d/direction",
			gpio);

	fd = open(f_name, O_WRONLY);
	if (fd < 0) {
		printf("%d: ", gpio);
		perror("failed to set gpio direction");
		ret = 1;
		goto out;
	}

	if (in_out == OUT_GPIO)
		w_res = write(fd, "out", 3);
	else
		w_res = write(fd, "in", 2);

	if (w_res < 0) {
		printf("%d: ", gpio);
		perror("failed to set gpio direction");
		ret = 1;
	}

	close(fd);
out:
	return ret;
}

int gpio_get_val_fd(uint32_t gpio)
{
	int ret, fd;
	char f_name[40] = {0};

	snprintf(f_name, sizeof(f_name), "/sys/class/gpio/gpio%d/value",
			gpio);

	fd = open(f_name, O_RDWR);
	if (fd < 0) {
		printf("%d: ", gpio);
		perror("failed to get gpio value control");
		ret = -1;
	} else
		ret = fd;

	return ret;
}

int set_gpio_val_by_fd(int fd, uint8_t val)
{
	int ret = 0;
	char buf[2] = {0};

	snprintf(buf, sizeof(buf), "%d", val);

	if (write(fd, buf, strlen(buf)) < 0)
		ret = 1;

	return ret;
}

int set_gpio_val(uint32_t gpio, uint8_t val)
{
	int fd, ret = 0;

	fd = gpio_get_val_fd(gpio);
	if (fd < 0) {
		printf("GPIO %d: failed to get value control\n", gpio);
		ret = 1;

		goto out;
	}

	if (set_gpio_val_by_fd(fd, val) != 0) {
		printf("GPIO %d: failed to set value\n", gpio);
		ret = 1;
	}

	close(fd);
out:
	return ret;
}
