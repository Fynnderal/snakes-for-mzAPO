#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

unsigned short* fb;
volatile uint32_t otoc;
unsigned char *mem_base;

enum {
  DOWN = 0,
  RIGHT,
  UP,
  LEFT,
  NONE
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
    double speedFactor;
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

void draw_on_screen();

void clean_screen();

bool checkConrools(int cotrolID, snake* player_snake, long long now, long long previous_update);

int main(int argc, char *argv[]) {
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
  otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o); 

  section *sections = (section*) malloc(10 * sizeof(section));
  Directions * directions  = (Directions* ) malloc (16 * sizeof(Directions));

  if (!sections || !directions){
    exit(-1);
  }

  for (int i = 0; i < 15; i++){

    directions[i] = DOWN;
  }
  directions[15] = RIGHT;

  snake red_snake = {10, 100, 8, 10, 1, sections, directions};
  


  snake_init(&red_snake, parlcd_mem_base);
  struct timespec ts;


  long long now;
  long long previousUpdateTime = 0;
  long long timeBetweenUpdates = 1000;

  long long previousCheckControls = 0;

  int previousDelta = 0;
  while (true){
    clock_gettime(CLOCK_REALTIME, &ts);

    now = (long long) ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;
    
    if (checkConrools(0, &red_snake, now, previousCheckControls)) 
      previousCheckControls = now;

    if (now - previousUpdateTime >= timeBetweenUpdates){
      clean_screen();
      move_snake(&red_snake);

      draw_snake(&red_snake, parlcd_mem_base);
      previousUpdateTime = now;
    }

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

  draw_on_screen(parlcd_mem_base);
}

void draw_on_screen(unsigned char* parlcd_mem_base){
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

  draw_on_screen(parlcd_mem_base);
}

void move_snake(snake* player_snake){
  for (int i = 0; i < player_snake->sectionsNumber; i++) {

    switch(player_snake->directions[i]) {
      case RIGHT:
        player_snake->sections[i].currentX += player_snake->speedFactor * player_snake->sectionSize;
        break; 

      case LEFT:
        player_snake->sections[i].currentX -= player_snake->speedFactor * player_snake->sectionSize;
        break;

      case DOWN:
        player_snake->sections[i].currentY += player_snake->speedFactor * player_snake->sectionSize;
        break;

      case UP:
        player_snake->sections[i].currentY -= player_snake->speedFactor * player_snake->sectionSize;
        break;
      
      case NONE:
        printf("No directions for %d\n", i);
        break;
    }
  }

  for (int i = 15; i > 0; i--){
      player_snake->directions[i] = player_snake->directions[i - 1];
  }

}

bool checkConrools(int cotrolID, snake* player_snake, long long now, long long previous_update){
  volatile uint32_t novy_otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
  int delta = (int) ((novy_otoc >> 16)&0xff) - (int)((otoc >> 16)&0xff);
  if (delta > 128){
    delta -= 256;
  }

  if (delta < -128){
    delta += 256;
  }
  
  otoc = novy_otoc;

  if (now - previous_update >= 400){
      if (delta == 0)
          return false;
          
      if (delta > 0){
        if (player_snake->directions[0] == LEFT)
          player_snake->directions[0] = DOWN;
        else
          player_snake->directions[0]++;
      }
      if (delta < 0)
      {
        if (player_snake->directions[0] == DOWN)
          player_snake->directions[0] = LEFT;
        else
          player_snake->directions[0]--;
      }
    
    return true;
  }

  return false;
}