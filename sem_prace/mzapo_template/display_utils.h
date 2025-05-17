#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include "font_types.h"
#include "consts.h"

#include <stdlib.h>
#include "mzapo_parlcd.h"


extern unsigned short* fb;
extern font_descriptor_t* fdes;

void init_display_utils();
void draw_pixel(int x, int y, unsigned short color);
void draw_pixel_big(int x, int y, int scale, unsigned short color);
void draw_char(int x, int y, char ch, unsigned short color);
int char_width(int ch);
void draw_number(int x, int y, unsigned char number);
void draw_text(int x, int y, char* text, int amount_of_symbols, int color);
void draw_screen(unsigned char* parlcd_mem_base);
void clean_screen();
void draw_object(int x, int y, int width, int height, int color);

#endif