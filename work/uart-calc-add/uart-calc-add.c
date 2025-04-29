#define _POSIX_C_SOURCE 200112L


/*
 * Next macros provides location of knobs and LEDs peripherals
 * implemented on QtRVSim simulator.
 *
 * More information can be found on page
 *   https://github.com/cvut/qtrvsim
 */


/*
 * Base address of the region where simple serial port (UART)
 * implementation is mapped in emulated RISC-V address space
 */
#define SERIAL_PORT_BASE   0xffffc000

/*
 * Byte offset of the 32-bit receive status register
 * of the serial port
 */
#define SERP_RX_ST_REG_o         0x00
/*
 * Mask of the bit which inform that received character is ready
 to be read by CPU.
 */
#define SERP_RX_ST_REG_READY_m    0x1
/*
 * Byte offset of the UART received data register.
 * When the 32-bit word is read the least-significant (LSB)
 * eight bits are represet last complete byte received from terminal.
 */

#define SERP_RX_DATA_REG_o        0x04
/*
 * Byte offset of the 32-bit transition status register
 * of the serial port
 */
#define SERP_TX_ST_REG_o         0x08
/*
 * Mask of the bit which inform that peripheral is ready to accept
 * next character to send. If it is zero, then peripheral is
 * busy by sending of previous character.
 */
#define SERP_TX_ST_REG_READY_m    0x1

/*
 * Byte offset of the UART transmit register.
 * When the 32-bit word is written the least-significant (LSB)
 * eight bits are send to the terminal.
 */
#define SERP_TX_DATA_REG_o        0x0c

/*
 * Base address of the region where knobs and LEDs peripherals
 * are mapped in the emulated RISC-V physical memory address space.
 */
#define SPILED_REG_BASE      0xffffc100

/* Valid address range for the region */
#define SPILED_REG_SIZE      0x00000100

/*
 * Byte offset of the register which controls individual LEDs
 * in the row of 32 yellow LEDs. When the corresponding bit
 * is set (value 1) then the LED is lit.
 */
#define SPILED_REG_LED_LINE_o           0x004

/*
 * The register to control 8 bit RGB components of brightness
 * of the first RGB LED
 */
#define SPILED_REG_LED_RGB1_o           0x010

/*
 * The register to control 8 bit RGB components of brightness
 * of the second RGB LED
 */
#define SPILED_REG_LED_RGB2_o           0x014

/*
 * The register which combines direct write to RGB signals
 * of the RGB LEDs, write to the keyboard scan register
 * and control of the two additional individual LEDs.
 * The direct write to RGB signals is orred with PWM
 * signal generated according to the values in previous
 * registers.
 */
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

/*
 * Register providing access to unfiltered encoder channels
 * and keyboard return signals.
 */
#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020

/*
 * The register representing knobs positions as three
 * 8-bit values where each value is incremented
 * and decremented by the knob relative turning.
 */
#define SPILED_REG_KNOBS_8BIT_o         0x024

/*
 * The main entry into example program
 */
int main(int argc, char *argv[])
{
  int* mem = (int*)(SERIAL_PORT_BASE);
  int read_status_o = SERP_RX_ST_REG_o / 4;
  int read_data_o = SERP_RX_DATA_REG_o / 4;
  int write_statuus_o = SERP_TX_ST_REG_o / 4;
  int write_data_o = SERP_TX_DATA_REG_o / 4;

  int result = 0, counter = 0;
  unsigned int first_number = 0;
  unsigned char data[12] = {'\0'};

  int number_of_digits;
  int idx = 0;
  while (counter < 2){
    if (mem[read_status_o] == SERP_RX_ST_REG_READY_m){

      data[idx] = mem[read_data_o];

      if (data[idx] == 10){
        if (idx >= number_of_digits)
          number_of_digits = idx + 1;

        int c = 1 ;
        for (int i = idx - 1; i >= 0; i--){
          first_number += (data[i] - 48) * c;
          c *= 10;
        }
        result += first_number;
        counter++;
        idx = -1;
        first_number = 0;
      }
      idx++;
    }
  }


  while(1){
    if (mem[write_statuus_o] == SERP_TX_ST_REG_READY_m){
      mem[write_data_o] = '\n';
      break;
  }
}

  int c = 1;
  for (int i = 0; i < number_of_digits - 1; i++){
    c *= 10;
  }
  
  int temp = result / c;
  if (temp != 0) {
    while (1){
      if (mem[write_statuus_o] == SERP_TX_ST_REG_READY_m){
        mem[write_data_o] = temp + 48;
        break;
      }
    }
    result %= c;
  }
 
  c /= 10;
  while (c > 0){  
    if (mem[write_statuus_o] == SERP_TX_ST_REG_READY_m){
      mem[write_data_o] = result / c + 48;
      result %= c;
      c /= 10;
    }
  }


  return 0;
}
