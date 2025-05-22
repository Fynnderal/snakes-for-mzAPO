/*******************************************************************
  Program provides utils for controlling gui elements of the game "Snakes" for the MZ_APO board


  (C) Copyright 2025 by Abdzhanov Aidar
      e-mail:   abdzhaid@cvut.cz
      github: https://gitlab.fel.cvut.cz/B242_B0B35APO/abdzhaid/-/tree/main/sem_prace?ref_type=heads
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#include "gui_utils.h"

bool start = false;
bool in_main_menu = true;
bool in_options = false;
bool in_game_menu = false;
bool in_game_over_screen = false;
int game_over;

signed char current_option = 0;

void update_main_menu(unsigned char* parlcd_mem_base, snake* red_snake, snake* blue_snake, obstacle** obstacles, int* number_of_obstacles, int current_level){
  clean_screen(); 
  draw_main_menu(current_option);
  draw_screen(parlcd_mem_base);

  int delta = get_delta(1);

  if (delta > 0)
    current_option = (current_option + 1) % 3;
    
  if (delta < 0)
    current_option = (current_option + 2) % 3;
  
  if (delta != 0){
    // solves slippage
    usleep(150000);
    get_delta(1);
  }

  if (button_pressed(1)){
    switch(current_option){
      case 0:
        // starts game
        in_main_menu = false;
        start = true;
        init_game(parlcd_mem_base, red_snake, blue_snake, obstacles, number_of_obstacles, current_level);
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

void update_options_menu(unsigned char* parlcd_mem_base){
  int delta = get_delta(1);
  
  if (delta != 0){
    current_option = (current_option + 1) % 2;
    // solves slippage
    usleep(150000);
    get_delta(1);
  }



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

void update_ingame_menu(unsigned char* parlcd_mem_base){
  
  int delta = get_delta(1);
  if (delta != 0){
    current_option = (current_option + 1) % 2;
    // solves slippage
    usleep(150000);
    get_delta(1);
  }


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

void update_game_over_screen(unsigned char* parlcd_mem_base, snake* red_snake, snake* blue_snake, obstacle** obstacles, int* number_of_obstacles, int current_level){
  
  int delta = get_delta(1);
  if (delta != 0){
    current_option = (current_option + 1) % 2;
    // solves slippage
    usleep(150000);
    get_delta(1);
  }
  
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
        init_game(parlcd_mem_base, red_snake, blue_snake, obstacles, number_of_obstacles, current_level);
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