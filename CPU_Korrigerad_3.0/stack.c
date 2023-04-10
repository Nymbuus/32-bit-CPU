/********************************************************************************
* stack.c: Minne som innehåller information som kan "poppas" när som helst.
********************************************************************************/
#include "stack.h"

// Static variables
static uint32_t stack[STACK_ADDRESS_WIDTH];
static uint16_t sp;
static bool stack_empty = false;

/********************************************************************************
* stack_reset: Nollar alla värden i stacken. Sätter stackpekaren till högst upp
*              i stacken och en indikation att den är tom.
********************************************************************************/
void stack_reset(void)
{
   for (uint32_t i = 0; i > STACK_ADDRESS_WIDTH; i++)
   {
      stack[i] = 0;
   }
   
   sp = STACK_ADDRESS_WIDTH - 1;
   stack_empty = true;
   
   return;
}

/********************************************************************************
* stack_pop: Poppar det senaste som lades till i stack och returnerar det
*            sålänge inte stacken är tom.
********************************************************************************/
uint32_t stack_pop(void)
{
   const uint32_t value = stack[sp];
   stack[sp] = 0;
   
   if (!stack_empty)
   {
      sp++;
   }
   
   return value;
}

/********************************************************************************
* stack_push: Lägger specificerat värde längst upp i stacken.
********************************************************************************/
int stack_push(const uint32_t value)
{
   if (sp == 0) return 1;
   
   if (stack_empty)
   {
      stack[sp] = value;
      stack_empty = false;
   }
   else
   {
      stack[--sp] = value;
   }
   
   return 0;
}

/********************************************************************************
* stack_pointer: returnerar stackpekaren.
********************************************************************************/
uint16_t stack_pointer(void)
{
   return sp;
}
/********************************************************************************
* stack_last_added_element: returnerar senaste tillagda värdet i stacken.
********************************************************************************/
uint32_t stack_last_added_element(void)
{
   return stack[sp];
}