/*******************************************************************
  Program provides utils for controlling snake to the game "Snakes" for the MZ_APO board


  (C) Copyright 2025 by Abdzhanov Aidar
      e-mail:   abdzhaid@cvut.cz
      github: https://gitlab.fel.cvut.cz/B242_B0B35APO/abdzhaid/-/tree/main/sem_prace?ref_type=heads
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/


#include "snake.h"


void create_snake_section(snake* player_snake){
        player_snake->sections_number++;
        player_snake->sections = (section*)realloc(player_snake->sections, player_snake->sections_number * sizeof(section));
        if (!player_snake->sections){
          exit(-1);
        }

        //saves information about previously last section.
        section lastSection = player_snake->sections[player_snake->sections_number - 2];
        Directions lastSectionDirection = player_snake->directions[player_snake->sections_number - 2];
        
        
        player_snake->directions[player_snake->sections_number - 1] = lastSectionDirection;
        
        // Sets coordinates of the added section
        switch(lastSectionDirection){
          case UP:
            player_snake->sections[player_snake->sections_number - 1].current_x = lastSection.current_x;
            player_snake->sections[player_snake->sections_number - 1].current_y = lastSection.current_y + player_snake->section_size;
            player_snake->sections[player_snake->sections_number - 1].current_y %= 320;
            break;

          case DOWN:
            player_snake->sections[player_snake->sections_number - 1].current_x = lastSection.current_x;
            player_snake->sections[player_snake->sections_number - 1].current_y = lastSection.current_y - player_snake->section_size;
            player_snake->sections[player_snake->sections_number - 1].current_y = (player_snake->sections[player_snake->sections_number - 1].current_y + 320) % 320;
            break;

          case RIGHT:
            player_snake->sections[player_snake->sections_number - 1].current_x = lastSection.current_x - player_snake->section_size;
            player_snake->sections[player_snake->sections_number - 1].current_x = (player_snake->sections[player_snake->sections_number - 1].current_x + 480) % 480;
            player_snake->sections[player_snake->sections_number - 1].current_y = lastSection.current_y;
            break;

          case LEFT:
            player_snake->sections[player_snake->sections_number - 1].current_x = lastSection.current_x + player_snake->section_size;
            player_snake->sections[player_snake->sections_number - 1].current_x %= 480;

            player_snake->sections[player_snake->sections_number - 1].current_y = lastSection.current_y;
            break;
          case NONE:
            fprintf(stderr, "No direction\n");
        }
}


bool eat_apple(snake* player_snake, section* current_apple, bool left){
   section head = player_snake->sections[0];

    if (head.current_x + player_snake->section_size > current_apple->current_x &&
        head.current_x < current_apple->current_x + AppleSize &&
        head.current_y + player_snake->section_size > current_apple->current_y &&
        head.current_y < current_apple->current_y + AppleSize
      ){
        create_snake_section(player_snake);
        update_LED_line(player_snake, left);
        return true;
      }

    return false;
}


void snake_init(snake* player_snake, unsigned char* parlcd_mem_base, int color){
  for (int k = 0; k < player_snake->sections_number; k++){
    player_snake->sections[k].current_x = player_snake->current_x;
    player_snake->sections[k].current_y = player_snake->current_y - k * player_snake->section_size;
  }


  for (int k = 0; k < player_snake->sections_number; k++){
    section currentSection = player_snake->sections[k];
    for (int i = 0; i < player_snake->section_size; i++){
      for (int j = 0; j < player_snake->section_size; j++){
        draw_pixel(currentSection.current_x  + j, currentSection.current_y + i, color);
      }
    }
  }

}

void draw_snake(snake* player_snake, unsigned char* parlcd_mem_base, int color){
  for (int k = 0; k < player_snake->sections_number; k++){
    section currentSection = player_snake->sections[k];
    draw_object(currentSection.current_x, currentSection.current_y, player_snake->section_size, player_snake->section_size, color);
  }
}

void move_snake(snake* player_snake){
  for (int i = 0; i < player_snake->sections_number; i++) {
      switch(player_snake->directions[i]) {

        case RIGHT:
          player_snake->sections[i].current_x += player_snake->section_size;
          break; 

        case LEFT:
          player_snake->sections[i].current_x -= player_snake->section_size;
          break;

        case DOWN:
          player_snake->sections[i].current_y += player_snake->section_size;
          break;

        case UP:
          player_snake->sections[i].current_y -= player_snake->section_size;
          break;
        
        case NONE:
          printf("No directions for %d\n", i);
          break;
      }
      
      player_snake->sections[i].current_x = (player_snake->sections[i].current_x + 480) % 480;
      player_snake->sections[i].current_y = (player_snake->sections[i].current_y + 320) % 320;
  }

  for (int i = 26; i > 0; i--){
      player_snake->directions[i] = player_snake->directions[i - 1];
  }

}

bool check_collisions(snake* snake_to_check, snake* snake_on_the_map, obstacle* obstacles, int number_of_obstacles){
  section head = snake_to_check->sections[0];

  for (int i = 0; i < snake_on_the_map->sections_number; ++i){
    if (head.current_x + snake_to_check->section_size > snake_on_the_map->sections[i].current_x &&
        head.current_x < snake_on_the_map->sections[i].current_x + snake_on_the_map->section_size &&
        head.current_y + snake_to_check->section_size > snake_on_the_map->sections[i].current_y &&
        head.current_y < snake_on_the_map->sections[i].current_y + snake_on_the_map->section_size
      ){
        return true;
      }
  }

  if (obstacles && number_of_obstacles > 0){
    for (int i = 0; i < number_of_obstacles; i++){
        if (head.current_x + snake_to_check->section_size > obstacles[i].current_x &&
          head.current_x < obstacles[i].current_x + obstacles[i].width &&
          head.current_y + snake_to_check->section_size > obstacles[i].current_y &&
          head.current_y < obstacles[i].current_y + obstacles[i].height
        ){
          return true;
        }
    }
  }

  return false;
}

bool check_controls(int controlID, snake* player_snake, long long now, long long previous_update, long long intervalBetweenChecks){

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

