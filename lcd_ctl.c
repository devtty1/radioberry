/*
 * lcd_ctl.c
 * (c) 2013 Luotao Fu <devtty0@gmail.com>
 *
 * control interface for hd44780 compatibel lcd modules connected in
 * 4-bit Mode to GPIO lines.
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lcd_ctl.h"

#define PIN(a) LCD_PIN_##a
#define get_bit(ch, pos) ((ch >> pos) & 1)
#define FILL_PATTERN " * "

static uint8_t pins[GPIO_PINS_USED] = {LCD_PIN_RS, LCD_PIN_E, LCD_PIN_DB4,
	LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7};

static int gpio_val_fds[GPIO_PINS_USED];

static uint8_t pins_disabled[GPIO_PINS_USED];

static struct extended_char ext_char_tbl = {
	/* These are German chars: ae, oe, ue, Ae, Oe, Ue, Sz */
	.in_char = {0xa4, 0xb6, 0xbc, 0x84, 0x96, 0x9c, 0x9f},
	.lcd_char = {0xE1, 0xEF, 0xF5, 0xE1, 0xEF, 0xF5, 0xE2}
};

static char inline decode_extended_char(unsigned char in_ch)
{
	int i = 0;
	unsigned char out_ch = 0xff;

	while (i < sizeof(ext_char_tbl.in_char) &&
			ext_char_tbl.in_char[i] != '\0') {
		if (in_ch == ext_char_tbl.in_char[i]) {
			out_ch = ext_char_tbl.lcd_char[i];
			break;
		} else
			i++;
	}

	return out_ch;
}

static int inline _get_index(uint8_t *array, uint8_t val)
{
	int i;

	for (i = 0; i < GPIO_PINS_USED; i++) {
		if (array[i] == val)
			return i;
	}

	return -1;
}

static void inline _check_and_set_gpio(uint8_t nr, uint8_t val, uint8_t mode)
{
	int idx;

	if (mode == BY_NR)
		idx = _get_index(pins, nr);
	else
		idx = nr;

	if (! pins_disabled[idx]) {
		pins_disabled[idx] = set_gpio_val_by_fd(gpio_val_fds[idx], val);

		if (pins_disabled[idx])
			printf("could not set GPIO %d to %d, Pin disabled\n",
					pins[idx], val);
	}
}

static void inline _lcd_clear_pins()
{
	/*TODO: supports 4bit interface only at time*/
	if (GPIO_PINS_USED < 7) {
		_check_and_set_gpio(PIN(DB4), 0, BY_NR);
		_check_and_set_gpio(PIN(DB5), 0, BY_NR);
		_check_and_set_gpio(PIN(DB6), 0, BY_NR);
		_check_and_set_gpio(PIN(DB7), 0, BY_NR);
	}
}

/* pulse Enable pin to switch to command mode */
static void inline _lcd_send_cmd()
{
	_check_and_set_gpio(PIN(E), 1, BY_NR);
	usleep(1000);
	_check_and_set_gpio(PIN(E), 0, BY_NR);
	usleep(1000);
}

static void inline scroll_buffer(char* l_buf, uint8_t *idx, char* line)
{
	uint8_t copy_size;
	uint8_t bufsize_padded = strlen(l_buf) + strlen(FILL_PATTERN);
	uint8_t remaining_chars = bufsize_padded - *idx;
	char lbuf_padded[bufsize_padded];

	memcpy(lbuf_padded, l_buf, strlen(l_buf));
	memcpy(lbuf_padded + strlen(l_buf), FILL_PATTERN, strlen(FILL_PATTERN));

	if (remaining_chars > MAX_LINE_CHAR)
		memcpy(line, lbuf_padded + *idx, MAX_LINE_CHAR);
	else {
		memcpy(line, lbuf_padded + *idx, remaining_chars);
		memcpy(line + remaining_chars, lbuf_padded, MAX_LINE_CHAR
				- remaining_chars);
	}

	(*idx)++;

	if (remaining_chars == 0)
		*idx = 0;
}

