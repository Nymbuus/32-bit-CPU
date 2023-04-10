/********************************************************************************
* program_memory.c: Här tas assemblerkoden och skrivs in i arrayen data.
*                   Denna array tas sedan till control_unit för att exekveras.
********************************************************************************/
#ifndef PROGRAM_MEMORY_H_
#define PROGRAM_MEMORY_H_

#include "control_unit.h"

#define PROGRAM_MEMORY_DATA_WIDTH 32
#define PROGRAM_MEMORY_ADDRESS_WIDTH 100


void program_memory_write(void);

uint64_t program_memory_read(const uint16_t address);

#endif /* PROGRAM_MEMORY_H_ */