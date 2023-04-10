#ifndef CPU_H_
#define CPU_H_

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define NOP   0x00 // Gör inget, hoppa till nästa rad.
#define COPY  0x01 // Kopierar värde mellan CPU-registrer specificerade i operanderna. Värdet lagras i registret angivet i operand 1.
#define COPYI 0x02 // Kopierar direkt värde från operand 2 till registret angivet i operand 1.
#define IN    0x03 // Läser av I/O-port med operand 2 och läger värde i registret angivet i operand 1.
#define INP   0x04 // Läser av värde från refererad adress i operand 2 till angivet register i operand 1.
#define OUT   0x05 // Skriver värdet från I/O-porten angiven i operand 2 och lägger det i dataminnets adress agnivet i operand 1.
#define OUTI  0x06 // Skriver värde angivet i operand 2 till I/O-porten angivet i operand 1.
#define OUTP  0x07 // Skriver värde från adressen angiven i operand 2 till I/O-porten angiven som adress i operand 1.
#define CLR   0x08 // Raderar värde i CPU-register.
#define AND   0x09 // Utför bitvis AND med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define ANDI  0x0A // Utför bitvis AND med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
#define OR    0x0B // Utför bitvis OR med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define ORI   0x0C // Utför bitvis OR med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
#define XOR   0x0D // Utför bitvis XOR med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define XORI  0x0E // Utför bitvis XOR med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
#define ADD   0x0F // Utför addition med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.

#define ADDI  0x10 // Utför addition med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
#define SUB   0x11 // Utför subtraktion med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define SUBI  0x12 // Utför subtraktion med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
#define MUL   0x13 // Utför multiplikation med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define MULI  0x14 // Utför multiplikation med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
#define DIV   0x15 // Utför division med värdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define DIVI  0x16 // Utför division med ett direkt värde angivet i operand 2 och värde från CPU-registret angivet i operand 1 där också resultatet lagras.
#define INC   0x17 // Inkrementerar värdet i CPU-registret angivet i operand 1.
#define DEC   0x18 // Dekrementerar värdet i CPU-registret angivet i operand 1.
#define POP   0x19 // Poppar värdet från stacken till CPU-registret angivet i operand 1.
#define PUSH  0x1A // Lägger in värdet från CPU-register angivet i operand 1 på stacken.
#define CALL  0x1B // Sparar adressen som hoppas ifrån på stack och hoppar till värdet angivet i operand 1.
#define RET   0x1C // Hoppar tillbaka till värdet som poppas från stacken.
#define RETI  0x1D // Status-registret sätts till vad det var innan RETI kallades på. Programmet hoppar tillbaka till vart de var innan RETI kallade på. Interupt-flaggan aktiveras igen.
#define CP    0x1E // Jämför värdena i dem två register angivna i operand 1 och 2.
#define CPI   0x1F // Jämför värde i registret angivet i operand 1 med värdet i operand 2.

#define JMP   0x20 // Hoppar till angiven adress från operand 1.
#define JE    0x21 // Hoppar till angiven adress från operand 1 ifall Zero-flaggan är aktiverad.
#define JNE   0x22 // Hoppar till angiven adress från operand 1 ifall Zero-flaggan är inaktiverad.
#define JGE   0x23 // Hoppar till angiven adress från operand 1 ifall Signed-flaggan är inaktiverad.
#define JGT   0x24 // Hoppar till angiven adress från operand 1 ifall både Zero-flaggan och Signed-flaggan är inaktiverade.
#define JLE   0x25 // Hoppar till angiven adress från operand 1 ifall både Zero-flaggan och Signed-flaggan är aktiverade.
#define JLT   0x26 // Hoppar till angiven adress från operand 1 ifall Signed-flaggan är aktiverad.
#define LSL   0x27 // Skiftar bitarna i värden angivet i operand 1 till vänster.
#define LSR   0x28 // Skiftar bitarna i värden angivet i operand 1 till höger.
#define SEI   0x29 // Aktiverar interupt-flaggan.
#define CLI   0x2A // Inaktiverar interupt-flaggan.

#define DDRA  0x00 // Port A Data Direction Register.
#define PORTA 0x01 // Port A Data Register.
#define PINA  0X02 // Port A Input Pins Address.

#define PCMSKA 0x03 // Port A Pin Change Mask Register.
#define ICR    0x04 // Interupt Control Register
#define IFR    0x05 // Ineterupt Flag Register

#define PCIEA  0x00 // Port A Pin Change Interrupt Enable bit.
#define PCIFA  0x00 // Port A Pin Change Interrupt Flag bit.

#define RESET_vect 0x00 // Vektor som representerar start av assemblerkod.
#define PCINTA_vect 0x02 // Avbrottsvektor för PCI-avbrott på I/O-portar.

// CPU-register.
#define R0   0x00
#define R1   0x01
#define R2   0x02
#define R3   0x03
#define R4   0x04
#define R5   0x05
#define R6   0x06
#define R7   0x07
#define R8   0x08
#define R9   0x09
#define R10  0x0A
#define R11  0x0B
#define R12  0x0C
#define R13  0x0D
#define R14  0x0E
#define R15  0x0F

#define R16  0x10
#define R17  0x11
#define R18  0x12
#define R19  0x13
#define R20  0x14
#define R21  0x15
#define R22  0x16
#define R23  0x17
#define R24  0x18
#define R25  0x19
#define R26  0x1A
#define R27  0x1B
#define R28  0x1C
#define R29  0x1D
#define R30  0x1E
#define R31  0x1F

#define XLOW R26
#define XHIGH R27
#define YLOW R28
#define YHIGH R29
#define ZLOW R30
#define ZHIGH R31
#define XREG XLOW
#define YREG YLOW
#define ZREG ZLOW

// Data register
#define PORTA0   0
#define PORTA1   1
#define PORTA2   2
#define PORTA3   3
#define PORTA4   4
#define PORTA5   5
#define PORTA6   6
#define PORTA7   7

#define PORTA8   8
#define PORTA9   9
#define PORTA10  10
#define PORTA11  11
#define PORTA12  12
#define PORTA13  13

#define PORTA14  14
#define PORTA15  15 
#define PORTA16  16 
#define PORTA17  17 
#define PORTA18  18 
#define PORTA19  19 

// Status-flaggor
#define I 5 /* Interrupt flag in status register. */
#define S 4 /* Signed flag in status register. */
#define N 3 /* Negative flag in status register. */
#define Z 2 /* Zero flag in status register. */
#define V 1 /* Overflow flag in status register. */
#define C 0 /* Carry flag in status register. */

#define CPU_REGISTER_ADDRESS_WIDTH 32 /* 32 CPU registers in control unit. */
#define CPU_REGISTER_DATA_WIDTH    8  /* 8 bit data width per CPU register. */
#define IO_REGISTER_DATA_WIDTH     8  /* 8 bit data width per I/O location. */

enum cpu_state
{
	CPU_STATE_FETCH,  /* Fetches next instruction from program memory. */
	CPU_STATE_DECODE, /* Decodes the fetched instruction. */
	CPU_STATE_EXECUTE /* Executes the decoded instruction. */
};


#endif /* CPU_H_ */