static void init_lcd_pins()
{
	int i;

	for (i = 0; i < (sizeof(pins)); i++) {
		pins_disabled[i] = init_gpio(pins[i]);
		if (pins_disabled[i])
			continue;

		/* first set gpio to high-impedance input, than to output */
		pins_disabled[i] = set_gpio_dir(pins[i], IN_GPIO);
		if (pins_disabled[i])
			continue;

		usleep(10000);

		pins_disabled[i] = set_gpio_dir(pins[i], OUT_GPIO);
		if (pins_disabled[i])
			continue;

		gpio_val_fds[i] = gpio_get_val_fd(pins[i]);
		if (gpio_val_fds[i] < 0)
			pins_disabled[i] = 1;
	}

	/* make sure that RS Pin is low */
	_check_and_set_gpio(PIN(RS), 0, BY_NR);
}

static void init_lcd_screen() {
	/* http://web.alfredstate.edu/weimandn/lcd/lcd_initialization/lcd_initialization_index.html */
	/* Step 2 ~ 5 are precondition for function set instruction (DL/N/F), DB5
	 * und 4 are set to 1 and enable pins will be pulsed four times. Timing
	 * conditions are listed in the document mentioned above */
	_lcd_clear_pins();
	_check_and_set_gpio(PIN(DB5), 1, BY_NR);
	_check_and_set_gpio(PIN(DB4), 1, BY_NR);
	_lcd_send_cmd();
	usleep(4200);

	/* Step 3 */
	_lcd_send_cmd();
	usleep(110);

	/* Step 4 */
	_lcd_send_cmd();
	usleep(110);

	/* Step 5 */
	_check_and_set_gpio(PIN(DB4), 0, BY_NR);
	_lcd_send_cmd();
	usleep(110);

	/* Step 6, the function set instruction. DL/N/F is set here. DL = 0, N =
	 * 1, F = 0 */
	_check_and_set_gpio(PIN(DB5), 1, BY_NR);
	_lcd_send_cmd();

	_lcd_clear_pins();
	_check_and_set_gpio(PIN(DB7), 1, BY_NR);
	_lcd_send_cmd();
	usleep(55);

	/* Step 7, DCB here. complete instruction in 8 bit would look like
	 * 00001DCB */
	_lcd_clear_pins();
	_lcd_send_cmd();

	_check_and_set_gpio(PIN(DB7), 1, BY_NR);
	_lcd_send_cmd();
	usleep(55);

	/* Step 8 , clear screen DB0 = 0 */
	lcd_clear_screen();

	/* Step 9, entry mode set I/D (DB5) and SH (DB4) here. */
	_lcd_clear_pins();
	_lcd_send_cmd();
	_check_and_set_gpio(PIN(DB6), 1, BY_NR);
	_check_and_set_gpio(PIN(DB5), 1, BY_NR);

	_lcd_send_cmd();
	usleep(55);

	/* Step 10 - empty*/
	/* Step 11, set DCB again and enables the display. */
	_lcd_clear_pins();
	_lcd_send_cmd();

	_check_and_set_gpio(PIN(DB7), 1, BY_NR);
	_check_and_set_gpio(PIN(DB6), 1, BY_NR);
	_lcd_send_cmd();
	usleep(55);
}

int lcd_init()
{
	int i, ret = 0;

	init_lcd_pins();
	init_lcd_screen();

	for (i = 0; i < sizeof(pins); i++)
		ret |= pins_disabled[i];

	return ret;
}

void lcd_close()
{
	int i;

	for (i = 0; i < sizeof(pins); i++) {
		close(gpio_val_fds[i]);
		close_gpio(pins[i]);
	}
}

/* use built-in command to do shift. Line1 and Line2 are always shifted together*/
void lcd_builtin_shift_screen(int direction)
{
	_lcd_clear_pins();

	_check_and_set_gpio(PIN(DB4), 1, BY_NR);

	_lcd_send_cmd();

	_check_and_set_gpio(PIN(DB7), 1, BY_NR);

	if (direction == LCD_SHIFT_R)
		_check_and_set_gpio(PIN(DB6), 1, BY_NR);

	_lcd_send_cmd();
}

void lcd_clear_screen()
{
	_lcd_clear_pins();

	_lcd_send_cmd();

	_check_and_set_gpio(PIN(DB4), 1, BY_NR);

	_lcd_send_cmd();

	usleep(1500);
}

void lcd_return_home()
{
	_lcd_clear_pins();

	_lcd_send_cmd();

	_check_and_set_gpio(PIN(DB5), 1, BY_NR);

	_lcd_send_cmd();

	usleep(1500);
}

