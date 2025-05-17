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

extern volatile uint32_t otoc;
extern unsigned char* mem_base;

void init_hardware_utils();
void reset_hardware();
int get_delta(int otoc_id);
void update_LED(int led_id, States state);
void update_LED_line(snake* snakeToUpdate, bool left);
bool button_pressed(int otoc_id);

#endif