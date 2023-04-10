/********************************************************************************
* main.c: Demonstration av en 32-bitars CPU.
********************************************************************************/
#include "control_unit.h"
#include "cpu.h"

int main(void)
{
   control_unit_reset();
   
   while (1) 
   {
      control_unit_run_next_state();
   }
}