void lcd_print_char(char ch) {
	_lcd_clear_pins();

	_check_and_set_gpio(PIN(RS), 1, BY_NR);

	_check_and_set_gpio(PIN(DB7), get_bit(ch, 7), BY_NR);
	_check_and_set_gpio(PIN(DB6), get_bit(ch, 6), BY_NR);
	_check_and_set_gpio(PIN(DB5), get_bit(ch, 5), BY_NR);
	_check_and_set_gpio(PIN(DB4), get_bit(ch, 4), BY_NR);
	_lcd_send_cmd();

	_check_and_set_gpio(PIN(DB7), get_bit(ch, 3), BY_NR);
	_check_and_set_gpio(PIN(DB6), get_bit(ch, 2), BY_NR);
	_check_and_set_gpio(PIN(DB5), get_bit(ch, 1), BY_NR);
	_check_and_set_gpio(PIN(DB4), get_bit(ch, 0), BY_NR);


	_lcd_send_cmd();

	_check_and_set_gpio(PIN(RS), 0, BY_NR);
}

void lcd_print_string(char* str)
{
	int i;
	int multi_byte_detected = 0;
	char st;

	for (i = 0; i < strlen(str); i++) {
		/* ugly hack to support utf-8 multibyte U+0x80 ~ U+0xFF */
		if ((unsigned char)str[i] == 195) {
			multi_byte_detected = 1;
			continue;
		}

		if (multi_byte_detected) {
			st = decode_extended_char(str[i]);
			multi_byte_detected = 0;
		} else
			st = str[i];

		lcd_print_char(st);
	}
}

void lcd_move_cursor_down() {
	/* RS Should be LOW, since the curso is usually moved during print, we
	 * don't need to set RS here separately */
	_lcd_clear_pins();

	/* TODO: implement moving cursor to any DDRAM address. Now we only move
	 * the cursor to 0x40, which is at the beginning of the second line for
	 * a 2x16 hd44780 lcd */
	_check_and_set_gpio(PIN(DB7), 1, BY_NR);
	_check_and_set_gpio(PIN(DB6), 1, BY_NR);

	_lcd_send_cmd();

	_lcd_clear_pins();

	_lcd_send_cmd();
}

/* print 2 lines and scroll if necessary. Two ring buffers are used to store the
 * content with the length of exact MAX_LINE_CHAR (mostly 16) characters.
 */
void lcd_update_screen(struct lcd_handle *lh)
{
	char f_line[MAX_LINE_CHAR + 1] = {0};
	char s_line[MAX_LINE_CHAR + 1] = {0};

	uint8_t fl_bufsize = strlen(lh->fline_buf);
	uint8_t sl_bufsize = strlen(lh->sline_buf);

	uint8_t fl_remaining_chars = 0, sl_remaining_chars = 0;
	uint8_t sl_line_remaining_chars = 0;

	uint8_t sl_copy_size, sline_scrolling = 0;

	/* FIXME: scrolling first line is currently commented out, since doing
	 * SW-scrolling sucks and the screen keeps flickering. Scrolling two
	 * line at once sucks even more. TODO: make it sucks less... supply
	 * problem?? */
#if 0
	uint8_t fl_copy_size;
	if (fl_bufsize) {
		fl_remaining_chars = fl_bufsize - lh->fline_idx;
		if (fl_remaining_chars > MAX_LINE_CHAR)
			fl_copy_size = MAX_LINE_CHAR;
		else
			fl_copy_size = fl_remaining_chars;

		memcpy(f_line, lh->fline_buf + lh->fline_idx, fl_copy_size);

		lh->fline_idx++;
	}
#endif
	if (fl_bufsize)
		memcpy(f_line, lh->fline_buf, MAX_LINE_CHAR);

	if (sl_bufsize > MAX_LINE_CHAR) {
		sline_scrolling = 1;
		scroll_buffer(lh->sline_buf, &(lh->sline_idx), s_line);
	} else
		memcpy(s_line, lh->sline_buf, sl_bufsize);


	if (fl_remaining_chars == 0)
		lh->fline_idx = 0;

	if (lh->fline_update) {
		lcd_clear_screen();
		lcd_print_string(f_line);
		lh->fline_update = 0;
	}

#if 0
	if (fl_bufsize) {
		lcd_return_home();
		lcd_print_string(f_line);
	}
#endif

	if (lh->sline_update || sline_scrolling) {
		lh->sline_update = 0;
		lcd_move_cursor_down();
		lcd_print_string(s_line);
	}
}
