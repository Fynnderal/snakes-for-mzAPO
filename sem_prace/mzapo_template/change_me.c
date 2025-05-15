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
#include "font_types.h"

unsigned short* fb;
volatile uint32_t otoc;
unsigned char *mem_base;
unsigned char speed = 3;
font_descriptor_t *fdes;

#define AppleSize 6
#define DEFALT_SNAKE_SIZE 10
#define FONT_SIZE 3

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

int get_delta(int otoc_id);

int char_width(int ch) {
  int width;
  if (!fdes->width) {
    width = fdes->maxwidth;
  } else {
    width = fdes->width[ch-fdes->firstchar];
  }
  return width;
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

void drawText(int x, int y, char* text, int amount_of_symbols, int color);

void draw_main_menu(int current_option);

bool button_pressed(int otoc_id);

int main(int argc, char *argv[]) {
  unsigned char *parlcd_mem_base;
  uint32_t val_line=5;
  unsigned int c;
  fb  = (unsigned short *)malloc(320*480*2);

  fdes = &font_winFreeSystem14x16;

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

  bool start = false;
  bool in_main_menu = true;
  bool in_options = false;
  signed char current_option = 0;

  //update
  while (true){
    clock_gettime(CLOCK_REALTIME, &ts);

    now = (long long) ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;



    if (in_main_menu) {
        clean_screen(); 
        draw_main_menu(current_option);
        drawScreen(parlcd_mem_base);

        int delta = get_delta(1);
        if (delta > 1)
          current_option = (current_option + 1) % 3;
        if (delta < -1){
          current_option = (current_option - 1) % 3;
          if (current_option < 0)
            current_option = 2;
        }

        if (button_pressed(1)){
          switch(current_option){
            case 0:
              in_main_menu = false;
              start = true;
              break;

            case 1:
              in_main_menu = false;
              in_options = true;
              break;

            case 2:
              clean_screen();
              drawScreen(parlcd_mem_base);
              exit(0);
              break;
          }
          sleep(1);
        }
    }

    if (in_options){
      char digits[] = "0123456789";
      clean_screen(); 
      int currentX = 160;
      drawText(currentX, 85, "SPEED", 6, 0xffff);

      currentX += (char_width('S') + char_width('P') + char_width('E') + char_width('E') + char_width('D') + char_width('S'))* FONT_SIZE;
      

      int delta = get_delta(1);
      speed += delta;

      int t = 100;
      int temp = speed; 
      int current_digit;

      for (int i = 0; i < 3; i++){
        current_digit = temp / t;
        temp %= t;
        t /= 10;
        if (current_digit == 0)
          continue;
        
        drawText(currentX, 85, &digits[current_digit], 1, 0xff);
        currentX += char_width(digits[current_digit]) * FONT_SIZE;
      }

      drawScreen(parlcd_mem_base);
      if (button_pressed(1)) {
        in_options = false;
        in_main_menu = true;
        sleep(1);
      }
            
    }

    if (start){
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
        if (!currentApple) {
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
  }
  printf("Goodbye world\n");
  
  return 0;
}


bool button_pressed(int otoc_id){  
  volatile uint32_t novy_otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);

  bool pressed = false;

  if (otoc_id == 0){
    pressed = (int) ((novy_otoc >> 26)&0x01) == 1;
  }
  else if (otoc_id == 1){
    pressed = (int) ((novy_otoc >> 25)&0x01)  == 1;
  }
  else{
    pressed = (int) ((novy_otoc >> 24) &0x01) == 1;
  }

  otoc = novy_otoc;
  return pressed;
}

void draw_main_menu(int current_option){
  drawText(160, 85, "START", 5, 0xffff);
  drawText(160, 85 + fdes->height * FONT_SIZE + 10, "OPTIONS", 7, 0xffff);
  drawText(160, 85 + 20 + fdes->height * 2 * FONT_SIZE, "EXIT", 4, 0xffff);
  switch(current_option){
    case 0:
      drawText(160, 85, "START", 5, 0xbdf7);
      break;

    case 1:
      drawText(160, 85 + fdes->height * FONT_SIZE + 10, "OPTIONS", 7, 0xbdf7);
      break;

    case 2:
        drawText(160, 85 + 20 + fdes->height * 2 * FONT_SIZE, "EXIT", 4, 0xbdf7);
        break;
  }

}

void drawText(int x, int y, char* text, int amount_of_symbols, int color){
  for (int i = 0; i < amount_of_symbols; i++){
    draw_char(x, y, text[i], color);
    x += char_width(text[i]) * FONT_SIZE;
  }
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

int get_delta(otoc_id){
  int delta = 0;
  
  volatile uint32_t novy_otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);

  if (otoc_id == 0){
    delta = (int) ((novy_otoc >> 16)&0xff) - (int)((otoc >> 16)&0xff);
  }
  else if (otoc_id == 1){
    delta = (int) ((novy_otoc >> 8)&0xff) - (int)((otoc >> 8)&0xff);
  }
  else{
    delta = (int) (novy_otoc &0xff) - (int)(otoc &0xff);
  }

  if (delta > 128){
    delta -= 256;
  }

  if (delta < -128){
    delta += 256;
  }


  otoc = novy_otoc;

  return delta;
}

bool checkConrools(int controlID, snake* player_snake, long long now, long long previous_update, long long intervalBetweenChecks){

  int delta = get_delta(controlID);

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