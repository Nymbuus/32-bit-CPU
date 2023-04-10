/********************************************************************************
* control_unit.c: Här hämtas instruktionen in från programminnet (FETCH).
*                 Efter det avkodas den till tre variabler, op-kod, operand 1
*                 och operand 2 (DECODE).
*                 Till sist exekveras intruktionen (EXECUTE).
********************************************************************************/
#include "control_unit.h"

// Statiska functioner
static void check_for_irq(void);
static void generate_interrupt(const uint8_t interrupt_vector);
static void monitor_PINA(void);
static inline void update_io(void);

// Statiska variabler
static uint64_t ir; // Instruktion register.
static uint16_t pc;  // Program räknaren.
static uint16_t mar; // Memory address register, en variabel som innehåller vart i koden programmet befinner sig.
static uint8_t sr;  // Status register, innehåller status bitarna ISNZVC.

static uint16_t op_code; // Innhåller OP-koden.
static uint16_t op1;     // Innehåller operand 1.
static uint32_t op2;     // Innehåller operand2.

static enum cpu_state state;                     // Innehåller viken läge CPUn är i, FETCH, DECODE eller EXECUTE.
static uint32_t reg[CPU_REGISTER_ADDRESS_WIDTH]; // CPU-register R0 - R31.

static uint32_t PINA_previous; /* Innehåller förra cykelns insignal på PINA. */

/********************************************************************************
* control_unit_reset: Startar upp/om control_unit och tillhörande minnen.
********************************************************************************/
void control_unit_reset(void)
{
	ir = 0x00;
	pc = 0x00;
	mar = 0x00;
	sr = 0x00;

	op_code = 0x00;
	op1 = 0x00;
	op2 = 0x00;

	state = CPU_STATE_FETCH;

	PINA_previous = 0x00;

	for (uint32_t i = 0; i < CPU_REGISTER_ADDRESS_WIDTH; ++i)
	{
		reg[i] = 0;
	}

	data_memory_reset();
	stack_reset();
	program_memory_write();
	return;
}

