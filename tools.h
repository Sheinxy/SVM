#ifndef _TOOLS_H
#define _TOOLS_H

#include "cpu.h"
#include <stdint.h>

#define DISASSEMBLING_SPACES 13

void get_mod_reg_rm_disp(CPU* cpu, uint8_t* mod, uint8_t* reg, uint8_t* rm, int16_t* disp);
uint8_t* get_ea(CPU* cpu, uint8_t mod, uint8_t rm, int16_t disp, uint8_t w);
uint8_t* get_memory_with_seg(CPU* cpu, enum CPU_SEGMENT_REGISTERS seg, uint16_t addr);

char* get_rm_str(uint8_t mod, uint8_t rm, int16_t disp, uint8_t w);
char* get_reg_str(uint8_t reg, uint8_t w);
char* get_seg_str(uint8_t seg);

void print_op(CPU* cpu, const char* op, uint8_t space, int argc, char** argv, char* add_info);
void print_disassembling(CPU* cpu, const char* op, int argc, char** argv);
void print_debugging(CPU* cpu, const char* op, int argc, char** argv, uint16_t* ea[2]);

#endif