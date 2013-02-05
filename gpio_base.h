/* 
 * gpio_base.h
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

#ifndef GPIO_BASE_H
#define GPIO_BASE_H

#define OUT_GPIO 1
#define IN_GPIO 0

int init_gpio(uint8_t gpio);
int set_gpio_dir(uint8_t gpio, uint8_t direction);
int set_gpio_val(uint8_t gpio, uint8_t val);
int close_gpio(uint8_t gpio);

#endif
