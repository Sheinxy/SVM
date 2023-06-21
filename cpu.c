#include "a.out.h"
#include "cpu.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Creates the CPU from the a.out file
CPU* get_cpu(FILE* f) {
    fseek(f, 0, SEEK_SET);

    CPU* cpu = (CPU*)calloc(1, sizeof(CPU));
    cpu->header = (A_HEADER*)malloc(sizeof(A_HEADER));
    fread(cpu->header, A_HEADER_SIZE, 1, f);

    fseek(f, cpu->header->a_hdrlen, SEEK_SET);
    cpu->text = (uint8_t*)malloc(cpu->header->a_text);
    fread(cpu->text, cpu->header->a_text, 1, f);

    cpu->data = (uint8_t*)calloc(1, MEM_SIZE);
    fread(cpu->data, cpu->header->a_data, 1, f);

    cpu->p_brk = cpu->header->a_data;
    cpu->running_mode = RUNNING;
    cpu->state = NORMAL; 

    return cpu;
}

// Initializes the stack pointer
void init_stack_pointer(CPU* cpu, int argc, char** argv) {
    set_register_16(cpu, SP, 0x0000);

    const char* env = "PATH=/usr:/usr/bin";

    uint16_t* addr = (uint16_t*)calloc(argc, sizeof(uint16_t));
    uint16_t env_head;

    uint16_t len = strlen(env) + 1;

    for (int i = 0; i < argc; i++)
        len += strlen(argv[i]) + 1;
    
    if (len % 2) push_byte(cpu, 0);

    push_byte(cpu, '\0');
    for (int i = strlen(env) - 1; i >= 0; --i)
        push_byte(cpu, env[i]);
    env_head = *get_register_16(cpu, SP);

    for (int i = argc - 1; i >= 0; --i) {
        push_byte(cpu, '\0');
        for (int j = strlen(argv[i]) - 1; j >= 0; --j)
            push_byte(cpu, argv[i][j]);
        addr[i] = *get_register_16(cpu, SP);
    }

    push_word(cpu, 0);
    push_word(cpu, env_head);

    push_word(cpu, 0);

    for (int i = argc - 1; i >= 0; --i)
        push_word(cpu, addr[i]);
    
    push_word(cpu, argc);

    free(addr);
}

// Frees the CPU
void free_cpu(CPU* cpu) {
    free(cpu->header);
    free(cpu->text);
    free(cpu->data);
    free(cpu);
}

// Gets the byte at the given address
uint8_t get_byte(CPU* cpu, uint16_t addr) {
    return cpu->text[addr];
}

// Reads the byte pointed by the program counter
uint8_t read_byte(CPU* cpu) {
    return get_byte(cpu, cpu->pc++);
}

// Gets the word at the given address
uint16_t get_word(CPU* cpu, uint16_t addr) {
    return *(uint16_t*)(cpu->text + addr);
}

// Reads the word pointed by the program counter
uint16_t read_word(CPU* cpu) {
    uint16_t val = get_word(cpu, cpu->pc);
    cpu->pc += 2;
    return val;
}

// Gets the register pointer of the given register
uint16_t* get_register_16(CPU* cpu, enum CPU_REGISTERS_16 reg) {
    return (uint16_t*)(cpu->registers + reg);
}

// Sets the register value of the given register
void set_register_16(CPU* cpu, enum CPU_REGISTERS_16 reg, uint16_t val) {
    *get_register_16(cpu, reg) = val;
}

// Gets the register pointer of the given register  (8-bit)
uint8_t* get_register_8(CPU* cpu, enum CPU_REGISTERS_8 reg) {
    if (reg < 4)
        return (uint8_t*)(cpu->registers + reg);

    return (uint8_t*)(cpu->registers + reg - 4) + 1;
}

// Sets the register value of the given register (8-bit)
void set_register_8(CPU* cpu, enum CPU_REGISTERS_8 reg, uint8_t val) {
    *get_register_8(cpu, reg) = val;
}

// Gets the flag value of the given flag
uint8_t get_flag(CPU* cpu, enum CPU_FLAGS flag) {
    return !!(cpu->flags & flag);
}

