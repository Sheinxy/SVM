#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "cpu.h"
#include "message.h"
#include <stdint.h>
#include <unistd.h>

extern int16_t (*syscall_table[78])(CPU* cpu, uint8_t* m);

int16_t syscall_undef(CPU* cpu, uint8_t* m);
int16_t syscall_exit(CPU* cpu, uint8_t* m);
int16_t syscall_read(CPU* cpu, uint8_t* m);
int16_t syscall_write(CPU* cpu, uint8_t* m);
int16_t syscall_open(CPU* cpu, uint8_t* m);
int16_t syscall_close(CPU* cpu, uint8_t* m);
int16_t syscall_brk(CPU* cpu, uint8_t* m);
int16_t syscall_lseek(CPU* cpu, uint8_t* m);
int16_t syscall_ioctl(CPU* cpu, uint8_t* m);

void syscalls(CPU* cpu);

#endif