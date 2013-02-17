/*
 * lcd_ctl.h
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

#ifndef SET_LCD_CTL_H
#define SET_LCD_CTL_H

#include "config_ctl.h"
#include "gpio_base.h"

#define LCD_SHIFT_R 1
#define LCD_SHIFT_L 0

#define BY_IDX 1
#define BY_NR 0

struct lcd_handle {
	char fline_buf[MAX_LINE_BUF + 1];
	char sline_buf[MAX_LINE_BUF + 1];
	uint8_t fline_idx;
	uint8_t sline_idx;
	uint8_t fline_update;
	uint8_t sline_update;
	uint8_t cline_scr;
};

struct extended_char {
	unsigned char in_char[MAX_EXT_CHARS];
	unsigned char lcd_char[MAX_EXT_CHARS];
};

#ifdef __cplusplus
 extern "C" {
#endif

void lcd_builtin_shift_screen(int direction);
void lcd_clear_screen();
void lcd_return_home();
void lcd_print_char(char ch);
void lcd_print_string(char* str);
void lcd_move_cursor_down();
void lcd_update_screen(struct lcd_handle *lh);

int lcd_init();
void lcd_close();

#ifdef __cplusplus
 }
#endif

#endif