// Sets the flag value of the given flag
void set_flag(CPU* cpu, enum CPU_FLAGS flag, uint8_t val) {
    if (val) {
        cpu->flags |= flag;
    } else {
        cpu->flags &= ~flag;
    }
}

// Gets the parity of a 16-bit number
uint8_t get_parity(uint16_t n) {
    uint8_t parity = 0;
    while (n) {
        parity ^= n & 1;
        n >>= 1;
    }
    return parity;
}

// Add two 16-bit numbers
uint16_t add16(CPU* cpu, uint16_t a, uint16_t b) {
    uint32_t result = a + b;
    uint16_t res16 = result & 0xFFFF;
    
    set_flag(cpu, CF, !!(result - res16));
    set_flag(cpu, OF, (a >> 15 && b >> 15 && !(res16 >> 15)) || (!(a >> 15) && !(b >> 15) && res16 >> 15));
    set_flag(cpu, ZF, res16 == 0);
    set_flag(cpu, SF, res16 >> 15);
    set_flag(cpu, PF, get_parity(res16));

    return res16;
}

// Add two 8-bit numbers
uint8_t add8(CPU* cpu, uint8_t a, uint8_t b) {
    uint16_t result = a + b;
    uint8_t res8 = result & 0xFF;
    
    set_flag(cpu, CF, !!(result - res8));
    set_flag(cpu, OF, (a >> 7 && b >> 7 && !(res8 >> 7)) || (!(a >> 7) && !(b >> 7) && res8 >> 7));
    set_flag(cpu, ZF, res8 == 0);
    set_flag(cpu, SF, res8 >> 7);
    set_flag(cpu, PF, get_parity(res8));
    
    return res8;
}

// Subtract two 16-bit numbers
uint16_t sub16(CPU* cpu, uint16_t a, uint16_t b) {
    uint32_t result = a - b;
    uint16_t res16 = result & 0xFFFF;
    
    set_flag(cpu, CF, !!(result - res16));
    set_flag(cpu, OF, !!(a >> 15 && !(b >> 15) && !(res16 >> 15)) || (!(a >> 15) && b >> 15 && res16 >> 15));
    set_flag(cpu, ZF, res16 == 0);
    set_flag(cpu, SF, res16 >> 15);
    set_flag(cpu, PF, get_parity(res16));
    
    return res16;
}

// Subtract two 8-bit numbers
uint8_t sub8(CPU* cpu, uint8_t a, uint8_t b) {
    uint16_t result = a - b;
    uint8_t res8 = result & 0xFF;
    
    set_flag(cpu, CF, !!(result - res8));
    set_flag(cpu, OF, !!(a >> 7 && !(b >> 7) && !(res8 >> 7)) || (!(a >> 7) && b >> 7 && res8 >> 7));
    set_flag(cpu, ZF, res8 == 0);
    set_flag(cpu, SF, res8 >> 7);
    set_flag(cpu, PF, get_parity(res8));
    
    return res8;
}

// Or two 16-bit numbers
uint16_t or16(CPU* cpu, uint16_t a, uint16_t b) {
    uint16_t result = a | b;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 15);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

// And two 16-bit numbers
uint16_t and16(CPU* cpu, uint16_t a, uint16_t b) {
    uint16_t result = a & b;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 15);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

// And two 8-bit numbers
uint8_t and8(CPU* cpu, uint8_t a, uint8_t b) {
    uint8_t result = a & b;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 7);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

// Or two 8-bit numbers
uint8_t or8(CPU* cpu, uint8_t a, uint8_t b) {
    uint8_t result = a | b;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 7);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

// Xor two 16-bit numbers
uint16_t xor16(CPU* cpu, uint16_t a, uint16_t b) {
    uint16_t result = a ^ b;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 15);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

// Xor two 8-bit numbers
uint8_t xor8(CPU* cpu, uint8_t a, uint8_t b) {
    uint8_t result = a ^ b;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 7);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

// Negate a 16-bit number
uint16_t neg16(CPU* cpu, uint16_t a) {
    uint16_t result = -a;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, a != 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 15);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

