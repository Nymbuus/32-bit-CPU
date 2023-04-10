/********************************************************************************
* control_unit.c: Hanterar dataminnet. Både skriva och läsa av minnet.
*                 Kan göras bitvis.
********************************************************************************/
#ifndef DATA_MEMORY_H_
#define DATA_MEMORY_H_

#include "cpu.h"

#define DATA_MEMORY_DATA_WIDTH    32
#define DATA_MEMORY_ADDRESS_WIDTH 100

void data_memory_reset(void);

void data_memory_address_clear(const uint16_t address);

int data_memory_write(const uint16_t address, const uint32_t value);

uint32_t data_memory_read(const uint16_t address);

void data_memory_set_bit(const uint16_t address, const uint32_t bit);

void data_memory_clear_bit(const uint16_t address, const uint32_t bit);

#endif /* DATA_MEMORY_H_ */