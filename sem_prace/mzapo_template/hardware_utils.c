/*******************************************************************
  Program provides utils for controlling hardware to the game "Snakes" for the MZ_APO board


  (C) Copyright 2025 by Abdzhanov Aidar
      e-mail:   abdzhaid@cvut.cz
      github: https://gitlab.fel.cvut.cz/B242_B0B35APO/abdzhaid/-/tree/main/sem_prace?ref_type=heads
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/


#include "hardware_utils.h"

volatile uint32_t knob;
unsigned char* mem_base;

void init_hardware_utils(){
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        exit(1);

    knob = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o); 
}

void reset_hardware(){
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0;
}

int get_delta(int knob_id){
  int delta = 0;
  
  volatile uint32_t new_knob = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);

  // gets specified delta
  if (knob_id == 0){
    delta = (int) ((new_knob >> 16)&0xff) - (int)((knob >> 16)&0xff);
  }
  else if (knob_id == 1){
    delta = (int) ((new_knob >> 8)&0xff) - (int)((knob >> 8)&0xff);
  }
  else{
    delta = (int) (new_knob &0xff) - (int)(knob &0xff);
  }

  // solves oberflow problem
  if (delta > 128){
    delta -= 256;
  }

  if (delta < -128){
    delta += 256;
  }

  // updates state of the knob.
  if (delta != 0)
    knob = new_knob;

  return delta;
}

void update_LED(int led_id, States state){
  int color = 0;

  switch (state) {
    case OK:
      color = 0x00ff00;
      break;
    case INJURED:
      color = 0x0000ff;
      break;
    case WOUNDED:
      color = 0xff0000;
      break;
    case DEAD:
      color = 0;
      break;
    }
  if (led_id == 0)
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = color;
  else if(led_id == 1)
      *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = color;

}

void update_LED_line(snake* snake_to_update, bool left){
  volatile uint32_t previous_value = *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o);

  if (!left){
  int temp = 1 << (snake_to_update->sections_number - DEFALT_SNAKE_SIZE - 1);
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = previous_value | temp; 
  }else{
    int temp = 1 << (31 - snake_to_update->sections_number + DEFALT_SNAKE_SIZE + 1);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = previous_value | temp;
  }
}

bool button_pressed(int knob_id){  
  volatile uint32_t new_knob = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);

  bool pressed = false;

  if (knob_id == 0){
    pressed = (int) ((new_knob >> 26)&0x01) == 1;
  }
  else if (knob_id == 1){
    pressed = (int) ((new_knob >> 25)&0x01)  == 1;
  }
  else{
    pressed = (int) ((new_knob >> 24) &0x01) == 1;
  }

  return pressed;
}