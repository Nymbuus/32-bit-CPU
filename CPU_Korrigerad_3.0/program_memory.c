/********************************************************************************
* program_memory.c: Här tas assemblerkoden och skrivs in i arrayen data.
*                   Denna array tas sedan till control_unit för att exekveras.
********************************************************************************/
#include "program_memory.h"

// Macro definitions
#define ISR_PCINTA     4
#define button2_check  9
#define button3_check  14
#define ISR_PCINTA_end 18
#define main           19
#define setup          21

#define LED1_toggle    35
#define LED1_off       37
#define LED1_on        42

#define LED2_toggle    47
#define LED2_off       49
#define LED2_on        54

#define LED3_toggle    59
#define LED3_off       61
#define LED3_on        66

#define main_loop      71


#define LED1 PORTA8
#define LED2 PORTA9
#define LED3 PORTA10

#define BUTTON1 PORTA11
#define BUTTON2 PORTA12
#define BUTTON3 PORTA13

// Varibel som indikerar om vi skrivit till programminnet.
static bool program_memory_initialized = false;

// Array som innehåller alla assembler-instruktioner.
uint64_t data[PROGRAM_MEMORY_ADDRESS_WIDTH];

/********************************************************************************
* assemble: Läser in tre parametrar, op-koden, operand 1 och operand 2 i
*           variabeln instructions. För att få allt i ordning bitvis skiftas
*           op-koden och operand 1 åt MSB-hållet.
********************************************************************************/
static inline uint64_t assemble(const uint16_t op_code, const uint16_t op1, const uint32_t op2);

