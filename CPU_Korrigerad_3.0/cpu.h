#ifndef CPU_H_
#define CPU_H_

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define NOP   0x00 // G�r inget, hoppa till n�sta rad.
#define COPY  0x01 // Kopierar v�rde mellan CPU-registrer specificerade i operanderna. V�rdet lagras i registret angivet i operand 1.
#define COPYI 0x02 // Kopierar direkt v�rde fr�n operand 2 till registret angivet i operand 1.
#define IN    0x03 // L�ser av I/O-port med operand 2 och l�ger v�rde i registret angivet i operand 1.
#define INP   0x04 // L�ser av v�rde fr�n refererad adress i operand 2 till angivet register i operand 1.
#define OUT   0x05 // Skriver v�rdet fr�n I/O-porten angiven i operand 2 och l�gger det i dataminnets adress agnivet i operand 1.
#define OUTI  0x06 // Skriver v�rde angivet i operand 2 till I/O-porten angivet i operand 1.
#define OUTP  0x07 // Skriver v�rde fr�n adressen angiven i operand 2 till I/O-porten angiven som adress i operand 1.
#define CLR   0x08 // Raderar v�rde i CPU-register.
#define AND   0x09 // Utf�r bitvis AND med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define ANDI  0x0A // Utf�r bitvis AND med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
#define OR    0x0B // Utf�r bitvis OR med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define ORI   0x0C // Utf�r bitvis OR med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
#define XOR   0x0D // Utf�r bitvis XOR med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define XORI  0x0E // Utf�r bitvis XOR med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
#define ADD   0x0F // Utf�r addition med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.

#define ADDI  0x10 // Utf�r addition med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
#define SUB   0x11 // Utf�r subtraktion med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define SUBI  0x12 // Utf�r subtraktion med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
#define MUL   0x13 // Utf�r multiplikation med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define MULI  0x14 // Utf�r multiplikation med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
#define DIV   0x15 // Utf�r division med v�rdena i CPU-registerna angivna i operand 1 och 2. Restultat lagras i CPU-registret angivet i operand 1.
#define DIVI  0x16 // Utf�r division med ett direkt v�rde angivet i operand 2 och v�rde fr�n CPU-registret angivet i operand 1 d�r ocks� resultatet lagras.
#define INC   0x17 // Inkrementerar v�rdet i CPU-registret angivet i operand 1.
#define DEC   0x18 // Dekrementerar v�rdet i CPU-registret angivet i operand 1.
#define POP   0x19 // Poppar v�rdet fr�n stacken till CPU-registret angivet i operand 1.
#define PUSH  0x1A // L�gger in v�rdet fr�n CPU-register angivet i operand 1 p� stacken.
#define CALL  0x1B // Sparar adressen som hoppas ifr�n p� stack och hoppar till v�rdet angivet i operand 1.
#define RET   0x1C // Hoppar tillbaka till v�rdet som poppas fr�n stacken.
#define RETI  0x1D // Status-registret s�tts till vad det var innan RETI kallades p�. Programmet hoppar tillbaka till vart de var innan RETI kallade p�. Interupt-flaggan aktiveras igen.
#define CP    0x1E // J�mf�r v�rdena i dem tv� register angivna i operand 1 och 2.
#define CPI   0x1F // J�mf�r v�rde i registret angivet i operand 1 med v�rdet i operand 2.

#define JMP   0x20 // Hoppar till angiven adress fr�n operand 1.
#define JE    0x21 // Hoppar till angiven adress fr�n operand 1 ifall Zero-flaggan �r aktiverad.
#define JNE   0x22 // Hoppar till angiven adress fr�n operand 1 ifall Zero-flaggan �r inaktiverad.
#define JGE   0x23 // Hoppar till angiven adress fr�n operand 1 ifall Signed-flaggan �r inaktiverad.
#define JGT   0x24 // Hoppar till angiven adress fr�n operand 1 ifall b�de Zero-flaggan och Signed-flaggan �r inaktiverade.
#define JLE   0x25 // Hoppar till angiven adress fr�n operand 1 ifall b�de Zero-flaggan och Signed-flaggan �r aktiverade.
#define JLT   0x26 // Hoppar till angiven adress fr�n operand 1 ifall Signed-flaggan �r aktiverad.
#define LSL   0x27 // Skiftar bitarna i v�rden angivet i operand 1 till v�nster.
#define LSR   0x28 // Skiftar bitarna i v�rden angivet i operand 1 till h�ger.
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
#define PCINTA_vect 0x02 // Avbrottsvektor f�r PCI-avbrott p� I/O-portar.

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