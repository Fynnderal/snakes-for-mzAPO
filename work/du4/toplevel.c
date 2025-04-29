/* Implementation of the subroutine toplevel_fnc */

#include "subroutine.h"
#include <stdio.h> 

int data = 0;

int toplevel_fnc(void)
{
  char b;
  int a5;
  while ((a5 = read(0, &b, 1)) == 1){
    b = (int) subroutine_fnc(b);
    a5 = write(1, &b, 1);
    if (a5 < 0)
      break;
  }
  return data;
}

          