void program_memory_write(void)
{
   if (program_memory_initialized) return;
   program_memory_initialized = true;

   /********************************************************************************
   * RESET_vect: Vektor som representerar start av assemblerkod. Hoppar direkt till
   *             main där systemet startar.
   ********************************************************************************/
   data[0] = assemble(JMP, main, 0x00);
   data[1] = assemble(NOP, 0x00, 0x00);
   
   /********************************************************************************
   * PCINTA_vect: Avbrottsvektor för PCI-avbrott på I/O-portar. Vi hoppar till
   *              motsvarande avbrottsrutin ISR_PCINTA för att hantera avbrottet.
   ********************************************************************************/
   data[2] = assemble(JMP, ISR_PCINTA, 0x00);
   data[3] = assemble(NOP, 0x00, 0x00);
   
   /********************************************************************************
   * ISR_PCINTA: Avbrottsrutin för PCI-avbrott på I/O-portar som äger rum vid
   *             nedtryckning och uppsläppning av tryckknappar. Vid nedtryckning
   *             togglas lysndioden som är kopplad till respektive knapp.
   ********************************************************************************/
   data[4] =  assemble(IN, R24, PINA);
   data[5] =  assemble(ANDI, R24, (1 << BUTTON1));
   data[6] =  assemble(JE, button2_check, 0x00);
   data[7] =  assemble(CALL, LED1_toggle, 0x00);
   data[8] =  assemble(JMP, ISR_PCINTA_end, 0x00);
   data[9] =  assemble(IN, R24, PINA);
   data[10] = assemble(ANDI, R24, (1 << BUTTON2));
   data[11] = assemble(JE, button3_check, 0x00);
   data[12] = assemble(CALL, LED2_toggle, 0x00);
   data[13] = assemble(JMP, ISR_PCINTA_end, 0x00);
   data[14] = assemble(IN, R24, PINA);
   data[15] = assemble(ANDI, R24, (1 << BUTTON3));
   data[16] = assemble(JE, ISR_PCINTA_end, 0x00);
   data[17] = assemble(CALL, LED3_toggle, 0x00);
   data[18] = assemble(RETI, 0x00, 0x00);
   
   /********************************************************************************
   * main: Initierar systemstart.
   ********************************************************************************/
   data[19] = assemble(CALL, setup, 0x00);
   data[20] = assemble(JMP, main_loop, 0x00);
   
   /********************************************************************************
   * setup: Aktiverar leds, knappar och avbrott.
   ********************************************************************************/
   data[21] = assemble(COPYI, R16, (1 << LED1));
   data[22] = assemble(COPYI, R17, (1 << LED2));
   data[23] = assemble(COPYI, R18, (1 << LED3));
   data[24] = assemble(COPYI, R19, (1 << LED1) | (1 << LED2) | (1 << LED3));
   data[25] = assemble(OUT, DDRA, R19);
   data[26] = assemble(COPYI, XREG, 0x00);
   data[27] = assemble(COPYI, YREG, 0x00);
   data[28] = assemble(COPYI, ZREG, 0x00);
   data[29] = assemble(COPYI, R20, (1 << BUTTON1) | (1 << BUTTON2) | (1 << BUTTON3));
   data[30] = assemble(OUT, PORTA, R20);
   data[31] = assemble(SEI, 0x00, 0x00);
   data[32] = assemble(OUTI, ICR, (1 << PCIEA)); 
   data[33] = assemble(OUT, PCMSKA, R20); 
   data[34] = assemble(RET, 0x00, 0x00);
   
   /********************************************************************************
   * LED1_toggle: Togglar LED1 mellan av och på.
   ********************************************************************************/
   data[35] = assemble(CPI, XREG, 0x00);
   data[36] = assemble(JE, LED1_on, 0x00);

   /********************************************************************************
   * LED1_off: Släcker LED1.
   ********************************************************************************/
   data[37] = assemble(IN, R24, PORTA);
   data[38] = assemble(ANDI, R24, ~(1 << LED1));
   data[39] = assemble(OUT, PORTA, R24);
   data[40] = assemble(COPYI, XREG, 0x00);
   data[41] = assemble(RET, 0x00, 0x00);
   
   /********************************************************************************
   * LED1_on: Tänder LED1.
   ********************************************************************************/
   data[42] = assemble(IN, R24, PORTA);
   data[43] = assemble(ORI, R24, (1 << LED1));
   data[44] = assemble(OUT, PORTA, R24);
   data[45] = assemble(COPYI, XREG, 0x01);
   data[46] = assemble(RET, 0x00, 0x00);
   
   /********************************************************************************
   * LED2_toggle: Togglar LED2 mellan av och på.
   ********************************************************************************/
   data[47] = assemble(CPI, YREG, 0x00);
   data[48] = assemble(JE, LED2_on, 0x00);

   /********************************************************************************
   * LED2_off: Släcker LED2.
   ********************************************************************************/
   data[49] = assemble(IN, R24, PORTA);
   data[50] = assemble(ANDI, R24, ~(1 << LED2));
   data[51] = assemble(OUT, PORTA, R24);
   data[52] = assemble(COPYI, YREG, 0x00);
   data[53] = assemble(RET, 0x00, 0x00);
   
   /********************************************************************************
   * LED2_on: Tänder LED2.
   ********************************************************************************/
   data[54] = assemble(IN, R24, PORTA);
   data[55] = assemble(ORI, R24, (1 << LED2));
   data[56] = assemble(OUT, PORTA, R24);
   data[57] = assemble(COPYI, YREG, 0x01);
   data[58] = assemble(RET, 0x00, 0x00);
   
   /********************************************************************************
   * LED3_toggle: Togglar LED3 mellan av och på.
   ********************************************************************************/
   data[59] = assemble(CPI, ZREG, 0x00);
   data[60] = assemble(JE, LED3_on, 0x00);

   /********************************************************************************
   * LED3_off: Släcker LED3.
   ********************************************************************************/
   data[61] = assemble(IN, R24, PORTA);
   data[62] = assemble(ANDI, R24, ~(1 << LED3));
   data[63] = assemble(OUT, PORTA, R24);
   data[64] = assemble(COPYI, ZREG, 0x00);
   data[65] = assemble(RET, 0x00, 0x00);
   
   /********************************************************************************
   * LED3_on: Tänder LED3.
   ********************************************************************************/
   data[66] = assemble(IN, R24, PORTA);
   data[67] = assemble(ORI, R24, (1 << LED3));
   data[68] = assemble(OUT, PORTA, R24);
   data[69] = assemble(COPYI, ZREG, 0x01);
   data[70] = assemble(RET, 0x00, 0x00);

   /********************************************************************************
   * main_loop: Här stannar programmet tills ett avbrott sker och då hoppar 
   *            programräknaren till rad 3 (data[2]).
   ********************************************************************************/   
   data[71] = assemble(JMP, main_loop, 0x00);
   
   return;
}

/********************************************************************************
* program_memory_read: Läser av innehållet i addressen och returnerar värdet.
*                      Är minnet fullt eller av någon anledning under 0
*                      returneras 0.
********************************************************************************/
uint64_t program_memory_read(const uint16_t address)
{
   if(address < PROGRAM_MEMORY_ADDRESS_WIDTH && address >= 0)
   {
      return data[address];
   }
   else
   {
      return 0;
   }
}

/********************************************************************************
* assemble: Läser in tre parametrar, op-koden, operand 1 och operand 2 i
*           variabeln instructions. För att få allt i ordning bitvis skiftas
*           op-koden och operand 1 åt MSB-hållet.
********************************************************************************/
static inline uint64_t assemble(const uint16_t op_code, const uint16_t op1, const uint32_t op2)
{
   const uint64_t instruction = ((uint64_t)(op_code) << 48) | ((uint64_t)(op1) << 32) | op2;
   return instruction;
}