/********************************************************************************
* alu.c: Utför dem logiska operationerna.
********************************************************************************/
#include "alu.h"

/********************************************************************************
* alu: Utför en logisk operation med variablerna a och b. Efter det sätts de
*      olika statusflaggorna. Resultatet returnes med 64 bitar storlek.
********************************************************************************/
uint64_t alu(const uint8_t operation,
             const uint32_t a,
             const uint32_t b,
             uint8_t *sr)
{
	uint64_t result = 0;
	*sr &= ~((1 << S) | (1 << N) | (1 << Z) | (1 << V) | (1 << C));
	
	switch(operation)
	{
		case AND:
		{
			result = a & b;
			break;
		}
		case OR:
		{
			result = a | b;
			break;
		}
		case XOR:
		{
			result = a ^ b;
			break;
		}
		case ADD:
		{
			result = a + b; // Kanske (65536 - b)
			
			if ((a & (1UL << 31)) == (b & (1UL << 31)) && (result & (1UL << 31)) != (a & (1UL << 31)))
			{
				*sr |= (1 << V);
			}

			break;
		}
		case SUB:
		{
			result = a - b;
			
			if ((a & (1UL << 31)) == (b & (1UL << 31)) && (result & (1UL << 31)) != (a & (1UL << 31)))
			{
				*sr |= (1 << V);
			}
			
			break;
		}
		case MUL:
		{
			result = a * b;
			break;
		}
		case DIV:
		{
			result = a / b;
			break;
		}
	}
	
   // statusflaggor.
	if ((uint32_t)(result == 0)) *sr |= (1 << Z);
	if ((result & (1UL << 31)) == 1) *sr |= (1 << N);
	if ((result & (1ULL << 32)) == 1) *sr |= (1 << C);
	if ((bool)(*sr & (1UL << N)) != (bool)(*sr & (1 << V)))
	{
		*sr |= (1 << S);
	}
	
	return result;
}