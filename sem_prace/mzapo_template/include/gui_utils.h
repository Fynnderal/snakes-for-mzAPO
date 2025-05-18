#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include "gameobjects.h"
#include "display_utils.h" 
#include "game_utils.h"

#include <stdbool.h>

/**Determines whther the game is started or not */
extern bool start;
/**Determines whther the player in the main menu or not */
extern bool in_main_menu;
/**Determines whther the player in the options menu or not */
extern bool in_options;
/**Determines whther the player in the in-game menu or not */
extern bool in_game_menu;
/**Determines whther the player in the game over screen or not */
extern bool in_game_over_screen;
/**Keeps information about the winner*/
extern int game_over;
/** Currently selected option in the menu */
extern signed char current_option;

/**
 * Updates state of the main menu.
 * It checks what option in the main menu player chose and draws respective menu.
 * 
 * @param parlcd_mem_base - address of the lcd display.
 * @param red_snake - snake of the first player.
 * @param blue_snake - snake of the second player.
 * @param obstacles - ref to the array of the obstacles 
 * @param number_of_obstacles - ref to the number of the obstacles
 * @param current_level - number of the current level.
 */
void update_main_menu(unsigned char* parlcd_mem_base, snake* red_snake, snake* blue_snake, obstacle** obstacles, int* number_of_obstacles, int current_level);

/**
 * Updates state of the option menu.
 * @param parlcd_mem_base - address of the lcd display.
 */
void update_options_menu(unsigned char* parlcd_mem_base);


/**
 * Updates state of the in-game menu. This menu freezes game
 * @param parlcd_mem_base - address of the lcd display.
 */
void update_ingame_menu(unsigned char* parlcd_mem_base);
 
/**
 * Updates the state of the game-over screen
 * @param parlc_mem_base - address of the lcd display.
 */
void update_game_over_screen(unsigned char* parlcd_mem_base, snake* red_snake, snake* blue_snake, obstacle** obstacles, int* number_of_obstacles, int current_level);

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

#endif