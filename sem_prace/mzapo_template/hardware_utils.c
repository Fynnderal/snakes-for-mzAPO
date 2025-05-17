#include "hardware_utils.h"

volatile uint32_t otoc;
unsigned char* mem_base;

void init_hardware_utils(){
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        exit(1);

    otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o); 
}

void reset_hardware(){
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0;
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0;
}

int get_delta(int otoc_id){
  int delta = 0;
  
  volatile uint32_t novy_otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);

  if (otoc_id == 0){
    delta = (int) ((novy_otoc >> 16)&0xff) - (int)((otoc >> 16)&0xff);
  }
  else if (otoc_id == 1){
    delta = (int) ((novy_otoc >> 8)&0xff) - (int)((otoc >> 8)&0xff);
  }
  else{
    delta = (int) (novy_otoc &0xff) - (int)(otoc &0xff);
  }

  if (delta > 128){
    delta -= 256;
  }

  if (delta < -128){
    delta += 256;
  }

  if (delta != 0)
    otoc = novy_otoc;



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

bool button_pressed(int otoc_id){  
  volatile uint32_t novy_otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);

  bool pressed = false;

  if (otoc_id == 0){
    pressed = (int) ((novy_otoc >> 26)&0x01) == 1;
  }
  else if (otoc_id == 1){
    pressed = (int) ((novy_otoc >> 25)&0x01)  == 1;
  }
  else{
    pressed = (int) ((novy_otoc >> 24) &0x01) == 1;
  }

  return pressed;
}