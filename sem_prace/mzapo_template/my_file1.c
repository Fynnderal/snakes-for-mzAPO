#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

int main(int argc, char *argv[]) {
  unsigned char *mem_base;
  uint32_t val_line=5;
  printf("Hello world\n");

  sleep(1);

  /*
   * Setup memory mapping which provides access to the peripheral
   * registers region of RGB LEDs, knobs and line of yellow LEDs.
   */
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  /* If mapping fails exit with error code */
  if (mem_base == NULL)
    exit(1);

  struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000};


  uint32_t otoc = 0; 

  while (((otoc>>24) & 7) == 0) {
    otoc = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o ) = otoc&0xff0000;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o ) = otoc&0x0000ff;

    printf("LED val 0x%x\n", otoc);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }

    while ((val_line & 1) == 0) {
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    val_line>>=1;
    printf("LED val 0x%x\n", val_line);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }

  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0; 
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0; 



  printf("Goodbye world\n");

  return 0;
}
