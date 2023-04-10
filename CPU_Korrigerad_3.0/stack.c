/********************************************************************************
* stack.c: Minne som inneh�ller information som kan "poppas" n�r som helst.
********************************************************************************/
#include "stack.h"

// Static variables
static uint32_t stack[STACK_ADDRESS_WIDTH];
static uint16_t sp;
static bool stack_empty = false;

/********************************************************************************
* stack_reset: Nollar alla v�rden i stacken. S�tter stackpekaren till h�gst upp
*              i stacken och en indikation att den �r tom.
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
*            s�l�nge inte stacken �r tom.
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
* stack_push: L�gger specificerat v�rde l�ngst upp i stacken.
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
* stack_last_added_element: returnerar senaste tillagda v�rdet i stacken.
********************************************************************************/
uint32_t stack_last_added_element(void)
{
   return stack[sp];
}