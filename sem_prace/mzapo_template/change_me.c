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

#define AppleSize 6
#define DEFALT_SNAKE_SIZE 10

enum {
  DOWN = 0,
  RIGHT,
  UP,
  LEFT,
  NONE
} typedef Directions;

enum {
  OK,
  INJURED,
  WOUNDED
} typedef States;

struct {
  int currentX;
  int currentY;
} typedef section;

struct {
    int currentX;
    int currentY;
    int sectionSize;
    int sectionsNumber;
    States currentState;
    section* sections;
    Directions* directions;
} typedef snake;



void draw_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}

void draw_snake(snake* player_snake, unsigned char* parlcd_mem_base, int color);

void move_snake(snake* player_snake);

void snake_init(snake* player_snake, unsigned char* parlcd_mem_base, int color);

void drawScreen(unsigned char* parlcd_mem_base);

void clean_screen();

bool checkConrools(int cotrolID, snake* player_snake, long long now, long long previous_update, long long intervalBetweenChecks);

bool check_collisions(snake* snake_to_check, snake* snake_on_the_map); 

void updateLED(int led_id, States state);

section* spawn_apple(snake* snake1, snake* snake2, section* apple);

section* create_apple(snake* snake1, snake* snake2, section* apple);

void createSnakeSection(snake* playerSnake);

bool eatApple(snake* playerSnake, section* currentApple, bool left);

void drawObject(int X, int Y, int Width, int Height, int color);

void updateLEDLine(snake* snakeToUpdate, bool left);

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

  section *sectionsRED = (section*) malloc(10 * sizeof(section));
  Directions * directionsRED  = (Directions* ) malloc (16 * sizeof(Directions));

  section *sectionsBLUE = (section*) malloc(10 * sizeof(section));
  Directions * directionsBLUE  = (Directions* ) malloc (16 * sizeof(Directions));

  if (!sectionsRED || !sectionsBLUE || !directionsRED || !directionsBLUE){
    exit(-1);
  }

  for (int i = 0; i < 16; i++){

    directionsRED[i] = DOWN;
  }

  for (int i = 0; i < 16; i++){

    directionsBLUE[i] = DOWN;
  }

  snake red_snake = {10, 100, 8, DEFALT_SNAKE_SIZE, OK, sectionsRED, directionsRED};
  snake blue_snake = {400, 100, 8, DEFALT_SNAKE_SIZE, OK, sectionsBLUE, directionsBLUE};
  


  snake_init(&red_snake, parlcd_mem_base, 0xf800);
  snake_init(&blue_snake, parlcd_mem_base, 0x1f);

  struct timespec ts;


  int speed = 3;
  long long now;
  long long previousUpdateTime = 0;
  long long timeBetweenUpdates = 1000 / speed;

  long long previousRedCheckControls = 0;
  long long previousBlueCheckControls = 0;

  long long invincibleInterval = 5000 / speed;

  long long intervalBetweenChecks = 400 / speed;

  long long redGotDamage = 0;
  long long blueGotDamage = 0;
  
  bool isRedInvincible = false;
  bool isBlueInvincible = false;

  section* currentApple = NULL;


  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
  srand (time(NULL));
  while (true){
    clock_gettime(CLOCK_REALTIME, &ts);

    now = (long long) ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;
    
    updateLED(0, red_snake.currentState);
    updateLED(1, blue_snake.currentState);

    if (now - redGotDamage >= invincibleInterval){
      isRedInvincible = false;
    }

    if (now - blueGotDamage >= invincibleInterval){
      isBlueInvincible = false;
    }

    if (checkConrools(0, &red_snake, now, previousRedCheckControls, intervalBetweenChecks)) 
      previousRedCheckControls = now;

    if (checkConrools(2, &blue_snake, now, previousBlueCheckControls, intervalBetweenChecks))
      previousBlueCheckControls = now;

    if (now - previousUpdateTime >= timeBetweenUpdates){
      clean_screen();
      if (!currentApple){
        currentApple = spawn_apple(&red_snake, &blue_snake, currentApple); 
      } else {
        drawObject(currentApple->currentX, currentApple->currentY, AppleSize, AppleSize, 0xf800);
      }
      move_snake(&red_snake);
      move_snake(&blue_snake);

      if (isRedInvincible)
        draw_snake(&red_snake, parlcd_mem_base, 0x807f7c);
      else
        draw_snake(&red_snake, parlcd_mem_base, 0xf800);

      if (isBlueInvincible)
        draw_snake(&blue_snake, parlcd_mem_base, 0x807f7c);
      else
        draw_snake(&blue_snake, parlcd_mem_base, 0x1f);
      previousUpdateTime = now;
    
      if (!isBlueInvincible && !isRedInvincible){
        if (check_collisions(&red_snake, &blue_snake)){
          printf("red got\n");
          redGotDamage = now;
          isRedInvincible = true;
          red_snake.currentState++;
        }
        if (check_collisions(&blue_snake, &red_snake)){
          printf("blue got\n");
          blueGotDamage = now;
          isBlueInvincible = true; 
          blue_snake.currentState++;
        }
      }

      if (currentApple != NULL && eatApple(&red_snake, currentApple, true))
        currentApple = NULL;
      
      if (currentApple != NULL && eatApple(&blue_snake, currentApple, false))
        currentApple = NULL;
      
      drawScreen(parlcd_mem_base);
    }
  }
  printf("Goodbye world\n");
  
  return 0;
}

