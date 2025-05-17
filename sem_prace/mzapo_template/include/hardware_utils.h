#ifndef HARDWARE_UTILS_H
#define HARDWARE_UTILS_H

#include "gameobjects.h"
#include "consts.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include <stdbool.h>

extern volatile uint32_t knob;
extern unsigned char* mem_base;

/**
 * Initializes utils. Must be called before using these functions
 */
void init_hardware_utils();

/**
 * Resets hardware to the default state
 */
void reset_hardware();

/**
 * Gets delta between old and new value of the knob.
 * 
 * @param knob_id - id of the knob.
 */
int get_delta(int knob_id);

/**
 * Updates state of the RGB LED
 * 
 * @param led_id - id of the led.
 * @param state - current state of the snake for which LED should be updated
 */
void update_LED(int led_id, States state);

/**
 * Updates state of the LED line.
 * 
 * @param snake_to_update - snake for which state should be updated.
 * @param left - decides whether left or right side of the line must be updated.
 */
void update_LED_line(snake* snake_to_update, bool left);

/**
 * Determines whther knob was pressed.
 * 
 * @param knob_id - id of the knob
 * 
 * @return - returns true if knob was pressed, false otherwise.
 */
bool button_pressed(int knob_id);

#endif