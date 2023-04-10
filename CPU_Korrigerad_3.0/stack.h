/********************************************************************************
* stack.c: Minne som innehåller information som kan "poppas" när som helst.
********************************************************************************/
#ifndef STACK_H_
#define STACK_H_

#include "cpu.h"

#define STACK_ADDRESS_WIDTH 100
#define STACK_DATA_WIDTH    32

void stack_reset(void);

uint32_t stack_pop(void);

int stack_push(const uint32_t value);

uint16_t stack_pointer(void);

uint32_t stack_last_added_element(void);

#endif /* STACK_H_ */