void createSnakeSection(snake* playerSnake){
        playerSnake->sectionsNumber++;
        playerSnake->sections = (section*)realloc(playerSnake->sections, playerSnake->sectionsNumber * sizeof(section));
        if (!playerSnake->sections){
          exit(-1);
        }
        section lastSection = playerSnake->sections[playerSnake->sectionsNumber - 2];
        Directions lastSectionDirection = playerSnake->directions[playerSnake->sectionsNumber - 2];
        playerSnake->directions[playerSnake->sectionsNumber - 1] = lastSectionDirection;
        switch(lastSectionDirection){
          case UP:
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentX = lastSection.currentX;
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentY = lastSection.currentY + playerSnake->sectionSize;
            break;

          case DOWN:
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentX = lastSection.currentX;
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentY = lastSection.currentY - playerSnake->sectionSize;
            break;

          case RIGHT:
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentX = lastSection.currentX - playerSnake->sectionSize;
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentY = lastSection.currentY;
            break;

          case LEFT:
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentX = lastSection.currentX + playerSnake->sectionSize;
            playerSnake->sections[playerSnake->sectionsNumber - 1].currentY = lastSection.currentY;
            break;
          case NONE:
            fprintf(stderr, "No direction\n");
        }
}

bool eatApple(snake* playerSnake, section* currentApple, bool left){
   section head = playerSnake->sections[0];

    if (head.currentX + playerSnake->sectionSize > currentApple->currentX &&
        head.currentX < currentApple->currentX + AppleSize &&
        head.currentY + playerSnake->sectionSize > currentApple->currentY &&
        head.currentY < currentApple->currentY + AppleSize
      ){
        createSnakeSection(playerSnake);
        updateLEDLine(playerSnake, left);
        return true;
      }

    return false;
}

void updateLEDLine(snake* snakeToUpdate, bool left){
  volatile uint32_t previousValue = *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o);

  if (!left){
  int temp = 1 << (snakeToUpdate->sectionsNumber - DEFALT_SNAKE_SIZE - 1);
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = previousValue | temp; 
  }else{
    int temp = 1 << (31 - snakeToUpdate->sectionsNumber + DEFALT_SNAKE_SIZE + 1);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = previousValue | temp;
  }
}

void updateLED(int led_id, States state){
  int color = 0;

  switch (state) {
    case OK:
      color = 0x00ff00;
      break;
    case INJURED:
      color = 0x0000ff;
      break;
    case WOUNDED:
      color = 0xff0000;
      break;
    }
  if (led_id == 0)
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = color;
  else if(led_id == 1)
      *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = color;

}
void snake_init(snake* player_snake, unsigned char* parlcd_mem_base, int color){
  for (int k = 0; k < player_snake->sectionsNumber; k++){
    player_snake->sections[k].currentX = player_snake->currentX;
    player_snake->sections[k].currentY = player_snake->currentY - k * player_snake->sectionSize;
  }


  for (int k = 0; k < player_snake->sectionsNumber; k++){
    section currentSection = player_snake->sections[k];
    for (int i = 0; i < player_snake->sectionSize; i++){
      for (int j = 0; j < player_snake->sectionSize; j++){
        draw_pixel(currentSection.currentX  + j, currentSection.currentY + i, color);
      }
    }
  }

}

