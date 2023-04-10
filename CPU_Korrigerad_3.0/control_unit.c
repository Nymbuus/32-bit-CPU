/********************************************************************************
* control_unit.c: H�r h�mtas instruktionen in fr�n programminnet (FETCH).
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
static uint16_t pc;  // Program r�knaren.
static uint16_t mar; // Memory address register, en variabel som inneh�ller vart i koden programmet befinner sig.
static uint8_t sr;  // Status register, inneh�ller status bitarna ISNZVC.

static uint16_t op_code; // Innh�ller OP-koden.
static uint16_t op1;     // Inneh�ller operand 1.
static uint32_t op2;     // Inneh�ller operand2.

static enum cpu_state state;                     // Inneh�ller viken l�ge CPUn �r i, FETCH, DECODE eller EXECUTE.
static uint32_t reg[CPU_REGISTER_ADDRESS_WIDTH]; // CPU-register R0 - R31.

static uint32_t PINA_previous; /* Inneh�ller f�rra cykelns insignal p� PINA. */

/********************************************************************************
* control_unit_reset: Startar upp/om control_unit och tillh�rande minnen.
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
* control_unit_run_next_state: K�r igenom en alla l�gen i CPU-cykeln.
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
				case NOP: // G�r inget, hoppa till n�sta rad.
				{
					break;
				}
				case COPY: // Kopierar v�rde mellan CPU-registrer specificerade i operanderna. V�rdet lagras i registret angivet i operand 1.
				{
					reg[op1] = reg[op2];
					break;
				}
				case COPYI: // Kopierar direkt v�rde fr�n operand 2 till registret angivet i operand 1.
				{
					reg[op1] = op2;
					break;
			    }
		     	case IN: // L�ser av I/O-port med operand 2 och l�ger v�rde i registret angivet i operand 1.
				{
					reg[op1] = data_memory_read(op2);
					break;
				}
				case INP: // L�ser av v�rde fr�n refererad adress i operand 2 till angivet register i operand 1.
				{
					reg[op1] = data_memory_read(reg[op2]);
					break;
				}
				case OUT: // Skriver v�rdet fr�n I/O-porten angiven i operand 2 och l�gger det i dataminnets adress agnivet i operand 1.
				{
					data_memory_write(op1, reg[op2]);
					break;
				}
				case OUTI: // Skriver v�rde angivet i operand 2 till I/O-porten angivet i operand 1.
				{
					data_memory_write(op1, op2);
					break;
				}
				case OUTP: // Skriver v�rde fr�n adressen angiven i operand 2 till I/O-porten angiven som adress i operand 1.
				{
					data_memory_write(reg[op1], reg[op2]);
					break;
				}
				case CLR: // Raderar v�rde i CPU-register.
				{
					reg[op1] = 0x00;
					break;
	         }
	         case AND: // Utf�r bitvis AND med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
	         {
   	         reg[op1] = alu(AND, reg[op1], reg[op2], &sr);
   	         break;
            }
            case ANDI: // Utf�r bitvis AND med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
            {
               reg[op1] = alu(AND, reg[op1], op2, &sr);
               break;
            }
            case OR: // Utf�r bitvis OR med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
            {
               reg[op1] = alu(OR, reg[op1], reg[op2], &sr);
               break;
            }
				case ORI: // Utf�r bitvis OR med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
				{
					reg[op1] = alu(OR, reg[op1], op2, &sr);
					break;
				}
				case XOR: // Utf�r bitvis XOR med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(XOR, reg[op1], reg[op2], &sr);
					break;
				}
				case XORI: // Utf�r bitvis XOR med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
				{
					reg[op1] = alu(XOR, reg[op1], op2, &sr);
					break;
				}
				case ADD: // Utf�r addition med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(ADD, reg[op1], reg[op2], &sr);
					break;
				}
				case ADDI: // Utf�r addition med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
				{
					reg[op1] = alu(ADD, reg[op1], op2, &sr);
					break;
				}
				case SUB: // Utf�r subtraktion med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(SUB, reg[op1], reg[op2], &sr);
					break;
				}
				case SUBI: // Utf�r subtraktion med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
				{
					reg[op1] = alu(SUB, reg[op1], op2, &sr);
					break;
				}
            case MUL: // Utf�r multiplikation med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
            {
               reg[op1] = alu(MUL, reg[op1], reg[op2], &sr);
               break;
            }
            case MULI: // Utf�r multiplikation med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
            {
               reg[op1] = alu(MUL, reg[op1], op2, &sr);
               break;
            }
            case DIV: // Utf�r division med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
            {
               reg[op1] = alu(DIV, reg[op1], reg[op2], &sr);
               break;
            }
            case DIVI: // Utf�r division med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
            {
               reg[op1] = alu(DIV, reg[op1], op2, &sr);
               break;
            }               
				case INC: // Inkrementerar v�rdet i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(ADD, reg[op1], 1, &sr);
					break;
				}
				case DEC: // dekrementerar v�rdet i CPU-registret angivet i operand 1.
				{
					reg[op1] = alu(SUB, reg[op1], 1, &sr);
					break;
				}
            case POP: // Poppar v�rdet fr�n stacken till CPU-registret angivet i operand 1.
            {
               reg[op1] = stack_pop();
               break;
            }
            case PUSH: // L�gger in v�rdet fr�n CPU-register angivet i operand 1 p� stacken.
            {
               stack_push(reg[op1]);
               break;
            }
            case CALL: // Sparar adressen som hoppas ifr�n p� stack och hoppar till v�rdet angivet i operand 1.
            {
               stack_push(pc);
               pc = op1;
               break;
            }
            case RET: // Hoppar tillbaka till v�rdet som poppas fr�n stacken.
            {
               pc = stack_pop();
               break;
            }
            case RETI: // Status-registret s�tts till vad det var innan RETI kallades p�.
                       // Programmet hoppar tillbaka till vart de var innan RETI kallade p�.
                       // Interupt-flaggan aktiveras igen.
            {
               sr = stack_pop();
               pc = stack_pop();
			      sr |= (1 << I); 
               break;
            }
            case CP: // J�mf�r v�rdena i dem tv� register angivna i operand 1 och 2.
            {
               (void)alu(SUB, reg[op1], reg[op2], &sr);
               break;
            }
				case CPI: // J�mf�r v�rde i registret angivet i operand 1 med v�rdet i operand 2.
				{
					(void)alu(SUB, reg[op1], op2, &sr); /* Return value is not stored. */
					break;
				}
				case JMP: // Hoppar till angiven adress fr�n operand 1.
				{
					pc = op1;
					break;
				}
				case JE: // Hoppar till angiven adress fr�n operand 1 ifall Zero-flaggan �r aktiverad.
				{
					if ((sr & (1 << Z))) pc = op1;
					break;
				}
				case JNE: // Hoppar till angiven adress fr�n operand 1 ifall Zero-flaggan �r inaktiverad.
				{
					if (!(sr & (1 << Z))) pc = op1;
					break;
				}
				case JGE: // Hoppar till angiven adress fr�n operand 1 ifall Signed-flaggan �r inaktiverad.
				{
					if (!(sr & (1 << S))) pc = op1;
					break;
				}
				case JGT: // Hoppar till angiven adress fr�n operand 1 ifall b�de Zero-flaggan och Signed-flaggan �r inaktiverade.
				{
					if (!(sr & (1 << S)) && !(sr & (1 << Z))) pc = op1;
					break;
				}
				case JLE: // Hoppar till angiven adress fr�n operand 1 ifall b�de Zero-flaggan och Signed-flaggan �r aktiverade.
				{
					if ((sr & (1 << S)) || (sr & (1 << Z))) pc = op1;
					break;
				}
				case JLT: // Hoppar till angiven adress fr�n operand 1 ifall Signed-flaggan �r aktiverad.
				{
					if ((sr & (1 << S))) pc = op1;
					break;
				}
				case LSL: // Skiftar bitarna i v�rden angivet i operand 1 till v�nster.
				{
					reg[op1] = reg[op1] << 1;
					break;
				}
				case LSR: // Skiftar bitarna i v�rden angivet i operand 1 till h�ger.
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
				default: // Systemet startar om ifall n�got ov�ntat h�nder.
				{
					control_unit_reset();
					break;
				}
			}

			state = CPU_STATE_FETCH;
			check_for_irq();
			break;
		}
		default: // Systemet startar om ifall n�got ov�ntat h�nder.
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
* control_unit_run_next_state: K�r n�sta CPU-cykel.
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
* check_for_irq: Kollar ifall ett avbrott ska ske. Om detta �r fallet genereras
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
*                     statusregister till stacken. St�nger av interupt-flaggan
*                     s� inte ett till avbrott kan ske medans vi h�ller p� att
*                     hantera det nuvarande. Programr�knaren s�tts till
*                     avbrottsvektorn som �r PCINTA_vect.
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
* update_io: Uppdaterar I/O-portarna. L�gger allt i ett enda register
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