// Negate a 8-bit number
uint8_t neg8(CPU* cpu, uint8_t a) {
    uint8_t result = -a;
    set_flag(cpu, OF, 0);
    set_flag(cpu, CF, a != 0);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 8);
    set_flag(cpu, PF, get_parity(result));
    return result;
}

#define CF_SL16(a, count) ((count) ? ((a << (count - 1)) >> 15) & 1 : 0)
#define CF_SL8(a, count) ((count) ? ((a << (count - 1)) >> 7) & 1 : 0)

// Shifts a 16-bit number left
uint16_t shl16(CPU* cpu, uint16_t a, uint8_t count) {
    uint16_t result = a << count;
    set_flag(cpu, CF, CF_SL16(a, count));
    set_flag(cpu, OF, ((result >> 15) != get_flag(cpu, CF)));
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 15);
    set_flag(cpu, PF, get_parity(result));


    return result;
}

// Shifts a 8-bit number left
uint8_t shl8(CPU* cpu, uint8_t a, uint8_t count) {
    uint8_t result = a << count;
    set_flag(cpu, CF, CF_SL8(a, count));
    set_flag(cpu, OF, (result >> 7 != get_flag(cpu, CF)));
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 7);
    set_flag(cpu, PF, get_parity(result));

    return result;
}

#define CF_SR(a, count) ((count) ? (a >> (count - 1)) & 1 : 0)

// Shifts a 16-bit number right
uint16_t shr16(CPU* cpu, uint16_t a, uint8_t count) {
    uint16_t result = a >> count;
    set_flag(cpu, CF, CF_SR(a, count));
    set_flag(cpu, OF, result >> 15);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 15);
    set_flag(cpu, PF, get_parity(result));

    return result;
}

// Shifts a 8-bit number right
uint8_t shr8(CPU* cpu, uint8_t a, uint8_t count) {
    uint8_t result = a >> count;
    set_flag(cpu, CF, CF_SR(a, count));
    set_flag(cpu, OF, result >> 7);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 7);
    set_flag(cpu, PF, get_parity(result));

    return result;
}

// Shifts a signed 16-bit number right
int16_t sar16(CPU* cpu, int16_t a, uint8_t count) {
    int16_t result = a >> count;
    set_flag(cpu, CF, CF_SR(a, count));
    set_flag(cpu, OF, result >> 15);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 15);
    set_flag(cpu, PF, get_parity(result));

    return result;
}

// Shifts a signed 8-bit number right
int8_t sar8(CPU* cpu, int8_t a, uint8_t count) {
    uint8_t result = a >> count;
    set_flag(cpu, CF, CF_SR(a, count));
    set_flag(cpu, OF, result >> 7);
    set_flag(cpu, ZF, result == 0);
    set_flag(cpu, SF, result >> 7);
    set_flag(cpu, PF, get_parity(result));

    return result;
}

// Push a 16-bit value to the stack
void push_word(CPU* cpu, uint16_t val) {
    set_register_16(cpu, SP, *get_register_16(cpu, SP) - 2);
    *(uint16_t*)(cpu->data + *get_register_16(cpu, SP)) = val;
}

// Push a 8-bit value to the stack
void push_byte(CPU* cpu, uint8_t val) {
    set_register_16(cpu, SP, *get_register_16(cpu, SP) - 1);
    *(uint8_t*)(cpu->data + *get_register_16(cpu, SP)) = val;
}

// Pop a 16-bit value from the stack
void pop_word(CPU* cpu, uint16_t* reg) {
    *reg = *(uint16_t*)(cpu->data + *get_register_16(cpu, SP));
    set_register_16(cpu, SP, *get_register_16(cpu, SP) + 2);
}

// Pop a 8-bit value from the stack
void pop_byte(CPU* cpu, uint8_t* reg) {
    *reg = *(uint8_t*)(cpu->data + *get_register_16(cpu, SP));
    set_register_16(cpu, SP, *get_register_16(cpu, SP) + 1);
}

int16_t cpu_brk(CPU* cpu, uint16_t address) {
    if (address < cpu->header->a_data) return -1;
    if (address >= ((*get_register_16(cpu, SP) & ~0x3ff) - 0x400)) return -1;

    cpu->p_brk = address;
    return 0;
}