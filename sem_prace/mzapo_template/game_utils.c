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