void drawScreen(unsigned char* parlcd_mem_base){
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

void draw_snake(snake* player_snake, unsigned char* parlcd_mem_base, int color){
  for (int k = 0; k < player_snake->sectionsNumber; k++){
    section currentSection = player_snake->sections[k];
    drawObject(currentSection.currentX, currentSection.currentY, player_snake->sectionSize, player_snake->sectionSize, color);
  }
}

void move_snake(snake* player_snake){
  for (int i = 0; i < player_snake->sectionsNumber; i++) {
      switch(player_snake->directions[i]) {

        case RIGHT:
          player_snake->sections[i].currentX += player_snake->sectionSize;
          break; 

        case LEFT:
          player_snake->sections[i].currentX -= player_snake->sectionSize;
          break;

        case DOWN:
          player_snake->sections[i].currentY += player_snake->sectionSize;
          break;

        case UP:
          player_snake->sections[i].currentY -= player_snake->sectionSize;
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

bool checkConrools(int controlID, snake* player_snake, long long now, long long previous_update, long long intervalBetweenChecks){
  int delta = 0;
  volatile uint32_t novy_otoc = otoc;

  if (controlID == 0){
    novy_otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    delta = (int) ((novy_otoc >> 16)&0xff) - (int)((otoc >> 16)&0xff);
  }
  else if (controlID == 2){
    novy_otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    delta = (int) (novy_otoc &0xff) - (int)(otoc &0xff);
  }


  if (delta > 128){
    delta -= 256;
  }

  if (delta < -128){
    delta += 256;
  }
  
  otoc = novy_otoc;

  if (now - previous_update >= intervalBetweenChecks){
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

bool check_collisions(snake* snake_to_check, snake* snake_on_the_map){
  section head = snake_to_check->sections[0];

  for (int i = 0; i < snake_on_the_map->sectionsNumber; ++i){
    if (head.currentX + snake_to_check->sectionSize > snake_on_the_map->sections[i].currentX &&
        head.currentX < snake_on_the_map->sections[i].currentX + snake_on_the_map->sectionSize &&
        head.currentY + snake_to_check->sectionSize > snake_on_the_map->sections[i].currentY &&
        head.currentY < snake_on_the_map->sections[i].currentY + snake_on_the_map->sectionSize
      ){
        return true;
      }
  }
    return false;
}

section* create_apple(snake* snake1, snake* snake2, section* apple){
  int randomX = rand() % 480;
  int randomY = rand() % 320;
  for (int i = 0; i < snake1->sectionsNumber; i++){
    if (randomX + AppleSize >= snake1->sections[i].currentX &&
        randomX <= snake1->sections[i].currentX + snake1->sectionSize &&
        randomY + AppleSize >= snake1 -> sections[i].currentY &&
        randomY <= snake1 ->sections[i].currentY + snake1->sectionSize
        ){
          return NULL;
        }
  }
  
  for (int i = 0; i < snake2->sectionsNumber; i++){
    if (randomX + AppleSize >= snake2->sections[i].currentX &&
        randomX <= snake2->sections[i].currentX + snake2->sectionSize &&
        randomY + AppleSize >= snake2 -> sections[i].currentY &&
        randomY <= snake2 ->sections[i].currentY + snake2->sectionSize
        ){
          return NULL;
        }
  }
  apple = (section*) malloc(sizeof(section));
  if (!apple)
    exit(-1);

  apple->currentX = randomX;
  apple->currentY = randomY;
  return apple;
}

section* spawn_apple(snake* snake1, snake* snake2, section* apple){
  apple = create_apple(snake1, snake2, apple);
  if (apple == NULL)
    return NULL;

  drawObject(apple->currentX, apple->currentY, AppleSize, AppleSize, 0xf800);
  return apple;
}

void drawObject(int X, int Y, int Width, int Height, int color){
  for (int i = 0; i < Width; i++){
    for (int j = 0; j < Height; j++){
      draw_pixel(X + i, Y + j, color);
    }
  }
}