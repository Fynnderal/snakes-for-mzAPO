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

/**
 * Checks if game should be over
 * 
 * @param first_snake - snake of the first player.
 * @param second_snake - snake of the second player.
 * 
 * @return - returns -1 if game must not be ended, 0 if the first player won, 1 otherwise
 */
int check_game_over(snake* first_snake, snake* second_snake);

/**
 * Draws all obstacles on the level.
 * 
 * @param obstacles - array of the obstacles.
 * @param number_of_obstacles - number of the obstacles.
 */
void draw_obstacles(obstacle* obstacles, int number_of_obstacles);

/**
 * Initializes game. Creates snakes and initializes them, resets hardwares and creates obstacles.
 * 
 * @param parlcd_mem_base - address of the lcd display.
 * @param first_snake - snake of the first player.
 * @param second_snake - snake of the second player.
 * @param obstacles - ref to array of the obstacles on the map. It will be used for keeping information about created obstacles.
 * @param number_of_obstacles - ref to int where infromation about number of the obstacles must be safev.
 * @param current_level - number of level to launch.
 */
void init_game(unsigned char* parlcd_mem_base, snake* first_snake, snake* second_snake, obstacle** obstacles, int* number_of_obstacles, int current_level);

/**
 * Draws game-over screen.
 * 
 * @param text - text to write on the screen.
 * @param text_length - length of the text.
 * @param current_optioin - currently selected option in the menu.
 */
void draw_game_over_screen(char* text, int text_length, int current_option);

/**
 * Draws ingame menu.
 * 
 * @param current_optioin - currently selected option in the menu.
 */
void draw_ingame_menu(int current_option);

/**
 * Draws options menu.
 * 
 * @param current_optioin - currently selected option in the menu.
 */
void draw_options_menu(int current_option);

/**
 * Draws main menu.
 * 
 * @param current_optioin - currently selected option in the menu.
 */
void draw_main_menu(int current_option);

/**
 * Checks controls and updates direction of the snake.
 * 
 * @param controlID - id of the controller for this snake. (0 for red, 1 for green, 2 for blue)
 * @param player_snale - the snake for which direction must be updated.
 * @param now - current time.
 * @param previous_update - time of the previous update.
 * @param interval_between_checks - time between updates
 */
bool checkConrools(int controlID, snake* player_snake, long long now, long long previous_update, long long interval_between_checks);

/**
 * Creates apple object
 * 
 * @param snake1 - snake of the first player.
 * @param snake2 - snake of the second player.
 * @param obstacles - array of the obstacles.
 * @param number_of_obstacles - number of the obstacles.
 * @param apple - ref to section where infromation about the apple must be saved.
 * 
 * @return - returns apple and NULL if it was not created.
 */
section* create_apple(snake* snake1, snake* snake2, obstacle* obstacles, int number_of_the_obstacles, section* apple);

/**
 * Creates and draws apple on the display.
 * @param snake1 - snake of the first player.
 * @param snake2 - snake of the second player.
 * @param obstacles - array of the obstacles.
 * @param number_of_obstacles - number of the obstacles.
 * @param apple - ref to section where infromation about the apple must be saved.
 * 
 * @return - returns apple and NULL if it was not created.
 */
section* spawn_apple(snake* snake1, snake* snake2, obstacle* obstacles, int number_of_the_obstacles, section* apple);

#endif