#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H


enum {
  DOWN = 0,
  RIGHT,
  UP,
  LEFT,
  NONE
} typedef Directions;

enum {
  OK,
  INJURED,
  WOUNDED,
  DEAD
} typedef States;


/** 
 * @brief Represents one section of the snake 
 */
struct {
  int current_x;  /**<x coordinate in pixels*/
  int current_y;  /**<y coordinate in pixels*/
} typedef section;

/** 
 * @brief Represents snake
 */
struct {
    int current_x; /**< x coordinate in pixels*/
    int current_y; /**< y coordinate in pixels*/
    int section_size; /**< size of one section in pixels*/
    int sections_number; /**< number of sections*/
    States current_state; /**< current state of the snake*/
    section* sections; /**< array of the sections of the snake*/
    Directions* directions;  /**<array of directions where each section has its own direction */
} typedef snake;

/**
 * @brief Represents obstacle
 */
struct {
  int current_x; /**< x coordinate in pixels*/
  int current_y; /**< y coordinate in pixels*/
  int width;    /**< width of the obstacle in pixels*/
  int height;   /**< height of the obstacle in pixels*/

} typedef obstacle;

#endif