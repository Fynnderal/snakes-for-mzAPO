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
    int currentX;
    int currentY;
    int sectionSize;
    int sectionsNumber;
    int speed;
    section* sections;
    Directions* directions;
} typedef snake;




void draw_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}

void draw_snake(snake* player_snake, unsigned char* parlcd_mem_base);

void move_snake(snake* player_snake);

void snake_init(snake* player_snake, unsigned char* parlcd_mem_base);

int main(int argc, char *argv[]) {
  unsigned char *mem_base;
  unsigned char *parlcd_mem_base;
  uint32_t val_line=5;
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

  //initialization of the snake
  section *sections = (section*) malloc(10 * sizeof(section));
  Directions * directions  = (Directions* ) malloc (16 * sizeof(Directions));

  if (!sections || !directions){
    exit(-1);
  }

  for (int i = 0; i < 16; i++){
    directions[i] = DOWN;
  }

  snake red_snake = {10, 100, 8, 10, 5, sections, directions};
  



  while (true){
    clean_screen()
    move_snake(&red_snake);
    for (int i = 1; i < 16; i++){
      red_snake.directions[i] = red_snake.directions[i - 1];
    }

    draw_snake(&red_snake, parlcd_mem_base);
    sleep(0.5);
  }


  printf("Goodbye world\n");

  return 0;
}


void snake_init(snake* player_snake, unsigned char* parlcd_mem_base){
    for (int k = 0; k < player_snake->sectionsNumber; k++){
    player_snake->sections[k].currentX = player_snake->currentX;
    player_snake->sections[k].currentY = player_snake->currentY - k * player_snake->sectionSize;
  }


  for (int k = 0; k < player_snake->sectionsNumber; k++){
    section currentSection = player_snake->sections[k];
    for (int i = 0; i < player_snake->sectionSize; i++){
      for (int j = 0; j < player_snake->sectionSize; j++){
        draw_pixel(currentSection.currentX  + j, currentSection.currentY + i, 0xf800);
      }
    }
  }

  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  for (int ptr = 0; ptr < 480*320 ; ptr++) {
      parlcd_write_data(parlcd_mem_base, fb[ptr]);
  }

}

void draw_on_screen(){
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

void draw_snake(snake* player_snake, unsigned char* parlcd_mem_base){
  for (int k = 0; k < player_snake->sectionsNumber; k++){
    section currentSection = player_snake->sections[k];
    for (int i = 0; i < player_snake->sectionSize; i++){
      for (int j = 0; j < player_snake->sectionSize; j++){
        draw_pixel(currentSection.currentX  + j, currentSection.currentY + i, 0xf800);
      }
    }
  }
}

void move_snake(snake* player_snake){
  for (int i = 0; i < player_snake->sectionsNumber; i++) {

    switch(player_snake->directions[i]) {
      case RIGHT:
        printf("Got it\n");
        break; 

      case DOWN:
        player_snake->sections[i].currentY += player_snake->speed;
        break;
    }
  }
} 