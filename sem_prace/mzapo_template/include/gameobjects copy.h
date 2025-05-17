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


struct {
  int current_x;
  int current_y;
} typedef section;

struct {
    int current_x;
    int current_y;
    int section_size;
    int sections_number;
    States current_state;
    section* sections;
    Directions* directions;
} typedef snake;


struct {
  int current_x;
  int current_y;
  int width;
  int height;

} typedef obstacle;

#endif