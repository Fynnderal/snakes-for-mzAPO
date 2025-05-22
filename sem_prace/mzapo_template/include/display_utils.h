#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include "font_types.h"
#include "consts.h"

#include <stdlib.h>
#include "mzapo_parlcd.h"

/**frame buffer */
extern unsigned short* fb;

/**font descriptor */
extern font_descriptor_t* fdes;

/**
 * Initialize utils. Must be called before using functions from this program.
 */
void init_display_utils();

/**
 * Draws one pixel to the frame buffer.
 * 
 * @param x - x coordinate where pixel must be drawn.
 * @param y - y coordinate where pixel must be drawn.
 * @param color - color of the pixel
 */
void draw_pixel(int x, int y, unsigned short color);

/**
 * Draws one big pixel to the frame buffer.
 * 
 * @param x - x coordinate where pixel must be drawn.
 * @param y - y coordinate where pixel must be drawn.
 * @param scale - scale factor for the pixel.
 * @param color - color of the pixel.
 */
void draw_pixel_big(int x, int y, int scale, unsigned short color);

/**
 * Draws one character to the frame buffer.
 * 
 * @param x - x coordinate where the character be drawn.
 * @param y - y coordinate where the character must be drawn.
 * @param ch - the character that must be drawn.
 * @param color - color of the character
 */
void draw_char(int x, int y, char ch, unsigned short color);

/**
 * Gets width of the specified character.
 * 
 * @param ch - the character for which width should be calculated.
 * 
 * @return - width of the character
 */
int char_width(int ch);

/**
 * Draws a number to the frame buffer.
 * 
 * @param x - x coordinate where the number must be drawn.
 * @param y - y coordinate where the number must be drawn.
 * @param number - the number that must be drawn.
 */
void draw_number(int x, int y, unsigned char number);

/**
 * Draws a text to the frame buffer.
 * 
 * @param x - x coordinate where the text must be drawn.
 * @param y - y coordinate where the text must be drawn.
 * @param text - text that must be drawn.
 * @param amount_of_symbols - amount of symbols in the text.
 * @param color - color of the text.
 */
void draw_text(int x, int y, char* text, int amount_of_symbols, int color);

/**
 * Draws frame in the frame buffer on the display.
 * 
 * @param parlcd_mem_base - address of the lcd
 */
void draw_screen(unsigned char* parlcd_mem_base);

/**
 * Cleans the screen,
 */
void clean_screen();

/**
 * Draws square object on the screen.
 * 
 * @param x - x coordinate of the object.
 * @param y - y coordinate of the object.
 * @param width - width of the object.
 * @param height - height of the object.
 * @param color - color of the object.
 */
void draw_object(int x, int y, int width, int height, int color);

#endif