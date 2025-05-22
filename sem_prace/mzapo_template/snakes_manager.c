/*******************************************************************
  Program that implements simple game "Snakes" for the MZ_APO board


  (C) Copyright 2025 by Abdzhanov Aidar
      e-mail:   abdzhaid@cvut.cz
      github: https://gitlab.fel.cvut.cz/B242_B0B35APO/abdzhaid/-/tree/main/sem_prace?ref_type=heads
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

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
#include "gameobjects.h"
#include "game_utils.h"
#include "hardware_utils.h"
#include "display_utils.h"
#include "gui_utils.h"


unsigned char *parlcd_mem_base;



snake red_snake = {0};
snake blue_snake = {0};

obstacle* obstacles = NULL; 
int number_of_obstacles = 0;

long long previousUpdateTime = 0;
long long now;

long long redGotDamage = 0;
long long blueGotDamage = 0;
bool isRedInvincible = false;
bool isBlueInvincible = false;

section* currentApple = NULL;



/**
 * Draws the current state of the game.
 */
void draw_game_screen(){
  clean_screen();

  // creates and draws new apple if it currently doesn't exist
  if (!currentApple) {
    currentApple = spawn_apple(&red_snake, &blue_snake, obstacles, number_of_obstacles, currentApple); 
  } else {
    draw_object(currentApple->current_x, currentApple->current_y, AppleSize, AppleSize, 0xf800);
  }

  draw_obstacles(obstacles, number_of_obstacles);

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

  // checks if snakes should receive damage
  if (!isBlueInvincible && !isRedInvincible){
    if (check_collisions(&red_snake, &blue_snake, obstacles, number_of_obstacles)){
      redGotDamage = now;
      isRedInvincible = true;
      red_snake.current_state++;
    }
    if (check_collisions(&blue_snake, &red_snake, obstacles, number_of_obstacles)){
      blueGotDamage = now;
      isBlueInvincible = true; 
      blue_snake.current_state++;
    }
  }

  // checks if any snake must eat apple and deletes it.
  if (currentApple != NULL && eat_apple(&red_snake, currentApple, true))
    currentApple = NULL;
  
  if (currentApple != NULL && eat_apple(&blue_snake, currentApple, false))
    currentApple = NULL;
  
  draw_screen(parlcd_mem_base);
}

int main(int argc, char *argv[]) {

  sleep(1);

  
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);

  if (parlcd_mem_base == NULL)
    exit(1);

  parlcd_hx8357_init(parlcd_mem_base);

  init_hardware_utils();
  init_display_utils();

  struct timespec ts;

  long long timeBetweenUpdates;

  long long previous_red_check_controls = 0;
  long long previous_blue_check_controls = 0;

  long long invincibleInterval = 5000 / speed;
  long long intervalBetweenChecks = 400 / speed;

  srand (time(NULL));

  //update
  while (true){
    clock_gettime(CLOCK_REALTIME, &ts);

    timeBetweenUpdates = 1000 / speed;
    
    now = (long long) ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;


    if (in_main_menu) {
      update_main_menu(parlcd_mem_base, &red_snake, &blue_snake, &obstacles, &number_of_obstacles, current_level);
    }

    if (in_options){
      update_options_menu(parlcd_mem_base);
    }

    if (in_game_menu){
      update_ingame_menu(parlcd_mem_base);
    }

    if (start){

      update_LED(0, red_snake.current_state);
      update_LED(1, blue_snake.current_state);

      if (now - redGotDamage >= invincibleInterval){
        isRedInvincible = false;
      }

      if (now - blueGotDamage >= invincibleInterval){
        isBlueInvincible = false;
      }

      if (check_controls(0, &red_snake, now, previous_red_check_controls, intervalBetweenChecks)) 
        previous_red_check_controls = now;

      if (check_controls(2, &blue_snake, now, previous_blue_check_controls, intervalBetweenChecks))
        previous_blue_check_controls = now;


      // updates screen with required speed
      if (now - previousUpdateTime >= timeBetweenUpdates){
        draw_game_screen();
      }

      // opens in-game menu
      if (button_pressed(1)){
        in_game_menu = true;
        start = false; 
        sleep(1);
      }
      
      game_over = check_game_over(&red_snake, &blue_snake);
      
      // if game was over
      if (game_over != -1){ 
        start = false;
        in_game_over_screen = true;
      }
    }

    if (in_game_over_screen){
      update_game_over_screen(parlcd_mem_base, &red_snake, &blue_snake, &obstacles, &number_of_obstacles, current_level);
    }
    
  }

  return 0;
}










