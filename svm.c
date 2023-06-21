#include "a.out.h"
#include "cpu.h"
#include "op.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-m|-r|-d] <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* file_name = (argv[1][0] != '-') ? argv[1] : argv[2];
    char* running_mode = (argv[1][0] != '-') ? "-r" : argv[1];
    
    FILE* f = fopen(file_name, "rb");
    if (!f) {
        fprintf(stderr, "Error: can't open file %s\n", file_name);
        return EXIT_FAILURE;
    }

    CPU* cpu = get_cpu(f);
    init_stack_pointer(cpu, (argv[1][0] != '-') ? argc - 1 : argc - 2, (argv[1][0] != '-') ? argv + 1: argv + 2);
    fclose(f);

    if (strcmp(running_mode, "-m") == 0) {
        cpu->running_mode = DEBUGGING;
        fprintf(stderr, " AX   BX   CX   DX   SP   BP   SI   DI  FLAGS IP\n");
    } else if (strcmp(running_mode, "-r") == 0) {
        cpu->running_mode = RUNNING;
    } else if (strcmp(running_mode, "-d") == 0) {
        cpu->running_mode = DISASSEMBLING;
    } else {
        fprintf(stderr, "Usage: %s [-m|-r|-d] <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    while (cpu->pc < cpu->header->a_text) {
        cpu->op_start = cpu->pc;

        if (cpu->pc == cpu->header->a_text - 1 && get_byte(cpu, cpu->pc) == 0x00) {
            cpu->pc++;
            op_undef(cpu);
            break;
        }

        op_codes[read_byte(cpu)](cpu);
        cpu->state = NORMAL;
    }

    free_cpu(cpu);
    return EXIT_SUCCESS;
}