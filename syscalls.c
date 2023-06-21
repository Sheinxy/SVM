#include "cpu.h"
#include "message.h"
#include "syscalls.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

int16_t (*syscall_table[78])(CPU* cpu, uint8_t* m) = {
    &syscall_undef, &syscall_exit , &syscall_undef, &syscall_read , &syscall_write, &syscall_open , &syscall_close, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef,
    &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_brk  , &syscall_undef, &syscall_lseek, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef,
    &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef,
    &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef,
    &syscall_undef, &syscall_undef, &syscall_ioctl, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef,
    &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef, &syscall_undef
};

// Undefined syscall
int16_t syscall_undef(CPU* cpu, uint8_t* m) {
    if (cpu->running_mode == DEBUGGING) 
        fprintf(stderr, "<Undefined syscall>\n");

    return 0;
}

// Exit syscall
int16_t syscall_exit(CPU* cpu, uint8_t* m) {
    int16_t status = M1_I1(m);

    if (cpu->running_mode == DEBUGGING) 
        fprintf(stderr, "<exit(%hd)>\n", status);

    exit(status);

    return 0;
}

// Read Syscall
int16_t syscall_read(CPU* cpu, uint8_t* m) {
    int16_t fd = M1_I1(m);
    int16_t nbytes = M1_I2(m);
    size_t buffer = (size_t)(M1_P1(m));


    uint8_t* buf = buffer + cpu->data;

    if (cpu->running_mode == DEBUGGING) 
        fprintf(stderr, "<read(%hd, 0x%04zx, %hd)", fd, buffer, nbytes);
    ssize_t res = read(fd, buf, nbytes);
    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, " => %zd>\n", res);

    set_register_16(cpu, AX, 0);

    return res;
}

// Write Syscall
int16_t syscall_write(CPU* cpu, uint8_t* m) {
    int16_t fd = M1_I1(m);
    int16_t nbytes = M1_I2(m);
    size_t buffer = (size_t)(M1_P1(m));


    uint8_t* buf = buffer + cpu->data;

    if (cpu->running_mode == DEBUGGING) 
        fprintf(stderr, "<write(%hd, 0x%04zx, %hd)", fd, buffer, nbytes);
    ssize_t res = write(fd, buf, nbytes);
    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, " => %zd>\n", res);

    set_register_16(cpu, AX, 0);

    return res;
}

// Open Syscall
int16_t syscall_open(CPU* cpu, uint8_t* m) {
    uint16_t len = M1_I1(m);
    uint16_t flags = M1_I2(m);
    uint16_t addr, mode_t;
    if (flags & 00100) {
        mode_t = M1_I3(m);
        addr = M1_P1(m);
    } else {
        mode_t = 0;
        addr = (len <= 14) ? (*get_register_16(cpu, BX) + 10) : M1_I3(m);
    }

    char* path = (char*)malloc(len + 1);
    memcpy(path, addr + cpu->data, len);

    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, "<open(\"%s\", %hd)", path, flags);
    int res = open(path, flags);
    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, " => %d>\n", res);

    set_register_16(cpu, AX, 0);

    return res;
}

// Close Syscall
int16_t syscall_close(CPU* cpu, uint8_t* m) {
    uint16_t fd = M1_I1(m);

    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, "<close(%hd)", fd);
    int res = close(fd);
    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, " => %d>\n", res);

    set_register_16(cpu, AX, 0);

    return res;
}


// Break Syscall
int16_t syscall_brk(CPU* cpu, uint8_t* m) {
    int16_t brk = M1_P1(m);

    if (cpu->running_mode == DEBUGGING) 
        fprintf(stderr, "<brk(0x%04x) => ", brk);

    set_register_16(cpu, AX, 0);

    if (cpu_brk(cpu, brk) == -1) {
        errno = ENOMEM;
        if (cpu->running_mode == DEBUGGING) 
            fprintf(stderr, "ENOMEM>\n");
        return -1;
    }
    if (cpu->running_mode == DEBUGGING) 
        fprintf(stderr, "0>\n");

    M2_P1(m) = brk;

    return 0;
}

// Lseek Syscall
int16_t syscall_lseek(CPU* cpu, uint8_t* m) {
    int16_t fd = M2_I1(m);
    int32_t offset = M2_L1(m);
    int16_t whence = M2_I2(m);

    if (cpu->running_mode == DEBUGGING) 
        fprintf(stderr, "<lseek(%hd, %d, %hd)", fd, offset, whence);
    int res = lseek(fd, offset, whence);
    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, " => %d>\n", res);

    set_register_16(cpu, AX, 0);

    if (res != 1) {
        M2_L1(m) = res;
        return 0;
    }

    return res;
}

// IOCTL syscall
int16_t syscall_ioctl(CPU* cpu, uint8_t* m) {
    int16_t fd = M2_I1(m);
    int16_t request = M2_I3(m);
    uint16_t address = M2_P1(m);

    if (cpu->running_mode == DEBUGGING)
        fprintf(stderr, "<ioctl(%hd, 0x%04hx, 0x%04hx)>\n", fd, request, address);

    set_register_16(cpu, AX, 0);

    errno = EINVAL;
    return -1;
}

int16_t get_res(uint16_t type, int16_t res) {
    switch (type) {
        case 3:
        case 4:
        case 19:
        case 54:
            return (res == -1) ? -errno : res;
        default:
            return res;
    }
}

// Retrieves the message pointer from the data and execute the correct syscall
void syscalls(CPU* cpu) {
    uint8_t* m = (cpu->data + *get_register_16(cpu, BX));
    int16_t res = syscall_table[M_TYPE(m)](cpu, m);

    M_TYPE(m) = get_res(M_TYPE(m), res);
}
