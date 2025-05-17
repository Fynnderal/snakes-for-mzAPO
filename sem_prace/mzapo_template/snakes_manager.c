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


unsigned char *parlcd_mem_base;

bool start = false;
bool in_main_menu = true;
bool in_options = false;
bool in_game_menu = false;
bool in_game_over_screen = false;

signed char current_option = 0;
int game_over;

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
 * Updates state of the main menu.
 * It checks what option in the main menu player chose and draws respective menu.
 */
void update_main_menu(){
  clean_screen(); 
  draw_main_menu(current_option);
  draw_screen(parlcd_mem_base);

  int delta = get_delta(1);

  if (delta > 1)
    current_option = (current_option + 1) % 3;
  if (delta < -1){
    current_option = (current_option + 2) % 3;
  }

  if (button_pressed(1)){
    switch(current_option){
      case 0:
        // starts game
        in_main_menu = false;
        start = true;
        init_game(parlcd_mem_base, &red_snake, &blue_snake, &obstacles, &number_of_obstacles, current_level);
        break;

      case 1:
        // opens menu with options
        in_main_menu = false;
        in_options = true;
        current_option = 0;
        break;

      case 2:
        // exits from the game
        clean_screen();
        draw_screen(parlcd_mem_base);
        exit(0);
        break;
    }
    sleep(1);
  }
}

/**
 * Updates state of the option menu.
 */
void update_options_menu(){
  int delta = get_delta(1);
  
  if (delta != 0)
    current_option = (current_option + 1) % 2;

  clean_screen();
  draw_options_menu(current_option);
  draw_screen(parlcd_mem_base);


  if (button_pressed(1)) {
    // returns to the main menu
    in_options = false;
    in_main_menu = true;
    current_option = 0;
    sleep(1);
  }
}

/**
 * Updates state of the in-game menu. This menu freezes game
 */
void update_ingame_menu(){
  int delta = get_delta(1);
  if (delta != 0)
    current_option = (current_option + 1) % 2;

  draw_ingame_menu(current_option);
  draw_screen(parlcd_mem_base);

  if (button_pressed(1)) {
    if (current_option == 0){
      // continues game
      in_game_menu = false;
      current_option = 0;
      start = true;
    }else if (current_option == 1){
      // exits to the main menu
      in_game_menu = false;
      in_main_menu = true;
      current_option = 0;
      reset_hardware();
    }
    sleep(1);
  }
}

/**
 * Updates the state of the game-over screen
 */
void update_game_over_screen(){
  int delta = get_delta(1);
  if (delta != 0)
    current_option = (current_option + 1) % 2;
  
  // checks what player has won
  if (game_over == 0)
    draw_game_over_screen("RED WON!", 8, current_option);
  else
    draw_game_over_screen("BLUE WON!", 9, current_option);
  
  draw_screen(parlcd_mem_base);

  if (button_pressed(1)){
    if (current_option == 0){
        // restarts game
        in_game_over_screen = false;
        start = true;
        init_game(parlcd_mem_base, &red_snake, &blue_snake, &obstacles, &number_of_obstacles, current_level);
        sleep(1);
    }
    else {
      // returns to the main menu
      in_game_over_screen = false;
      in_main_menu = true;
      reset_hardware();
      sleep(1);
    }
  }
}

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

  long long previousRedCheckControls = 0;
  long long previousBlueCheckControls = 0;

  long long invincibleInterval = 5000 / speed;
  long long intervalBetweenChecks = 400 / speed;

  srand (time(NULL));

  //update
  while (true){
    clock_gettime(CLOCK_REALTIME, &ts);

    timeBetweenUpdates = 1000 / speed;
    
    now = (long long) ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;


    if (in_main_menu) {
      update_main_menu();
    }

    if (in_options){
      update_options_menu();
    }

    if (in_game_menu){
      update_ingame_menu();
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

      if (checkConrools(0, &red_snake, now, previousRedCheckControls, intervalBetweenChecks)) 
        previousRedCheckControls = now;

      if (checkConrools(2, &blue_snake, now, previousBlueCheckControls, intervalBetweenChecks))
        previousBlueCheckControls = now;


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
      update_game_over_screen();
    }
    
  }

  return 0;
}










