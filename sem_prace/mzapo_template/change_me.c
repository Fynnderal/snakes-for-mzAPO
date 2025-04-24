#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

unsigned short* fb;

enum {
  UP = 0,
  RIGHT,
  LEFT,
  DOWN,
} typedef Directions;

struct {
  int currentX;
  int currentY;
} typedef section;

struct {
    Directions direction;
    int currentX;
    int currentY;
    int sectionSize;
    int sectionsNumber;
    section* sections;
} typedef snake;




void draw_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}


int main(int argc, char *argv[]) {
  unsigned char *mem_base;
  unsigned char *parlcd_mem_base;
  uint32_t val_line=5;
  int ptr;
  unsigned int c;
  fb  = (unsigned short *)malloc(320*480*2);



  printf("Hello world\n");

  sleep(1);

  /*
   * Setup memory mapping which provides access to the peripheral
   * registers region of RGB LEDs, knobs and line of yellow LEDs.
   */
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  /* If mapping fails exit with error code */
  if (mem_base == NULL)
    exit(1);

  struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000};
  //clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);

  if (parlcd_mem_base == NULL)
    exit(1);

  parlcd_hx8357_init(parlcd_mem_base);

  section *sections = (section*) malloc(10 * sizeof(section));

  snake red_snake = {RIGHT, 10, 100, 8, 10, sections};

  for (int k = 0; k < red_snake.sectionsNumber; k++){
    red_snake.sections[k].currentX = red_snake.currentX;
    red_snake.sections[k].currentY = red_snake.currentY - k * red_snake.sectionSize;
  }


  for (int k = 0; k < red_snake.sectionsNumber; k++){
    section currentSection = red_snake.sections[k];
    for (int i = 0; i < red_snake.sectionSize; i++){
      for (int j = 0; j < red_snake.sectionSize; j++){
        draw_pixel(currentSection.currentX  + j, currentSection.currentY + i, 0xf800);
      }
    }
  }

  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (ptr = 0; ptr < 480*320 ; ptr++) {
      parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }

  while (true){
    move_snake(&red_snake);
  }


  printf("Goodbye world\n");

  return 0;
}


void move_snake(snake* player_snake){
  switch(player_snake->direction) {
    case RIGHT:
      printf("Got it\n");
      break; 
  }
} 