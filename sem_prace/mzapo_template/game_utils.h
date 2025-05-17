#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "gameobjects.h"
#include "display_utils.h"
#include "snake.h"
#include "hardware_utils.h"

#include <stdbool.h>
#include <stdlib.h>

extern unsigned char speed;
extern int current_level;
extern int number_of_levels;

int check_game_over(snake* first_snake, snake* second_snake);
void draw_obstacles(obstacle* obstacles, int number_of_obstacles);
void init_game(unsigned char* parlcd_mem_base, snake* first_snake, snake* second_snake, obstacle** obstacles, int* number_of_obstacles, int current_level);
void draw_game_over_screen(char* text, int text_length, int current_option);
void draw_ingame_menu(int current_option);
void draw_options_menu(int current_option);
void draw_main_menu(int current_option);
bool checkConrools(int controlID, snake* player_snake, long long now, long long previous_update, long long intervalBetweenChecks);
section* create_apple(snake* snake1, snake* snake2, section* apple);
section* spawn_apple(snake* snake1, snake* snake2, section* apple);

#endif