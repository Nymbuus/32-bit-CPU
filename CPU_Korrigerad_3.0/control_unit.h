/********************************************************************************
* control_unit.c: Här hämtas instruktionen in från programminnet (FETCH).
*                 Efter det avkodas den till tre variabler, op-kod, operand 1
*                 och operand 2 (DECODE).
*                 Till sist exekveras intruktionen (EXECUTE).
********************************************************************************/
#ifndef CONTROL_UNIT_H_
#define CONTROL_UNIT_H_

#include "cpu.h"
#include "data_memory.h"
#include "stack.h"
#include "alu.h"
#include "program_memory.h"

void control_unit_reset(void);
void control_unit_run_next_state(void);

#endif /* CONTROL_UNIT_H_ */