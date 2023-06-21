#include "a.out.h"
#include "cpu.h"
#include "op.h"
#include "tools.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Gets the mod, reg, rm and disp of the current op
void get_mod_reg_rm_disp(CPU* cpu, uint8_t* mod, uint8_t* reg, uint8_t* rm, int16_t* disp) {
    uint8_t src = read_byte(cpu);

    *mod = (src >> 6) & 0x03;
    *reg = (src >> 3) & 0x07;
    *rm  = src & 0x07;
    *disp = 0;

    if (*mod == 0b11) return;

    if (*mod == 0b00 && *rm != 0b110) return;

    if (*mod == 0b01) {
        *disp = (int8_t)read_byte(cpu);
    } else {
        *disp = read_word(cpu);
    }
}

// Gets the effective address of the current op (Effective address here is the address inside the emulator's memory)
uint8_t* get_ea(CPU* cpu, uint8_t mod, uint8_t rm, int16_t disp, uint8_t w) {
    if (mod == 0b11) 
        return (w) ? (uint8_t*)get_register_16(cpu, rm) : get_register_8(cpu, rm);
    if (mod == 0b00 && rm == 0b110) 
        return cpu->data +  disp;

    switch (rm) {
        case 0b000:
            return cpu->data + (uint16_t)(*get_register_16(cpu, BX) + *get_register_16(cpu, SI) + disp);
        case 0b001:
            return cpu->data + (uint16_t)(*get_register_16(cpu, BX) + *get_register_16(cpu, DI) + disp);
        case 0b010:
            return cpu->data + (uint16_t)(*get_register_16(cpu, BP) + *get_register_16(cpu, SI) + disp);
        case 0b011:
            return cpu->data + (uint16_t)(*get_register_16(cpu, BP) + *get_register_16(cpu, DI) + disp);
        case 0b100:
            return cpu->data + (uint16_t)(*get_register_16(cpu, SI) + disp);
        case 0b101:
            return cpu->data + (uint16_t)(*get_register_16(cpu, DI) + disp);
        case 0b110:
            return cpu->data + (uint16_t)(*get_register_16(cpu, BP) + disp);
        case 0b111:
            return cpu->data + (uint16_t)(*get_register_16(cpu, BX) + disp);
    }  
    return NULL;
}

uint8_t* get_memory_with_seg(CPU* cpu, enum CPU_SEGMENT_REGISTERS seg, uint16_t addr) {
    return cpu->data + addr + (cpu->seg_registers[seg] << 4);
}

// Gets string representation of the register
char* get_reg_str(uint8_t reg, uint8_t w) {
    char* str = (char*)calloc(3, sizeof(char));

    const char* reg16_str[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
    const char* reg8_str[]  = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };

    if (w)
        sprintf(str, "%s", reg16_str[reg]);
    else
        sprintf(str, "%s", reg8_str[reg]);

    return str;
}

// Gets string representation of the segment
char* get_seg_str(uint8_t seg) {
    char* str = (char*)calloc(3, sizeof(char));

    const char* seg_str[] = { "es", "cs", "ss", "ds" };

    sprintf(str, "%s", seg_str[seg]);

    return str;
}

// Gets string representation of the register/memory
char* get_rm_str(uint8_t mod, uint8_t rm, int16_t disp, uint8_t w) {
    if (mod == 0b11) return get_reg_str(rm, w);

    char* str = (char*)calloc(33, sizeof(char));

    if (mod == 0b00 && rm == 0b110) {
        sprintf(str, "[%04hx]", disp);
        return str;
    }

    const char* rm_str[] = { "bx+si", "bx+di", "bp+si", "bp+di", "si", "di", "bp", "bx" };
    if (!disp) {
        sprintf(str, "[%s]", rm_str[rm]);
        return str;
    }
    sprintf(str, "[%s%c%x]", rm_str[rm], disp < 0 ? '-' : '+', abs(disp));
    return str;
}

// Prints an operand
void print_op(CPU* cpu, const char* op, uint8_t space, int argc, char** argv, char* add_info) {
    for (uint16_t addr = cpu->op_start; addr < cpu->pc; addr++) 
        fprintf(stderr, "%02hx", get_byte(cpu, addr));
    for (size_t i = 0; i < DISASSEMBLING_SPACES - 2 * (cpu->pc - cpu->op_start) + space; i++) 
        fprintf(stderr, " ");

    if (cpu->state > NORMAL)
        fprintf(stderr, "rep ");
    fprintf(stderr, "%s", op);
    if (!argc) {
        fprintf(stderr, "\n");
        return;
    }
    fprintf(stderr, " ");

    for (int i = 0; i < argc - 1; i++) {
        fprintf(stderr, "%s, ", argv[i]);
        free(argv[i]);
    }
    fprintf(stderr, "%s", argv[argc - 1]);
    if (add_info)
        fprintf(stderr, " ;%s", add_info);
    fprintf(stderr, "\n");
    free(argv[argc - 1]);
    free(add_info);
}

// Prints disassembled instruction
void print_disassembling(CPU* cpu, const char* op, int argc, char** argv) {
    fprintf(stderr, "%04hx: ", cpu->op_start);
    print_op(cpu, op, 1, argc, argv, NULL);
}

// Prints debugging informations (Registers, flags and disassembled instruction)
void print_debugging(CPU* cpu, const char* op, int argc, char** argv, uint16_t* ea[2]) {
    fprintf(stderr, "%04hx ", *get_register_16(cpu, AX));
    fprintf(stderr, "%04hx ", *get_register_16(cpu, BX));
    fprintf(stderr, "%04hx ", *get_register_16(cpu, CX));
    fprintf(stderr, "%04hx ", *get_register_16(cpu, DX));
    fprintf(stderr, "%04hx ", *get_register_16(cpu, SP));
    fprintf(stderr, "%04hx ", *get_register_16(cpu, BP));
    fprintf(stderr, "%04hx ", *get_register_16(cpu, SI));
    fprintf(stderr, "%04hx ", *get_register_16(cpu, DI));

    fprintf(stderr, (get_flag(cpu, OF) ? "O" : "-"));
    fprintf(stderr, (get_flag(cpu, SF) ? "S" : "-"));
    fprintf(stderr, (get_flag(cpu, ZF) ? "Z" : "-"));
    fprintf(stderr, (get_flag(cpu, CF) ? "C" : "-"));

    fprintf(stderr, " ");
    fprintf(stderr, "%04hx:", cpu->op_start);

    char* add_info = NULL;
    if (ea && (size_t)ea[1] >= (size_t)cpu->data && (size_t)ea[1] < (size_t)cpu->data + MEM_SIZE) {
        add_info = (char*)calloc(33, sizeof(char));
        if (ea[0])
            sprintf(add_info, "[%04lx]%04x", (size_t)ea[1] - (size_t)cpu->data, *ea[1]);
        else
            sprintf(add_info, "[%04lx]%02x", (size_t)ea[1] - (size_t)cpu->data, (uint8_t)*ea[1]);
    }

    print_op(cpu, op, 0, argc, argv, add_info);
}