#ifndef _CPU_H
#define _CPU_H

#include "a.out.h"
#include <stdio.h>
#include <stdint.h>


#define MEM_SIZE 0x10000

enum CPU_RUNNING_MODE {
    DISASSEMBLING,
    DEBUGGING,
    RUNNING
};

enum CPU_STATE {
    NORMAL,
    REPEATNE,
    REPEAT
};

typedef struct cpu {
    A_HEADER *header; // The header of the a.out file

    uint8_t *text; // The text segment of the a.out file
    uint8_t *data; // The data segment of the a.out file (and CPU memory)

    uint16_t pc; // The program counter
    uint16_t p_brk; // The program break;

    uint16_t op_start; // The start address of the opcode

    uint16_t registers[8]; // The registers
    uint16_t seg_registers[8]; // The segment registers
    uint16_t flags; // The flags

    enum CPU_RUNNING_MODE running_mode; // The mode of the CPU 
    enum CPU_STATE state; // CPU state
} CPU;

enum CPU_REGISTERS_16 {
    AX,
    CX,
    DX,
    BX,
    SP,
    BP,
    SI,
    DI
};

enum CPU_REGISTERS_8 {
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH
};

enum CPU_SEGMENT_REGISTERS {
    ES,
    CS,
    SS,
    DS
};

enum CPU_FLAGS {
    CF = 1,
    PF = 4,
    AF = 16,
    ZF = 64,
    SF = 128,
    TF = 256,
    IF = 512,
    DF = 1024,
    OF = 2048
};

CPU* get_cpu(FILE* file);
void init_stack_pointer(CPU* cpu, int argc, char** argv);
void free_cpu(CPU* cpu);

uint8_t get_byte(CPU* cpu, uint16_t address);
uint8_t read_byte(CPU* cpu);
uint16_t get_word(CPU* cpu, uint16_t address);
uint16_t read_word(CPU* cpu);

uint16_t* get_register_16(CPU* cpu, enum CPU_REGISTERS_16 reg);
void set_register_16(CPU* cpu, enum CPU_REGISTERS_16 reg, uint16_t val);
uint8_t* get_register_8(CPU* cpu, enum CPU_REGISTERS_8 reg);
void set_register_8(CPU* cpu, enum CPU_REGISTERS_8 reg, uint8_t val);
uint8_t get_flag(CPU* cpu, enum CPU_FLAGS flag);
void set_flag(CPU* cpu, enum CPU_FLAGS flag, uint8_t value);

uint16_t add16(CPU* cpu, uint16_t a, uint16_t b);
uint8_t add8(CPU* cpu, uint8_t a, uint8_t b);
uint16_t sub16(CPU* cpu, uint16_t a, uint16_t b);
uint8_t sub8(CPU* cpu, uint8_t a, uint8_t b);

uint16_t and16(CPU* cpu, uint16_t a, uint16_t b);
uint8_t and8(CPU* cpu, uint8_t a, uint8_t b);
uint16_t or16(CPU* cpu, uint16_t a, uint16_t b);
uint8_t or8(CPU* cpu, uint8_t a, uint8_t b);
uint16_t xor16(CPU* cpu, uint16_t a, uint16_t b);
uint8_t xor8(CPU* cpu, uint8_t a, uint8_t b);

uint16_t neg16(CPU* cpu, uint16_t a);
uint8_t neg8(CPU* cpu, uint8_t a);

uint16_t shl16(CPU* cpu, uint16_t a, uint8_t b);
uint8_t shl8(CPU* cpu, uint8_t a, uint8_t b);
uint16_t shr16(CPU* cpu, uint16_t a, uint8_t b);
uint8_t shr8(CPU* cpu, uint8_t a, uint8_t b);

int16_t sar16(CPU* cpu, int16_t a, uint8_t b);
int8_t sar8(CPU* cpu, int8_t a, uint8_t b);


void push_word(CPU* cpu, uint16_t val);
void push_byte(CPU* cpu, uint8_t val);
void pop_word(CPU* cpu, uint16_t* reg);
void pop_byte(CPU* cpu, uint8_t* reg);

int16_t cpu_brk(CPU* cpu, uint16_t address);

#endif