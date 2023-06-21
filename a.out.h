#ifndef _AOUT_H
#define _AOUT_H

#include <stdint.h>

#define A_HEADER_SIZE sizeof(A_HEADER)

typedef struct exec {           /* a.out header */
    unsigned char a_magic[2];   /* magic number */
    unsigned char a_flags;      /* flags, see below */
    unsigned char a_cpu;        /* cpu id */
    unsigned char a_hdrlen;     /* length of header */
    unsigned char a_unused;     /* reserved for future use */
    unsigned short a_version;   /* version stamp (not used at present) */
    int32_t          a_text;       /* size of text segment in bytes */
    int32_t          a_data;       /* size of data segment in bytes */
    int32_t          a_bss;        /* size of bss segment in bytes */
    int32_t          a_entry;      /* entry point */
    int32_t          a_total;      /* total memory allocated */
    int32_t          a_sysms;      /* size of symbol table */

    /* SHORT FORM ENDS HERE */
    int32_t          a_trsize;     /* text relocation size */
    int32_t          a_drsize;     /* data relocation size */
    int32_t          a_tbase;      /* text relocation base */
    int32_t          a_dbase;      /* data relocation base */
} A_HEADER;

#endif
