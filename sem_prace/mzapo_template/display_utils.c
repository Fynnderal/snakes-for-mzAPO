#include "display_utils.h"

unsigned short* fb;
font_descriptor_t *fdes;

void init_display_utils(){
  fb = (unsigned short *)malloc(320*480*2);
  fdes = &font_winFreeSystem14x16;
}

void draw_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}

void draw_pixel_big(int x, int y, int scale, unsigned short color) {
  int i,j;
  for (i=0; i<scale; i++) {
    for (j=0; j<scale; j++) {
      draw_pixel(x+i, y+j, color);
    }
  }
}

void draw_char(int x, int y, char ch, unsigned short color) {
  int w = char_width(ch);
  const font_bits_t *ptr;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = &fdes->bits[(ch-fdes->firstchar)*bw*fdes->height];
    }
    int i, j;
    for (i=0; i<fdes->height; i++) {
      font_bits_t val = *ptr;
      for (j=0; j<w; j++) {
        if ((val&0x8000)!=0) {
          draw_pixel_big(x+FONT_SIZE*j, y+FONT_SIZE*i, FONT_SIZE ,color);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}

int char_width(int ch) {
  int width;
  if (!fdes->width) {
    width = fdes->maxwidth;
  } else {
    width = fdes->width[ch-fdes->firstchar];
  }
  return width;
}

void draw_number(int x, int y, unsigned char number){
  char digits[] = "0123456789";
  
  char digits_to_draw[12] = {'\0'};
  
  int current_digit;

  int idx = 0;

  do{
    current_digit = number % 10;
    number /= 10;

    digits_to_draw[11 - idx] = digits[current_digit];
    idx += 1;
  }while (number != 0);

  draw_text(x, y, &digits_to_draw[11 - idx + 1], idx , 0xff);
}

void draw_text(int x, int y, char* text, int amount_of_symbols, int color){
  for (int i = 0; i < amount_of_symbols; i++){
    draw_char(x, y, text[i], color);
    x += char_width(text[i]) * FONT_SIZE;
  }
}

void draw_screen(unsigned char* parlcd_mem_base){
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int ptr = 0; ptr < 480*320 ; ptr++) {
      parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }
}

void clean_screen(){
  for (int i = 0; i < 320; i++){
    for (int j = 0; j < 480; j++){
      draw_pixel(j, i, 0x0);
    }
  }
}

void draw_object(int x, int y, int width, int height, int color){
  for (int i = 0; i < width; i++){
    for (int j = 0; j < height; j++){
      draw_pixel(x + i, y + j, color);
    }
  }
}