/*******************************************************************
  Program provides utils for controlling and updating the state of the game "Snakes" for the MZ_APO board


  (C) Copyright 2025 by Abdzhanov Aidar
      e-mail:   abdzhaid@cvut.cz
      github: https://gitlab.fel.cvut.cz/B242_B0B35APO/abdzhaid/-/tree/main/sem_prace?ref_type=heads
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#include "game_utils.h"


unsigned char speed = 3;
int current_level = 0;
int number_of_levels = 2;


int check_game_over(snake* first_snake, snake* second_snake){
  if (first_snake->sections_number >= DEFALT_SNAKE_SIZE + 17 || second_snake->current_state == DEAD)
    return 0;
  
  if (second_snake->sections_number >= DEFALT_SNAKE_SIZE + 17 || first_snake->current_state == DEAD)
    return 1;

  return -1;
}

void draw_obstacles(obstacle* obstacles, int number_of_obstacles){
  if (!obstacles || number_of_obstacles == 0)
    return;

  for (int i = 0; i < number_of_obstacles; i++){
    draw_object(obstacles[i].current_x, obstacles[i].current_y, obstacles[i].width, obstacles[i].height, 0xffef);
  }

}

void init_game(unsigned char* parlcd_mem_base, snake* first_snake, snake* second_snake, obstacle** obstacles, int* number_of_obstacles, int current_level){
  reset_hardware();
  
  section *sectionsRED = (section*) malloc(10 * sizeof(section));
  Directions * directionsRED  = (Directions* ) malloc (27 * sizeof(Directions));

  section *sectionsBLUE = (section*) malloc(10 * sizeof(section));
  Directions * directionsBLUE  = (Directions* ) malloc (27 * sizeof(Directions));

  if (!sectionsRED || !sectionsBLUE || !directionsRED || !directionsBLUE){
    exit(-1);
  }

  for (int i = 0; i < 27; i++){
    directionsRED[i] = DOWN;
    directionsBLUE[i] = DOWN;
  }

  *first_snake = (snake) {50, 100, 8, DEFALT_SNAKE_SIZE, OK, sectionsRED, directionsRED};
  *second_snake = (snake) {400, 100, 8, DEFALT_SNAKE_SIZE, OK, sectionsBLUE, directionsBLUE};

  snake_init(first_snake, parlcd_mem_base, 0xf800);
  snake_init(second_snake, parlcd_mem_base, 0x1f);

  if (current_level == 0){
    *obstacles = NULL;
    *number_of_obstacles = 0;
  }else if (current_level == 1){
    *obstacles = (obstacle*)malloc(4 * sizeof(obstacle));

    if (!(*obstacles))
      exit(-1);

    (*obstacles)[0] = (obstacle) {0, 0, 480, 20};
    (*obstacles)[1] = (obstacle) {0, 0, 20, 320};
    (*obstacles)[2] = (obstacle) {460, 0, 20, 320};
    (*obstacles)[3] = (obstacle) {0, 300, 480, 20};
    *number_of_obstacles = 4;
  }
}

void draw_game_over_screen(char* text, int text_length, int current_option){
  int bottom_y = 85 + fdes->height * FONT_SIZE * 3 + 30;
  int bottom_x = 160 + (2 * char_width('R') + char_width('E') + char_width('S') + 3 * char_width('T') + char_width('A')) * FONT_SIZE;

  for (int j = 85 + fdes->height * FONT_SIZE; j < bottom_y; j++){
    for (int i = 150; i < bottom_x; i++){
      draw_pixel(i, j, 0xf800);
    }
  }

  // draws text and buttons
  draw_text(150, 85, text, text_length, 0xffff);
  draw_text(160, 85 + fdes->height * FONT_SIZE + 10, "RESTART", 8, 0xffff);
  draw_text(160, 85 + fdes->height * FONT_SIZE * 2 + 20, "QUIT", 4, 0xffff);

  // if the option selected, its color must be changed to grey.
  switch(current_option){
    case 0:
      draw_text(160, 85 + fdes->height * FONT_SIZE + 10, "RESTART", 8, 0xbdf7);
      break;

    case 1:
        draw_text(160, 85 + fdes->height * FONT_SIZE * 2 + 20,  "QUIT", 4, 0xbdf7);
        break;
  }
}

void draw_ingame_menu(int current_option){
  int bottom_y = 85 + fdes->height * FONT_SIZE * 2 + 20;
  int bottom_x = 160 + (char_width('C') + char_width('O') + 2 * char_width('N') + char_width('T') + char_width('I') + char_width('U') + 2 * char_width('E')) * FONT_SIZE;

  // creates panel for the texts
  for (int j = 80; j < bottom_y; j++){
    for (int i = 150; i < bottom_x; i++){
      draw_pixel(i, j, 0xf800);
    }
  }

  draw_text(160, 85, "CONTINUE", 8, 0xffff);
  draw_text(160, 85 + fdes->height * FONT_SIZE + 10, "QUIT", 4, 0xffff);

    // if the option selected, its color must be changed to grey.
  switch(current_option){
    case 0:
      draw_text(160, 85, "CONTINUE", 8, 0xbdf7);
      break;

    case 1:
        draw_text(160, 85 + fdes->height * FONT_SIZE + 10, "QUIT", 4, 0xbdf7);
        break;
  }
}


void draw_options_menu(int current_option){
      int textX = 160;
     
      // changes speed with buttons
      if (current_option == 0) {
        draw_text(textX, 85, "SPEED", 6, 0xbdf7);
        int delta = get_delta(2);
        speed += (delta / 2);
      }
      else{
        draw_text(textX, 85, "SPEED", 6, 0xffff);
      }
      textX += (char_width('S') + char_width('P') + char_width('E') + char_width('E') + char_width('D') + char_width('S'))* FONT_SIZE;

      draw_number(textX, 85, speed);
      
      textX = 160;

      if (current_option == 1){
        draw_text(160, 90 + fdes->height * FONT_SIZE, "LEVEL", 5, 0xbdf7);
        int delta = get_delta(2);
        
          // changes level with buttons
        if (delta > 1)
          current_level = (current_level + 1) % number_of_levels;
        else if (delta < -1)
          current_level = (current_level + number_of_levels - 1) % number_of_levels;  
      }
      else
        draw_text(textX, 90 + fdes->height * FONT_SIZE, "LEVEL", 5, 0xffff);

      textX += (char_width('L') + char_width('E') + char_width('V') + char_width('E') + char_width('L')  + char_width('S'))* FONT_SIZE;
      draw_number(textX, 90 + fdes->height * FONT_SIZE, current_level + 1);
}


void draw_main_menu(int current_option){
  draw_text(160, 85, "START", 5, 0xffff);
  draw_text(160, 85 + fdes->height * FONT_SIZE + 10, "OPTIONS", 7, 0xffff);
  draw_text(160, 85 + 20 + fdes->height * 2 * FONT_SIZE, "EXIT", 4, 0xffff);
      // if the option selected, its color must be changed to grey.
  switch(current_option){
    case 0:
      draw_text(160, 85, "START", 5, 0xbdf7);
      break;

    case 1:
      draw_text(160, 85 + fdes->height * FONT_SIZE + 10, "OPTIONS", 7, 0xbdf7);
      break;

    case 2:
        draw_text(160, 85 + 20 + fdes->height * 2 * FONT_SIZE, "EXIT", 4, 0xbdf7);
        break;
  }

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



section* create_apple(snake* snake1, snake* snake2, obstacle* obstacles, int number_of_the_obstacles, section* apple){
  int randomX = rand() % 460 + 10;
  int randomY = rand() % 300 + 10;

  // if apple was created on the snake, it must be deleted.
  for (int i = 0; i < snake1->sections_number; i++){
    if (randomX + AppleSize >= snake1->sections[i].current_x &&
        randomX <= snake1->sections[i].current_x + snake1->section_size &&
        randomY + AppleSize >= snake1 -> sections[i].current_y &&
        randomY <= snake1 ->sections[i].current_y + snake1->section_size
        ){
          return NULL;
        }
  }
  
    // if apple was created on the snake, it must be deleted.
  for (int i = 0; i < snake2->sections_number; i++){
    if (randomX + AppleSize >= snake2->sections[i].current_x &&
        randomX <= snake2->sections[i].current_x + snake2->section_size &&
        randomY + AppleSize >= snake2 -> sections[i].current_y &&
        randomY <= snake2 ->sections[i].current_y + snake2->section_size
        ){
          return NULL;
        }
  }

      // if apple was created on the obstacle, it must be deleted.
  for (int i = 0; i < number_of_the_obstacles; i++){
    if (randomX + AppleSize >= obstacles[i].current_x &&
        randomX <= obstacles[i].current_x + obstacles[i].width &&
        randomY + AppleSize >=  obstacles[i].current_y &&
        randomY <= obstacles[i].current_y + obstacles[i].height
        ){
          return NULL;
        }
  }
  apple = (section*) malloc(sizeof(section));
  if (!apple)
    exit(-1);

  apple->current_x = randomX;
  apple->current_y = randomY;
  return apple;
}

section* spawn_apple(snake* snake1, snake* snake2, obstacle* obstacles, int number_of_the_obstacles, section* apple){
  apple = create_apple(snake1, snake2, obstacles, number_of_the_obstacles, apple);
  if (apple == NULL)
    return NULL;

  draw_object(apple->current_x, apple->current_y, AppleSize, AppleSize, 0xf800);
  return apple;
}