/********************************************************************************
* control_unit_run_next_state: Kör igenom en alla lägen i CPU-cykeln.
********************************************************************************/
void control_unit_run_next_state(void)
{
	switch (state)
	{
		case CPU_STATE_FETCH:
		{
			ir = program_memory_read(pc); /* Fetches next instruction. */
			mar = pc;                     /* Stores address of current instruction. */
			pc++;                         /* Program counter points to next instruction. */
			state = CPU_STATE_DECODE;     /* Decodes the instruction during next clock cycle. */
			break;
		}
		case CPU_STATE_DECODE:
		{
			op_code = ir >> 48;           /* Bit 63 downto 48 consists of the OP code. */
			op1 = ir >> 32;                /* Bit 47 downto 32 consists of the first operand. */
			op2 = ir;                     /* Bit 31 downto 0 consists of the second operand. */
			state = CPU_STATE_EXECUTE;    /* Executes the instruction during next clock cycle. */
			break;
		}
		case CPU_STATE_EXECUTE:
		{
			switch (op_code)
			{
				case NOP: // Gör inget, hoppa till nästa rad.
				{
					break;
				}
				case COPY: // Kopierar värde mellan CPU-registrer specificerade i operanderna. Värdet lagras i registret angivet i operand 1.
				{
					reg[op1] = reg[op2];
					break;
				}
				case COPYI: // Kopierar direkt värde från operand 2 till registret angivet i operand 1.
				{
					reg[op1] = op2;
					break;
			    }
		     	case IN: // Läser av I/O-port med operand 2 och läger värde i registret angivet i operand 1.
				{
					reg[op1] = data_memory_read(op2);
					break;
				}
				case INP: // Läser av värde från refererad adress i operand 2 till angivet register i operand 1.
				{
					reg[op1] = data_memory_read(reg[op2]);
					break;
				}
				case OUT: // Skriver värdet från I/O-porten angiven i operand 2 och lägger det i dataminnets adress agnivet i operand 1.
				{
					data_memory_write(op1, reg[op2]);
					break;
				}
				case OUTI: // Skriver värde angivet i operand 2 till I/O-porten angivet i operand 1.
				{
					data_memory_write(op1, op2);
					break;
				}
				case OUTP: // Skriver värde från adressen angiven i operand 2 till I/O-porten angiven som adress i operand 1.
				{
					data_memory_write(reg[op1], reg[op2]);
					break;
				}
				case CLR: // Raderar värde i CPU-register.
				{
					reg[op1] = 0x00;
					break;
	         }
	         case AND: // Utför bitvis AND med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
	         {
   	         reg[op1] = alu(AND, reg[op1], reg[op2], &sr);
   	         break;
            }
            case ANDI: // Utför bitvis AND med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
            {
               reg[op1] = alu(AND, reg[op1], op2, &sr);
               break;
            }
            case OR: // Utför bitvis OR med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
            {
               reg[op1] = alu(OR, reg[op1], reg[op2], &sr);
               break;
            }
				case ORI: // Utför bitvis OR med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
				{
					reg[op1] = alu(OR, reg[op1], op2, &sr);
					break;
				}
				case XOR: // Utför bitvis XOR med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(XOR, reg[op1], reg[op2], &sr);
					break;
				}
				case XORI: // Utför bitvis XOR med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
				{
					reg[op1] = alu(XOR, reg[op1], op2, &sr);
					break;
				}
				case ADD: // Utför addition med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(ADD, reg[op1], reg[op2], &sr);
					break;
				}
				case ADDI: // Utför addition med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
				{
					reg[op1] = alu(ADD, reg[op1], op2, &sr);
					break;
				}
				case SUB: // Utför subtraktion med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(SUB, reg[op1], reg[op2], &sr);
					break;
				}
				case SUBI: // Utför subtraktion med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
				{
					reg[op1] = alu(SUB, reg[op1], op2, &sr);
					break;
				}
            case MUL: // Utför multiplikation med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
            {
               reg[op1] = alu(MUL, reg[op1], reg[op2], &sr);
               break;
            }
            case MULI: // Utför multiplikation med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
            {
               reg[op1] = alu(MUL, reg[op1], op2, &sr);
               break;
            }
            case DIV: // Utför division med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
            {
               reg[op1] = alu(DIV, reg[op1], reg[op2], &sr);
               break;
            }
            case DIVI: // Utför division med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
            {
               reg[op1] = alu(DIV, reg[op1], op2, &sr);
               break;
            }               
				case INC: // Inkrementerar värdet i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(ADD, reg[op1], 1, &sr);
					break;
				}
				case DEC: // dekrementerar värdet i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(SUB, reg[op1], 1, &sr);
					break;
				}
            case POP: // Poppar värdet från stacken till CPU-registret angivet i operand 1.
            {
               reg[op1] = stack_pop();
               break;
            }
            case PUSH: // Lägger in värdet från CPU-register angivet i operand 1 på stacken.
            {
               stack_push(reg[op1]);
               break;
            }
            case CALL: // Sparar adressen som hoppas ifrån på stack och hoppar till värdet angivet i operand 1.
            {
               stack_push(pc);
               pc = op1;
               break;
            }
            case RET: // Hoppar tillbaka till värdet som poppas från stacken.
            {
               pc = stack_pop();
               break;
            }
            case RETI: // Status-registret sätts till vad det var innan RETI kallades på.
                       // Programmet hoppar tillbaka till vart de var innan RETI kallade på.
                       // Interupt-flaggan aktiveras igen.
            {
               sr = stack_pop();
               pc = stack_pop();
			      sr |= (1 << I); 
               break;
            }
            case CP: // Jämför värdena i dem två register angivna i operand 1 och 2.
            {
               (void)alu(SUB, reg[op1], reg[op2], &sr);
               break;
            }
				case CPI: // Jämför värde i registret angivet i operand 1 med värdet i operand 2.
				{
					(void)alu(SUB, reg[op1], op2, &sr); /* Return value is not stored. */
					break;
				}
				case JMP: // Hoppar till angiven adress från operand 1.
				{
					pc = op1;
					break;
				}
				case JE: // Hoppar till angiven adress från operand 1 ifall Zero-flaggan är aktiverad.
				{
					if ((sr & (1 << Z))) pc = op1;
					break;
				}
				case JNE: // Hoppar till angiven adress från operand 1 ifall Zero-flaggan är inaktiverad.
				{
					if (!(sr & (1 << Z))) pc = op1;
					break;
				}
				case JGE: // Hoppar till angiven adress från operand 1 ifall Signed-flaggan är inaktiverad.
				{
					if (!(sr & (1 << S))) pc = op1;
					break;
				}
				case JGT: // Hoppar till angiven adress från operand 1 ifall både Zero-flaggan och Signed-flaggan är inaktiverade.
				{
					if (!(sr & (1 << S)) && !(sr & (1 << Z))) pc = op1;
					break;
				}
				case JLE: // Hoppar till angiven adress från operand 1 ifall både Zero-flaggan och Signed-flaggan är aktiverade.
				{
					if ((sr & (1 << S)) || (sr & (1 << Z))) pc = op1;
					break;
				}
				case JLT: // Hoppar till angiven adress från operand 1 ifall Signed-flaggan är aktiverad.
				{
					if ((sr & (1 << S))) pc = op1;
					break;
				}
				case LSL: // Skiftar bitarna i värden angivet i operand 1 till vänster.
				{
					reg[op1] = reg[op1] << 1;
					break;
				}
				case LSR: // Skiftar bitarna i värden angivet i operand 1 till höger.
				{
					reg[op1] = reg[op1] >> 1;
					break;
				}
				case SEI: // Aktiverar interupt-flaggan.
				{
					sr |= (1 << I);
					break;
				}
				case CLI: // Inaktiverar interupt-flaggan.
				{
					sr &= ~(1 << I);
					break;
				}
				default: // Systemet startar om ifall något oväntat händer.
				{
					control_unit_reset();
					break;
				}
			}

			state = CPU_STATE_FETCH;
			check_for_irq();
			break;
		}
		default: // Systemet startar om ifall något oväntat händer.
		{
			control_unit_reset();
			break;
		}
	}

   update_io();
	monitor_PINA();
	return;
}

