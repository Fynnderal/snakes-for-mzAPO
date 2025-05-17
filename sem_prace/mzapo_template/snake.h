#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "gameobjects.h"
#include "consts.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void create_snake_section(snake* player_snake);
bool eat_apple(snake* player_snake, section* current_apple, bool left);
void snake_init(snake* player_snake, unsigned char* parlcd_mem_base, int color);
void draw_snake(snake* player_snake, unsigned char* parlcd_mem_base, int color);
void move_snake(snake* player_snake);
bool check_collisions(snake* snake_to_check, snake* snake_on_the_map, obstacle* obstacles, int number_of_obstacles);



#endif
