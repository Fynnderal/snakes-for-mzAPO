#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "gameobjects.h"
#include "hardware_utils.h"
#include "display_utils.h"
#include "consts.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Creates and adds section to the snake.
 * 
 * @param player_snake - snake to which section must be added.
 */
void create_snake_section(snake* player_snake);

/**
 * Checks if snake must eat apple and updates state of the LED line
 * 
 * @param player_snake - snake for which possibility of eating apple should be checked.
 * @param current_apple - information about currenly existent apple.
 * @param left - determines whether left or right side of the LED must be updated. 
 */
bool eat_apple(snake* player_snake, section* current_apple, bool left);

/**
 * Initializes a snake.
 * 
 * @param player_snake - the snake that must be initialized.
 * @param parlcd_mem_base - address of the lcd display.
 * @param colod - colod of the snake.
 */
void snake_init(snake* player_snake, unsigned char* parlcd_mem_base, int color);

/**
 * Draws snake on the display
 * 
 * @param player_snake - snake that must be drawn.
 * @param parlcd_mem_base - address of the lcd diaplay.
 * @param color - color of the snake.
 */
void draw_snake(snake* player_snake, unsigned char* parlcd_mem_base, int color);

/**
 * Moves a snake.
 * 
 * @param player_snake - snake that must be moved.
 */
void move_snake(snake* player_snake);

/**
 * Checks if a snake collides with obstacles or another snake.
 * @param snake_to_check - the snake for which collisions must be checked.
 * @param snake_on_the_map - the second snake that is located on the display 
 * @param obstacles - array of obstacles on the display.
 * @param number_of_obstacles - number of the obstacles.
 * 
 * @return - return true if the snake collides, false otherwise.
 */
bool check_collisions(snake* snake_to_check, snake* snake_on_the_map, obstacle* obstacles, int number_of_obstacles);

/**
 * Checks controls and updates direction of the snake.
 * 
 * @param controlID - id of the controller for this snake. (0 for red, 1 for green, 2 for blue)
 * @param player_snale - the snake for which direction must be updated.
 * @param now - current time.
 * @param previous_update - time of the previous update.
 * @param interval_between_checks - time between updates
 */
bool check_controls(int controlID, snake* player_snake, long long now, long long previous_update, long long interval_between_checks);


#endif
