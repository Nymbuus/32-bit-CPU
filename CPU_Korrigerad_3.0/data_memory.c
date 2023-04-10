/********************************************************************************
* control_unit.c: Hanterar dataminnet. B�de skriva och l�sa av minnet.
*                 Kan g�ras bitvis.
********************************************************************************/
#include "data_memory.h"

// Array med alla dataminnesadresser.
static uint32_t data_memory[DATA_MEMORY_ADDRESS_WIDTH];

/********************************************************************************
* data_memory_reset: Nollar alla v�rden i dataminnet.
********************************************************************************/
void data_memory_reset(void)
{
   for(uint32_t i = 0; i < DATA_MEMORY_ADDRESS_WIDTH; i++)
   {
      data_memory[i] = 0;
   }

   return;
}

/********************************************************************************
* data_memory_address_clear: Nollar specifikt angiven dataminnesadress.
********************************************************************************/
void data_memory_address_clear(const uint16_t address)
{
   data_memory[address] = 0;

   return;
}

/********************************************************************************
* data_memory_write: Skriver specificerat v�rdet i angiven adress i dataminnet.
********************************************************************************/
int data_memory_write(const uint16_t address, const uint32_t value)
{
   if (address < DATA_MEMORY_ADDRESS_WIDTH && address >= 0)
   {
      data_memory[address] = value;
      return 0;
   }
   else
   {
      return 1;
   }
}

/********************************************************************************
* data_memory_read: L�ser av angiven adress i dataminnet och returnerar det.
*                   �r dataminnet fullt eller av annan anledning under noll
*                   returneras noll.
********************************************************************************/
uint32_t data_memory_read(const uint16_t address)
{
   if (address < DATA_MEMORY_ADDRESS_WIDTH && address >= 0)
   {
      return data_memory[address];
   }   
   else
   {
      return 0;
   }
}

/********************************************************************************
* data_memory_set_bit: Ettst�ller specificerad bit med angiven adress.
********************************************************************************/
void data_memory_set_bit(const uint16_t address, const uint32_t bit)
{
   data_memory[address] |= (1 << bit);

   return;
}

/********************************************************************************
* data_memory_clear_bit: Nollst�ller specificerad bit med angiven adress.
********************************************************************************/
void data_memory_clear_bit(const uint16_t address, const uint32_t bit)
{
   data_memory[address] &= ~(1 << bit);

   return;
}