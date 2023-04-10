/********************************************************************************
* alu.h: Utför dem logiska operationerna.
********************************************************************************/
#ifndef ALU_H_
#define ALU_H_

#include "cpu.h"

uint64_t alu(const uint8_t operation,
             const uint32_t a,
             const uint32_t b,
             uint8_t *sr);


#endif /* ALU_H_ */