/* Implementation of the subroutine subroutine_fnc */

#include "subroutine.h"

extern int data;

int subroutine_fnc(char a4)
{

  if (122 < a4){
    return a4;
  }

  if (96 < a4){
    data += 1;
    return a4 -32;
  }
  return a4;
}