/********************************************************************************
* control_unit_run_next_state: Kör nästa CPU-cykel.
********************************************************************************/
void control_unit_run_next_instruction_cycle(void)
{
	do
	{
		control_unit_run_next_state();
	} while (state != CPU_STATE_EXECUTE);
	return;
}

/********************************************************************************
* check_for_irq: Kollar ifall ett avbrott ska ske. Om detta är fallet genereras
*                avbrott och programmet hoppar till PCINTA_vektorn data adress.
********************************************************************************/
static void check_for_irq(void)
{
	if ((sr & (1 << I)))
	{
		const uint32_t ifr = data_memory_read(IFR);
		const uint32_t icr = data_memory_read(ICR);

		if ((ifr & (1 << PCIFA)) && (icr & (1 << PCIEA)))
		{
			data_memory_clear_bit(IFR, PCIFA);
			generate_interrupt(PCINTA_vect);
		}
	}
	return;
}

/********************************************************************************
* generate_interrupt: Genererar avbrott. pushar nuvarande programrad och
*                     statusregister till stacken. Stänger av interupt-flaggan
*                     så inte ett till avbrott kan ske medans vi håller på att
*                     hantera det nuvarande. Programräknaren sätts till
*                     avbrottsvektorn som är PCINTA_vect.
********************************************************************************/
static void generate_interrupt(const uint8_t interrupt_vector)
{
	stack_push(pc);
	stack_push(sr);
	sr &= ~(1 << I);
	pc = interrupt_vector;
	return;
}

/********************************************************************************
* monitor_PINA: Monitors pin change interrupts on I/O ports. All pins where
*               pin change monitoring is enabled (corresponding mask bit in
*               Pin Change Mask Register A i set) is monitored by comparing
*               current input signal versus the old one. If they don't match,
*               the corresponding PCIFA (pin change interrupt flag)
*               in the ifr (Interrupt Flag Register)
*               register is set to generate an interrupt request (IRQ).
********************************************************************************/
static void monitor_PINA(void)
{
	const uint32_t PINA_current = data_memory_read(PINA);
	const uint32_t pcmska = data_memory_read(PCMSKA);

	for (uint32_t i = 0; i < DATA_MEMORY_DATA_WIDTH; ++i)
	{
		if ((pcmska & (1 << i)))
		{
			if ((bool)(PINA_current & (1 << i)) != (bool)(PINA_previous & (1 << i)))
			{
				data_memory_set_bit(IFR, PCIFA);
				break;
			}
		}
	}

	PINA_previous = PINA_current;
	return;
}

/********************************************************************************
* update_io: Uppdaterar I/O-portarna. Lägger allt i ett enda register
*           (register A).
********************************************************************************/
static inline void update_io(void)
{
   const uint32_t ddra = data_memory_read(DDRA);
   const uint32_t porta = data_memory_read(PORTA);
   const uint32_t pina = ((uint32_t)(PINC) << 16) | ((uint16_t)(PINB) << 8) | PIND;

   DDRD = (uint8_t)(ddra);
   DDRB = (uint8_t)(ddra >> 8);
   DDRC = (uint8_t)(ddra >> 16);

   PORTD = (uint8_t)(porta);
   PORTB = (uint8_t)(porta >> 8);
   PORTC = (uint8_t)(porta >> 16);
   
   data_memory_write(PINA, pina);
   return;
}