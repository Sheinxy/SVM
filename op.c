#include "op.h"

#include <stdio.h>
#include <stdlib.h>

#include "a.out.h"
#include "cpu.h"
#include "tools.h"
#include "syscalls.h"

// The opcode table 0xXY -> Line X, Column Y
void (*op_codes[256])(CPU*) = {
        // 0            1            2            3            4            5            6            7            8            9            A            B            C            D            E            F
/* 0 */    &op_00_add , &op_01_add , &op_02_add , &op_03_add , &op_04_add , &op_05_add , &op_xx_push, &op_xx_pop , &op_08_or  , &op_09_or  , &op_0A_or  , &op_0B_or  , &op_0C_or  , &op_0D_or  , &op_xx_push, &op_xx_pop ,
/* 1 */    &op_10_adc , &op_11_adc , &op_12_adc , &op_13_adc , &op_14_adc , &op_15_adc , &op_xx_push, &op_xx_pop , &op_18_sbb , &op_19_sbb , &op_1A_sbb , &op_1B_sbb , &op_1C_sbb , &op_1D_sbb , &op_xx_push, &op_xx_pop ,
/* 2 */    &op_20_and , &op_21_and , &op_22_and , &op_23_and , &op_24_and , &op_25_and , &op_undef  , &op_27_baa , &op_28_sub , &op_29_sub , &op_2A_sub , &op_2B_sub , &op_2C_sub , &op_2D_sub , &op_undef  , &op_2F_das ,
/* 3 */    &op_30_xor , &op_31_xor , &op_32_xor , &op_33_xor , &op_34_xor , &op_35_xor , &op_undef  , &op_37_aaa , &op_38_cmp , &op_39_cmp , &op_3A_cmp , &op_3B_cmp , &op_3C_cmp , &op_3D_cmp , &op_undef  , &op_3F_aas ,
/* 4 */    &op_4b_inc , &op_4b_inc , &op_4b_inc , &op_4b_inc , &op_4b_inc , &op_4b_inc , &op_4b_inc , &op_4b_inc , &op_4w_dec , &op_4w_dec , &op_4w_dec , &op_4w_dec , &op_4w_dec , &op_4w_dec , &op_4w_dec , &op_4w_dec ,
/* 5 */    &op_5b_push, &op_5b_push, &op_5b_push, &op_5b_push, &op_5b_push, &op_5b_push, &op_5b_push, &op_5b_push, &op_5w_pop , &op_5w_pop , &op_5w_pop , &op_5w_pop , &op_5w_pop , &op_5w_pop , &op_5w_pop , &op_5w_pop ,
/* 6 */    &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  ,
/* 7 */    &op_70_jo  , &op_71_jno , &op_72_jb  , &op_73_jnb , &op_74_je  , &op_75_jne , &op_76_jbe , &op_77_jnbe, &op_78_js  , &op_79_jns , &op_7A_jp  , &op_7B_jnp , &op_7C_jl  , &op_7D_jnl , &op_7E_jle , &op_7F_jnle,
/* 8 */    &op_80     , &op_81     , &op_82     , &op_83     , &op_84_test, &op_85_test, &op_86_xchg, &op_87_xchg, &op_88_mov , &op_89_mov , &op_8A_mov , &op_8B_mov , &op_8C_mov , &op_8D_lea , &op_8E_mov , &op_8F     ,
/* 9 */    &op_90_nop , &op_9b_xchg, &op_9b_xchg, &op_9b_xchg, &op_9b_xchg, &op_9b_xchg, &op_9b_xchg, &op_9b_xchg, &op_98_cbw , &op_99_cwd , &op_undef  , &op_9B_wait,&op_9C_pushf, &op_9D_popf, &op_9E_sahf, &op_9F_lahf,
/* A */    &op_A0_mov , &op_A1_mov , &op_A2_mov , &op_A3_mov , &op_A4_movs, &op_A5_movs, &op_A6_cmps, &op_A7_cmps, &op_A8_test, &op_A9_test, &op_AA_stos, &op_AB_stos, &op_AC_lods, &op_AD_lods, &op_AE_scas, &op_AF_scas,
/* B */    &op_Bb_mov , &op_Bb_mov , &op_Bb_mov , &op_Bb_mov , &op_Bb_mov , &op_Bb_mov , &op_Bb_mov , &op_Bb_mov , &op_Bw_mov , &op_Bw_mov , &op_Bw_mov , &op_Bw_mov , &op_Bw_mov , &op_Bw_mov , &op_Bw_mov , &op_Bw_mov ,
/* C */    &op_undef  , &op_undef  , &op_C2_ret , &op_C3_ret , &op_C4_les , &op_C5_lds , &op_C6_mov , &op_C7_mov , &op_undef  , &op_undef  , &op_undef  , &op_CB_retf, &op_CC_int3, &op_CD_int , &op_CE_into, &op_CF_iret,
/* D */    &op_D0     , &op_D1     , &op_D2     , &op_D3     , &op_D4_aam , &op_D5_aad , &op_undef  , &op_D7_xlat, &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  , &op_undef  ,
/* E */    &op_E0_loop, &op_E1_loop, &op_E2_loop, &op_E3_jcxz, &op_E4_in  , &op_E5_in  , &op_E6_out , &op_E7_out , &op_E8_call, &op_E9_jmp , &op_EA_jmpf, &op_EB_jmp , &op_EC_in  , &op_ED_in  , &op_EE_out , &op_EF_out ,
/* F */    &op_undef  , &op_undef  , &op_F2_rep , &op_F3_rep , &op_F4_hlt , &op_F5_cmc , &op_F6     , &op_F7     , &op_F8_clc , &op_F9_stc , &op_FA_cli , &op_FB_sti , &op_FC_cld , &op_FD_std , &op_FE     , &op_FF
};

// Undefined opcode
void op_undef(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "(undefined)", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "(undefined)", 0, NULL);
            return;
        }
    }
}

#pragma region 0x00

// Add Register/Memory from Register on byte
void op_00_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "add", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }
}

// Add Register/Memory from Register on word
void op_01_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "add", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }

    *ea = add16(cpu, *ea, *get_register_16(cpu, reg));
}

// Add Register/Memory to Register on byte
void op_02_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "add", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }
}

// Add Register/Memory to Register on word
void op_03_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "add", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, reg, add16(cpu, *get_register_16(cpu, reg), *ea));
}

// Add Immediate to Accumulator on byte
void op_04_add(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "add", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }
}

// Add Immediate to Accumulator on word
void op_05_add(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "add", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, AX, add16(cpu, *get_register_16(cpu, 0), imm));
}

// Or Register/Memory from Register on byte
void op_08_or(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "or", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "or", 2, arg_str);
            return;
        }
    }
}

// Or Register/Memory from Register on word
void op_09_or(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "or", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "or", 2, arg_str);
            return;
        }
    }

    *ea = or16(cpu, *ea, *get_register_16(cpu, reg));
}

// Or Register/Memory to Register on byte
void op_0A_or(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "or", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "or", 2, arg_str);
            return;
        }
    }
}

// Or Register/Memory to Register on word
void op_0B_or(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "or", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "or", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, reg, or16(cpu, *get_register_16(cpu, reg), *ea));
}

// Or Immediate to Accumulator on byte
void op_0C_or(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "or", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "or", 2, arg_str);
            return;
        }
    }
}

// Or Immediate to Accumulator on word
void op_0D_or(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "or", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "or", 2, arg_str);
            return;
        }
    }
}

#pragma endregion

#pragma region 0x10

// Add with Carry Register/Memory from Register on byte
void op_10_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "adc", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }
}

// Add with Carry Register/Memory from Register on word
void op_11_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "adc", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }

    *ea = add16(cpu, *ea, *get_register_16(cpu, reg) + get_flag(cpu, CF));
}

// Add with Carry Register/Memory to Register on byte
void op_12_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "adc", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }
}

// Add with Carry Register/Memory to Register on word
void op_13_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "adc", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }

    *ea = add16(cpu, *ea, *get_register_16(cpu, reg) + get_flag(cpu, CF));
}

// Add with Carry Immediate to Accumulator on byte
void op_14_adc(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "adc", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }
}

// Add with Carry Immediate to Accumulator on byte
void op_15_adc(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "adc", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }
}

// Sun with Borrow Register/Memory from Register on byte
void op_18_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sbb", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }
}

// Sub with Carry Register/Memory from Register on word
void op_19_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sbb", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }
}

// Sub with Borrow Register/Memory to Register on byte
void op_1A_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sbb", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }
}

// Sub with Borrow Register/Memory to Register on word
void op_1B_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "sbb", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, reg, sub16(cpu, *get_register_16(cpu, reg), *ea + get_flag(cpu, CF)));
}

// Sub with Borrow Immediate from Accumulator on byte
void op_1C_sbb(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sbb", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }
}

// Sub with Borrow Immediate from Accumulator on word
void op_1D_sbb(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sbb", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }
}

#pragma endregion

#pragma region Push and Pop

// Push Segment Register
void op_xx_push(CPU* cpu) {
    uint8_t reg = (get_byte(cpu, cpu->pc - 1) >> 3) & 0b11;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_seg_str(reg)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "push", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "push", 1, arg_str);
            return;
        }
    }
}

// Pop Segment Register
void op_xx_pop(CPU* cpu) {
    uint8_t reg = (get_byte(cpu, cpu->pc - 1) >> 3) & 0b11;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_seg_str(reg)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "pop", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "pop", 1, arg_str);
            return;
        }
    }
}

#pragma endregion

#pragma region 0x20

// And Register/Memory from Register on byte
void op_20_and(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "and", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "and", 2, arg_str);
            return;
        }
    }
}

// And Register/Memory from Register on word
void op_21_and(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "and", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "and", 2, arg_str);
            return;
        }
    }
}

// And Register/Memory to Register on byte
void op_22_and(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "and", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "and", 2, arg_str);
            return;
        }
    }
}

// And Register/Memory to Register on word
void op_23_and(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "and", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "and", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, reg, and16(cpu, *get_register_16(cpu, reg), *ea));
}

// And Immediate to Accumulator on byte
void op_24_and(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "and", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "and", 2, arg_str);
            return;
        }
    }
}

// And Immediate to Accumulator on word
void op_25_and(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "and", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "and", 2, arg_str);
            return;
        }
    }
}

// Decimal Adjust for Add
void op_27_baa(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "baa", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "baa", 0, NULL);
            return;
        }
    }
}

// Sub Register/Memory from Register on byte
void op_28_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sub", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }
}

// Sub Register/Memory from Register on word
void op_29_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "sub", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }
    
    *ea = sub16(cpu, *ea, *get_register_16(cpu, reg));
}

// Sub Register/Memory to Register on byte
void op_2A_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sub", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }
}

// Sub Register/Memory to Register on word
void op_2B_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "sub", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, reg, sub16(cpu, *get_register_16(cpu, reg), *ea));
}

// Sub Immediate to Accumulator on byte
void op_2C_sub(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sub", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }
}

// Sub Immediate to Accumulator on word
void op_2D_sub(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sub", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, AX, sub16(cpu, *get_register_16(cpu, AX), imm));
}

// Decimal Adjust for Subtract
void op_2F_das(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "das", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "das", 0, NULL);
            return;
        }
    }
}

#pragma endregion

#pragma region 0x30

// Xor Register/Memory from Register on byte
void op_30_xor(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint8_t* ea = get_ea(cpu, mod, rm, disp, 0);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            uint8_t* add_info[2] = {NULL, ea};
            print_debugging(cpu, "xor", 2, arg_str, (uint16_t**)add_info);
        } else {
            print_disassembling(cpu, "xor", 2, arg_str);
            return;
        }
    }

    *ea = xor8(cpu, *ea, *get_register_8(cpu, reg));
}

// Xor Register/Memory from Register on word
void op_31_xor(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "xor", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "xor", 2, arg_str);
            return;
        }
    }

    *ea = xor16(cpu, *ea, *get_register_16(cpu, reg));
}

// Xor Register/Memory to Register on byte
void op_32_xor(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xor", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "xor", 2, arg_str);
            return;
        }
    }
}

// Xor Register/Memory to Register on word
void op_33_xor(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xor", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "xor", 2, arg_str);
            return;
        }
    }
}

// Xor Immediate to Accumulator on byte
void op_34_xor(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xor", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "xor", 2, arg_str);
            return;
        }
    }
}

// Xor Immediate to Accumulator on word
void op_35_xor(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xor", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "xor", 2, arg_str);
            return;
        }
    }
}

// ASCII Adjust for Add
void op_37_aaa(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "aaa", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "aaa", 0, NULL);
            return;
        }
    }
}

// Compare Register/Memory to Register on byte
void op_38_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cmp", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }
}

// Compare Register/Memory to Register on word
void op_39_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "cmp", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }

    sub16(cpu, *ea, *get_register_16(cpu, reg));
}

// Compare Register to Register/Memory on byte
void op_3A_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cmp", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }
}

// Compare Register to Register/Memory on word
void op_3B_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "cmp", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }

    sub16(cpu, *get_register_16(cpu, reg), *ea);
}

// Compare Immediate with Accumulator on byte
void op_3C_cmp(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cmp", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }
}

// Compare Immediate with Accumulator on word
void op_3D_cmp(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cmp", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }

    sub16(cpu, *get_register_16(cpu, AX), imm);
}

// ASCII Adjust for Subtract
void op_3F_aas(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "aas", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "aas", 0, NULL);
            return;
        }
    }
}

#pragma endregion

#pragma region 0x40

// Increment Register
void op_4b_inc(CPU* cpu) {
    uint8_t reg = get_byte(cpu, cpu->pc - 1) & 0b111;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "inc", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "inc", 1, arg_str);
            return;
        }
    }

    uint8_t cf = get_flag(cpu, CF);
    set_register_16(cpu, reg, add16(cpu, *get_register_16(cpu, reg), 1));
    set_flag(cpu, CF, cf);
}

// Decrement Register
void op_4w_dec(CPU* cpu) {
    uint8_t reg = get_byte(cpu, cpu->pc - 1) & 0b111;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "dec", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "dec", 1, arg_str);
            return;
        }
    }

    uint8_t cf = get_flag(cpu, CF);
    set_register_16(cpu, reg, sub16(cpu, *get_register_16(cpu, reg), 1));
    set_flag(cpu, CF, cf);
}

#pragma endregion

#pragma region 0x50

// Push Register
void op_5b_push(CPU* cpu) {
    uint8_t reg = get_byte(cpu, cpu->pc - 1) & 0b111;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "push", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "push", 1, arg_str);
            return;
        }
    }

    push_word(cpu, *get_register_16(cpu, reg));
}

// Pop Register
void op_5w_pop(CPU* cpu) {
    uint8_t reg = get_byte(cpu, cpu->pc - 1) & 0b111;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "pop", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "pop", 1, arg_str);
            return;
        }
    }
    
    pop_word(cpu, get_register_16(cpu, reg));
}

#pragma endregion

#pragma region 0x60

#pragma endregion

#pragma region 0x70

// Jump on Overflow
void op_70_jo(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);
    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jo", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jo", 1, arg_str);
            return;
        }
    }
}

// Jump on Not Overflow
void op_71_jno(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);
    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jno", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jno", 1, arg_str);
            return;
        }
    }
}

// Jump on Below
void op_72_jb(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);
    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jb", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jb", 1, arg_str);
            return;
        }
    }

    if (get_flag(cpu, CF))
        cpu->pc = target;
}

// Jump on Not Below
void op_73_jnb(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);
    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jnb", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jnb", 1, arg_str);
            return;
        }
    }

    if (!get_flag(cpu, CF))
        cpu->pc = target;
}

// Jump on Equal
void op_74_je(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "je", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "je", 1, arg_str);
            return;
        }
    }

    if (get_flag(cpu, ZF))
        cpu->pc = target;
}

// Jump on Not Equal
void op_75_jne(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jne", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jne", 1, arg_str);
            return;
        }
    }

    if (!get_flag(cpu, ZF))
        cpu->pc = target;
}

// Jump on Below or Equal
void op_76_jbe(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jbe", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jbe", 1, arg_str);
            return;
        }
    }

    if (get_flag(cpu, ZF) || get_flag(cpu, CF))
        cpu->pc = target;
}

// Jump on Not Below or Equal
void op_77_jnbe(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jnbe", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jnbe", 1, arg_str);
            return;
        }
    }

    if (!get_flag(cpu, ZF) && !get_flag(cpu, CF))
        cpu->pc = target;
}

// Jump on Sign
void op_78_js(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "js", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "js", 1, arg_str);
            return;
        }
    }
}

// Jump on Not Sign
void op_79_jns(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jns", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jns", 1, arg_str);
            return;
        }
    }
}

// Jump on Parity
void op_7A_jp(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jp", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jp", 1, arg_str);
            return;
        }
    }
}

// Jump on Not Parity
void op_7B_jnp(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jnp", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jnp", 1, arg_str);
            return;
        }
    }
}

// Jump on Less
void op_7C_jl(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jl", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jl", 1, arg_str);
            return;
        }
    }

    if (get_flag(cpu, SF) != get_flag(cpu, OF))
        cpu->pc = target;
}

// Jump on Not Less
void op_7D_jnl(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jnl", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jnl", 1, arg_str);
            return;
        }
    }

    if (get_flag(cpu, SF) == get_flag(cpu, OF))
        cpu->pc = target;
}

// Jump on Less or Equal
void op_7E_jle(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jle", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jle", 1, arg_str);
            return;
        }
    }

    if (get_flag(cpu, SF) != get_flag(cpu, OF) || get_flag(cpu, ZF))
        cpu->pc = target;
}

// Jump on Not Less or Equal
void op_7F_jnle(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jnle", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jnle", 1, arg_str);
            return;
        }
    }

    if (get_flag(cpu, SF) == get_flag(cpu, OF) && !get_flag(cpu, ZF))
        cpu->pc = target;
}

#pragma endregion

#pragma region 0x80

// Add Immediate to Register/Memory on byte
void op_80_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "add byte" : "add";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Or Immediate to Register/Memory on byte
void op_80_or(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "or byte" : "or";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Add with Carry Immediate to Register/Memory on byte
void op_80_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "adc byte" : "adc";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Sub with Borrow Immediate to Register/Memory on byte
void op_80_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "sbb byte" : "sbb";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// And Immediate to Register/Memory on byte
void op_80_and(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "and byte" : "and";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Sub Immediate to Register/Memory on byte
void op_80_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "sub byte" : "sub";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Xor Immediate to Register/Memory on byte
void op_80_xor(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "xor byte" : "xor";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Compare immediate to Register/Memory on byte
void op_80_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint8_t* ea = get_ea(cpu, mod, rm, disp, 0);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "cmp byte" : "cmp";
        if (cpu->running_mode == DEBUGGING) {
            uint8_t* add_info[2] = {NULL, ea};
            print_debugging(cpu, op_str, 2, arg_str, (uint16_t**)add_info);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }

    sub8(cpu, *ea, imm);
}

// Arithmetic Immediate to Register/Memory on byte
void op_80(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_80_add, &op_80_or,  &op_80_adc, &op_80_sbb,
                                 &op_80_and, &op_80_sub, &op_80_xor, &op_80_cmp};
    (ops[op])(cpu);
}

// Add Immediate to Register/Memory on word
void op_81_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "add", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }

    *ea = add16(cpu, *ea, imm);
}

// Or Immediate to Register/Memory on word
void op_81_or(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "or", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "or", 2, arg_str);
            return;
        }
    }

    *ea = or16(cpu, *ea, imm);
}

// Add with Carry Immediate to Register/Memory on word
void op_81_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "adc", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }
}

// Sub with Borrow Immediate to Register/Memory on word
void op_81_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sbb", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }
}

// And Immediate to Register/Memory on word
void op_81_and(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        uint16_t* add_info[2] = {ea, ea};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "and", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "and", 2, arg_str);
            return;
        }
    }

    *ea = and16(cpu, *ea, imm);
}

// Sub Immediate to Register/Memory on word
void op_81_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea}; 
            print_debugging(cpu, "sub", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }

    *ea = sub16(cpu, *ea, imm);
}

// Xor Immediate to Register/Memory on word
void op_81_xor(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xor", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "xor", 2, arg_str);
            return;
        }
    }
}

// Compare Immediate to Register/Memory on word
void op_81_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "cmp", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }

    sub16(cpu, *ea, imm);
}

// Arithmetic Immediate to Register/Memory on word
void op_81(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_81_add, &op_81_or,  &op_81_adc, &op_81_sbb,
                                 &op_81_and, &op_81_sub, &op_81_xor, &op_81_cmp};
    (ops[op])(cpu);
}

// Add Signed Immediate to Register/Memory on byte
void op_82_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%02hx", (uint8_t)imm);
        const char* op_str = (mod != 0b11) ? "add byte" : "add";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Add with Carry Signed Immediate to Register/Memory on byte
void op_82_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%02hx", (uint8_t)imm);
        const char* op_str = (mod != 0b11) ? "adc byte" : "adc";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Add with Carry Signed Immediate to Register/Memory on byte
void op_82_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%02hx", (uint8_t)imm);
        const char* op_str = (mod != 0b11) ? "sbb byte" : "sbb";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Sub Signed Immediate to Register/Memory on byte
void op_82_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%02hx", (uint8_t)imm);
        const char *op_str = (mod != 0b11) ? "sub byte" : "sub";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Compare Signed Immediate to Register/Memory on byte
void op_82_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%02hx", (uint8_t)imm);
        const char* op_str = (mod != 0b11) ? "cmp byte" : "cmp";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu,op_str, 2, arg_str);
            return;
        }
    }
}

// Arithmetic Signed Immediate to Register/Memory on byte
void op_82(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_82_add, &op_undef,  &op_82_adc, &op_82_sbb,
                                 &op_undef,  &op_82_sub, &op_undef,  &op_82_cmp};
    (ops[op])(cpu);
}

// Add Signed Immediate to Register/Memory on word
void op_83_add(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    int16_t imm = (int8_t)read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(6, sizeof(char))};
        if (imm < 0)
            sprintf(arg_str[1], "-%02hx", (uint16_t)abs(imm));
        else
            sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "add", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "add", 2, arg_str);
            return;
        }
    }

    *ea = add16(cpu, *ea, imm);
}

// Add with Carry Signed Immediate to Register/Memory on word
void op_83_adc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int16_t imm = (int8_t)read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(6, sizeof(char))};
        if (imm < 0)
            sprintf(arg_str[1], "-%02hx", (uint16_t)abs(imm));
        else
            sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "adc", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "adc", 2, arg_str);
            return;
        }
    }
}

// Sub with Borrow Signed Immediate to Register/Memory on word
void op_83_sbb(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int16_t imm = (int8_t)read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(6, sizeof(char))};
        if (imm < 0)
            sprintf(arg_str[1], "-%02hx", (uint16_t)abs(imm));
        else
            sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sbb", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sbb", 2, arg_str);
            return;
        }
    }
}

// Sub Signed Immediate to Register/Memory on word
void op_83_sub(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    int16_t imm = (int8_t)read_byte(cpu);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(6, sizeof(char))};
        if (imm < 0)
            sprintf(arg_str[1], "-%hx", (uint16_t)abs(imm));
        else
            sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "sub", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "sub", 2, arg_str);
            return;
        }
    }

    *ea = sub16(cpu, *ea, imm);
}

// Compare Signed Immediate to Register/Memory on word
void op_83_cmp(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    int16_t imm = (int8_t)read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(6, sizeof(char))};
        if (imm < 0)
            sprintf(arg_str[1], "-%hx", (uint16_t)abs(imm));
        else
            sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "cmp", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "cmp", 2, arg_str);
            return;
        }
    }

    sub16(cpu, *ea, imm);
}

// Arithmetic Signed Immediate to Register/Memory on word
void op_83(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_83_add, &op_undef,  &op_83_adc, &op_83_sbb,
                                 &op_undef,  &op_83_sub, &op_undef,  &op_83_cmp};
    (ops[op])(cpu);
}

// Test Register/Memory and Register on byte
void op_84_test(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "test", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "test", 2, arg_str);
            return;
        }
    }
}

// Test Register/Memory and Register on word
void op_85_test(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "test", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "test", 2, arg_str);
            return;
        }
    }

    and16(cpu, *ea, *get_register_16(cpu, reg));
}

// Exchange Register/Memory with Register on byte
void op_86_xchg(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xchg", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "xchg", 2, arg_str);
            return;
        }
    }
}

// Exchange Register/Memory with Register on word
void op_87_xchg(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "xchg", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "xchg", 2, arg_str);
            return;
        }
    }
    uint16_t tmp = *ea;
    *ea = *get_register_16(cpu, reg); 
    set_register_16(cpu, reg, tmp);

}

// Move Register/Memory from Register on byte
void op_88_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint8_t* ea = get_ea(cpu, mod, rm, disp, 0);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0), get_reg_str(reg, 0)};
        if (cpu->running_mode == DEBUGGING) {
            uint8_t* add_info[2] = {NULL, ea};
            print_debugging(cpu, "mov", 2, arg_str, (uint16_t**)add_info);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    *ea = *get_register_8(cpu, reg);
}

// Move Register/Memory from Register on word
void op_89_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_reg_str(reg, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "mov", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    *ea = *get_register_16(cpu, reg);
}

// Move Register/Memory to Register on byte
void op_8A_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint8_t *ea = get_ea(cpu, mod, rm, disp, 0);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {NULL, (uint16_t*)ea};
            print_debugging(cpu, "mov", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    set_register_8(cpu, reg, *ea);
}

// Move Register/Memory to Register on word
void op_8B_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "mov", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, reg, *ea);
}

// Move Segment Register to Register/Memory
void op_8C_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1), get_seg_str(reg)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mov", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }
}

// Load EA to Register
void op_8D_lea(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "lea", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "lea", 2, arg_str);
            return;
        }
    }

    if (mod == 0b11)
        set_register_16(cpu, reg, rm);
    else
        set_register_16(cpu, reg, (size_t)ea - (size_t)cpu->data);
}

// Move Register/Memory to Segment Register
void op_8E_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_seg_str(reg), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mov", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }
}

void op_8F_pop(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "pop", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "pop", 1, arg_str);
            return;
        }
    }
}

void op_8F(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_8F_pop, &op_undef, &op_undef, &op_undef,
                                 &op_undef,  &op_undef, &op_undef, &op_undef};
    (ops[op])(cpu);
}

#pragma endregion

#pragma region 0x90

// No Operation
void op_90_nop(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "nop", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "nop", 0, NULL);
            return;
        }
    }
}

// Exchange Register with Accumulator
void op_9b_xchg(CPU* cpu) {
    uint8_t reg = get_byte(cpu, cpu->pc - 1) & 0b111;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_reg_str(0, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xchg", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "xchg", 2, arg_str);
            return;
        }
    }
    uint16_t tmp = *get_register_16(cpu, AX);
    set_register_16(cpu, AX, *get_register_16(cpu, reg));
    set_register_16(cpu, reg, tmp);
}

// Convert Byte to Word
void op_98_cbw(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cbw", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "cbw", 0, NULL);
            return;
        }
    }

    int16_t al = *(int8_t*)get_register_8(cpu, AL);
    set_register_16(cpu, AX, al);
}

// Convert Word to Double Word
void op_99_cwd(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cwd", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "cwd", 0, NULL);
            return;
        }
    }

    set_register_16(cpu, DX, (*get_register_16(cpu, AX) >> 15) ? 0xFFFF : 0x0000);
}

// Wait
void op_9B_wait(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "wait", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "wait", 0, NULL);
            return;
        }
    }
}

// Push Flags
void op_9C_pushf(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "pushf", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "pushf", 0, NULL);
            return;
        }
    }
}

// Pop Flags
void op_9D_popf(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "popf", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "popf", 0, NULL);
            return;
        }
    }
}

// Store AH into Flags
void op_9E_sahf(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sahf", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "sahf", 0, NULL);
            return;
        }
    }
}

// Load AH with Flags
void op_9F_lahf(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "lahf", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "lahf", 0, NULL);
            return;
        }
    }
}

#pragma endregion

#pragma region 0xA0

// Move Memory to Accumulator on byte
void op_A0_mov(CPU* cpu) {
    uint16_t addr = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "[%04hx]", addr);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mov", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }
}

// Move Memory to Accumulator on word
void op_A1_mov(CPU* cpu) {
    uint16_t addr = read_word(cpu);
    uint16_t* ea = (uint16_t*)(cpu->data + addr);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "[%04hx]", addr);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "mov", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, AX, *ea);
}

// Move Memory from Accumulator on byte
void op_A2_mov(CPU* cpu) {
    uint16_t addr = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char)), get_reg_str(0, 0)};
        sprintf(arg_str[0], "[%04hx]", addr);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mov", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }
}

// Move Memory from Accumulator on word
void op_A3_mov(CPU* cpu) {
    uint16_t addr = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char)), get_reg_str(0, 1)};
        sprintf(arg_str[0], "[%04hx]", addr);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mov", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }
}

// Move Byte
void op_A4_movs(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "movsb", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "movsb", 0, NULL);
            return;
        }
    }

    if (!*get_register_16(cpu, CX))return;
    do {
        uint8_t* src = get_memory_with_seg(cpu, DS, *get_register_16(cpu, SI));
        uint8_t* dst = get_memory_with_seg(cpu, ES, *get_register_16(cpu, DI));
        *dst = *src;


        if (!get_flag(cpu, DF)) {
            set_register_16(cpu, SI, *get_register_16(cpu, SI) + 1);
            set_register_16(cpu, DI, *get_register_16(cpu, DI) + 1);
        }
        else {
            set_register_16(cpu, SI, *get_register_16(cpu, SI) - 1);
            set_register_16(cpu, DI, *get_register_16(cpu, DI) - 1);
        }
        if (cpu->state == NORMAL) break;

        set_register_16(cpu, CX, *get_register_16(cpu, CX) - 1);
    }   while (*get_register_16(cpu, CX));
}

// Move Word
void op_A5_movs(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "movsw", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "movsw", 0, NULL);
            return;
        }
    }
}

// Compare Byte
void op_A6_cmps(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cmpsb", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "cmpsb", 0, NULL);
            return;
        }
    }

    if (!*get_register_16(cpu, CX))return;
    do {
        uint8_t a = *get_memory_with_seg(cpu, DS, *get_register_16(cpu, SI));
        uint8_t b = *get_memory_with_seg(cpu, ES, *get_register_16(cpu, DI));
        sub8(cpu, a, b);

        if (!get_flag(cpu, DF)) {
            set_register_16(cpu, SI, *get_register_16(cpu, SI) + 1);
            set_register_16(cpu, DI, *get_register_16(cpu, DI) + 1);
        }
        else {
            set_register_16(cpu, SI, *get_register_16(cpu, SI) - 1);
            set_register_16(cpu, DI, *get_register_16(cpu, DI) - 1);
        }
        if (cpu->state == NORMAL) break;

        set_register_16(cpu, CX, *get_register_16(cpu, CX) - 1);
        if (*get_register_16(cpu, CX) == 0) break;
    }   while (cpu->state == REPEATNE && !get_flag(cpu, ZF) || cpu->state == REPEAT && get_flag(cpu, ZF));
}

// Compare Word
void op_A7_cmps(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cmpsw", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "cmpsw", 0, NULL);
            return;
        }
    }
}

// Test Immediate Data and Accumulator on byte
void op_A8_test(CPU* cpu) {
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hhx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "test", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "test", 2, arg_str);
            return;
        }
    }
}

// Test Immediate Data and Accumulator on word
void op_A9_test(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "test", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "test", 2, arg_str);
            return;
        }
    }
}

// Store Byte to AL
void op_AA_stos(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "stosb", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "stosb", 0, NULL);
            return;
        }
    }

    if (!*get_register_16(cpu, CX))return;
    do {
        *get_memory_with_seg(cpu, ES, *get_register_16(cpu, DI)) = *get_register_8(cpu, AL);

        if (!get_flag(cpu, DF)) set_register_16(cpu, DI, *get_register_16(cpu, DI) + 1);
        else set_register_16(cpu, DI, *get_register_16(cpu, DI) - 1);
        if (cpu->state == NORMAL) break;

        set_register_16(cpu, CX, *get_register_16(cpu, CX) - 1);
    }   while (*get_register_16(cpu, CX));
}

// Store Word to AX
void op_AB_stos(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "stosw", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "stosw", 0, NULL);
            return;
        }
    }
}

// Load Byte to AL
void op_AC_lods(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "lodsb", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "lodsb", 0, NULL);
            return;
        }
    }
}

// Load Word to AX
void op_AD_lods(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "lodsw", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "lodsw", 0, NULL);
            return;
        }
    }
}

// Scan Byte
void op_AE_scas(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "scasb", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "scasb", 0, NULL);
            return;
        }
    }

    if (!*get_register_16(cpu, CX))return;
    do {
        sub8(cpu, *get_register_8(cpu, AL), *get_memory_with_seg(cpu, ES, *get_register_16(cpu, DI)));

        if (!get_flag(cpu, DF)) set_register_16(cpu, DI, *get_register_16(cpu, DI) + 1);
        else set_register_16(cpu, DI, *get_register_16(cpu, DI) - 1);
        if (cpu->state == NORMAL) break;

        set_register_16(cpu, CX, *get_register_16(cpu, CX) - 1);
        if (*get_register_16(cpu, CX) == 0) break;
    }   while (cpu->state == REPEATNE && !get_flag(cpu, ZF) || cpu->state == REPEAT && get_flag(cpu, ZF));
}

// Scan Byte Word
void op_AF_scas(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "scasw", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "scasw", 0, NULL);
            return;
        }
    }
}

#pragma endregion

#pragma region 0xB0

// Move Immediate to Register on byte
void op_Bb_mov(CPU* cpu) {
    uint8_t reg = get_byte(cpu, cpu->pc - 1) & 0b111;
    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 0), (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mov", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    set_register_8(cpu, reg, imm);
}

// Move Immediate to Register on word
void op_Bw_mov(CPU* cpu) {
    uint8_t reg = get_byte(cpu, cpu->pc - 1) & 0b111;
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mov", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    set_register_16(cpu, reg, imm);
}

#pragma endregion

#pragma region 0xC0

// Return from CALL within Segment adding Immediate to SP
void op_C2_ret(CPU* cpu) {
    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "ret", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "ret", 1, arg_str);
            return;
        }
    }

    pop_word(cpu, &cpu->pc);
    set_register_16(cpu, SP, *get_register_16(cpu, SP) + imm);
}

// Return from CALL within Segment
void op_C3_ret(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "ret", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "ret", 0, NULL);
            return;
        }
    }

    pop_word(cpu, &cpu->pc);
}

// Load Pointer to ES
void op_C4_les(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "les", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "les", 2, arg_str);
            return;
        }
    }
}

// Load Pointer to DS
void op_C5_lds(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(reg, 1), get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "lds", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "lds", 2, arg_str);
            return;
        }
    }
}

// Move Immediate to Register/Memory on byte
void op_C6_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint8_t* ea = get_ea(cpu, mod, rm, disp, 0);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "mov byte" : "mov";
        if (cpu->running_mode == DEBUGGING) {
            uint8_t* add_info[2] = {NULL, ea};
            print_debugging(cpu, op_str, 2, arg_str, (uint16_t**)add_info);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }

    *ea = imm;
}

// Move Immediate to Register/Memory on word
void op_C7_mov(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "mov", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "mov", 2, arg_str);
            return;
        }
    }

    *ea = imm;
}

// Return from CALL Intersegment
void op_CB_retf(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "retf", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "retf", 0, NULL);
            return;
        }
    }
}

// Interrupt Type 3
void op_CC_int3(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "int3", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "int3", 0, NULL);
            return;
        }
    }
}

// Interrupt Type Specified
void op_CD_int(CPU* cpu) {
    uint8_t type = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(3, sizeof(char))};
        sprintf(arg_str[0], "%02hx", type);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "int", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "int", 1, arg_str);
            return;
        }
    }

    syscalls(cpu);
}

// Interrupt on Overflow
void op_CE_into(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "into", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "into", 0, NULL);
            return;
        }
    }
}

// Interrupt Return
void op_CF_iret(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "iret", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "iret", 0, NULL);
            return;
        }
    }
}

#pragma endregion

#pragma region 0xD0

// Rotate Left on byte
void op_D0_rol(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        const char* op_str = (mod != 0b11) ? "rol byte" : "rol";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Rotate Right on byte
void op_D0_ror(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        const char* op_str = (mod != 0b11) ? "ror byte" : "ror";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Rotate Through Carry Flag Left on byte
void op_D0_rcl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        const char* op_str = (mod != 0b11) ? "rcl byte" : "rcl";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Rotate Through Carry Right on byte
void op_D0_rcr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        const char* op_str = (mod != 0b11) ? "rcr byte" : "rcr";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Shift Logical/Arithmetic Left on byte
void op_D0_shl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "shl", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "shl", 2, arg_str);
            return;
        }
    }
}

// Shift Logical Right on byte
void op_D0_shr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "shr", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "shr", 2, arg_str);
            return;
        }
    }
}

// Shift Arithmetic Right on byte
void op_D0_sar(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sar", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sar", 2, arg_str);
            return;
        }
    }
}

// Shift/Rotate on byte
void op_D0(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_D0_rol, &op_D0_ror, &op_D0_rcl, &op_D0_rcr,
                                 &op_D0_shl, &op_D0_shr, &op_undef,  &op_D0_sar};
    (ops[op])(cpu);
}

// Rotate Left on word
void op_D1_rol(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "rol", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "rol", 2, arg_str);
            return;
        }
    }
}

// Rotate Right on word
void op_D1_ror(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "ror", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "ror", 2, arg_str);
            return;
        }
    }
}

// Rotate Thourgh Carry Flag Left on word
void op_D1_rcl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "rcl", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "rcl", 2, arg_str);
            return;
        }
    }
}

// Rotate Through Carry Right on word
void op_D1_rcr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "rcr", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "rcr", 2, arg_str);
            return;
        }
    }
}

// Shift Logical/Arithmetic Left on word
void op_D1_shl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "shl", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "shl", 2, arg_str);
            return;
        }
    }

    *ea = shl16(cpu, *ea, 1);
}

// Shift Logical Right on word
void op_D1_shr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "shr", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "shr", 2, arg_str);
            return;
        }
    }
}

// Shift Arithmetic Right on word
void op_D1_sar(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "1");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sar", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "sar", 2, arg_str);
            return;
        }
    }
}

// Shift/Rotate on word
void op_D1(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_D1_rol, &op_D1_ror, &op_D1_rcl, &op_D1_rcr,
                                 &op_D1_shl, &op_D1_shr, &op_undef,  &op_D1_sar};
    (ops[op])(cpu);
}

// Rotate Left on byte (CL)
void op_D2_rol(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        const char* op_str = (mod != 0b11) ? "rol byte" : "rol";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Rotate Right on byte (CL)
void op_D2_ror(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        const char* op_str = (mod != 0b11) ? "ror byte" : "ror";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Rotate Thourgh Carry Flag Left on byte (CL)
void op_D2_rcl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        const char* op_str = (mod != 0b11) ? "rcl byte" : "rcl";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Rotate Through Carry Right on byte (CL)
void op_D2_rcr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        const char* op_str = (mod != 0b11) ? "rcr byte" : "rcr";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }
}

// Shift Logical/Arithmetic Left on byte (CL)
void op_D2_shl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "shl", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "shl", 2, arg_str);
            return;
        }
    }
}

// Shift Logical Right on byte (CL)
void op_D2_shr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "shr", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "shr", 2, arg_str);
            return;
        }
    }
}

// Shift/Rotate on byte (CL)
void op_D2(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_D2_rol, &op_D2_ror, &op_D2_rcl, &op_D2_rcr,
                                 &op_undef,  &op_D2_shl, &op_D2_shr, &op_undef};
    (ops[op])(cpu);
}

// Rotate Left on word (CL)
void op_D3_rol(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "rol", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "rol", 2, arg_str);
            return;
        }
    }
}

// Rotate right on word (CL)
void op_D3_ror(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "ror", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "ror", 2, arg_str);
            return;
        }
    }
}

// Rotate through Carry Flag Left on word (CL)
void op_D3_rcl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "rcl", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "rcl", 2, arg_str);
            return;
        }
    }
}

// Rotate through Carry Right on word (CL)
void op_D3_rcr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "rcr", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "rcr", 2, arg_str);
            return;
        }
    }
}

// Shift Logical/Arithmetic Left on word (CL)
void op_D3_shl(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "shl", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "shl", 2, arg_str);
            return;
        }
    }

    *ea = shl16(cpu, *ea, *get_register_8(cpu, CL) & 0x1f);
}

// Shift Logical Right on word (CL)
void op_D3_shr(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);


    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "shr", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "shr", 2, arg_str);
            return;
        }
    }

    *ea = shr16(cpu, *ea, *get_register_8(cpu, CL) & 0x1f);
}

// Shift Arithmetic Right on word (CL)
void op_D3_sar(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[1], "cl");
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "sar", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "sar", 2, arg_str);
            return;
        }
    }

    uint8_t of = get_flag(cpu, of);
    *ea = sar16(cpu, *ea, *get_register_8(cpu, CL) & 0x1f);
    set_flag(cpu, OF, of);
}

// Shift/Rotate on word (CL)
void op_D3(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_D3_rol, &op_D3_ror, &op_D3_rcl, &op_D3_rcr,
                                 &op_D3_shl, &op_D3_shr, &op_undef, &op_D3_sar};
    (ops[op])(cpu);
}

// ASCII Adjust for Multiply
void op_D4_aam(CPU* cpu) {
    uint8_t imm = read_byte(cpu);
    if (imm != 0b00001010) {
        op_undef(cpu);
        return;
    }

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(3, sizeof(char)),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[0], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "aam", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "aam", 2, arg_str);
            return;
        }
    }
}

// ASCII Adjust for Divide
void op_D5_aad(CPU* cpu) {
    uint8_t imm = read_byte(cpu);
    if (imm != 0b00001010) {
        op_undef(cpu);
        return;
    }

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(3, sizeof(char)),
                             (char*)calloc(3, sizeof(char))};
        sprintf(arg_str[0], "%hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "aad", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "aad", 2, arg_str);
            return;
        }
    }
}

// Translate Byte to AL
void op_D7_xlat(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "xlat", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "xlat", 0, NULL);
            return;
        }
    }
}

#pragma endregion

#pragma region 0xE0

// Loop While not Zero
void op_E0_loop(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "loopnz", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "loopnz", 1, arg_str);
            return;
        }
    }
}

// Loop While Zero
void op_E1_loop(CPU* cpu) {
    int16_t disp = read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "loopz", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "loopz", 1, arg_str);
            return;
        }
    }
}

// Loop CX times
void op_E2_loop(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "loop", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "loop", 1, arg_str);
            return;
        }
    }
}

// Jump on CX Zero
void op_E3_jcxz(CPU* cpu) {
    int16_t disp = (int8_t)read_byte(cpu);

    uint16_t target = (uint16_t)(cpu->pc + disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jcxz", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jcxz", 1, arg_str);
            return;
        }
    }
}

// Input from fixed port on byte
void op_E4_in(CPU* cpu) {
    uint8_t port = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%02hx", port);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "in", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "in", 2, arg_str);
            return;
        }
    }
}

// Input from fixed port on word
void op_E5_in(CPU* cpu) {
    uint8_t port = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%02hx", port);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "in", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "in", 2, arg_str);
            return;
        }
    }
}

// Output to fixed port on byte
void op_E6_out(CPU* cpu) {
    uint8_t port = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char)), get_reg_str(0, 0)};
        sprintf(arg_str[0], "%02hx", port);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "out", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "out", 2, arg_str);
            return;
        }
    }
}

// Output to fixed port on word
void op_E7_out(CPU* cpu) {
    uint8_t port = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char)), get_reg_str(0, 1)};
        sprintf(arg_str[0], "%02hx", port);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "out", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "out", 2, arg_str);
            return;
        }
    }
}

// Call Direct within Segment
void op_E8_call(CPU* cpu) {
    int16_t addr = read_word(cpu);

    uint16_t target_addr = cpu->pc + addr;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target_addr);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "call", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "call", 1, arg_str);
            return;
        }
    }

    push_word(cpu, cpu->pc);
    cpu->pc = target_addr;
}

// Unconditional Jump direct within Segment
void op_E9_jmp(CPU* cpu) {
    int16_t addr = read_word(cpu);

    uint16_t target_addr = cpu->pc + addr;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target_addr);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jmp", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jmp", 1, arg_str);
            return;
        }
    }

    cpu->pc = target_addr;
}

// Unconditional Jump Direct Intersegment
void op_EA_jmpf(CPU* cpu) {
    uint16_t seg = read_word(cpu);
    uint16_t off = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx:%04hx", off, seg);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jmpf", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jmpf", 1, arg_str);
            return;
        }
    }
}

// Unconditional Jump Direct withing Segment-short
void op_EB_jmp(CPU* cpu) {
    int8_t addr = read_byte(cpu);

    uint16_t target_addr = cpu->pc + addr;

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char))};
        sprintf(arg_str[0], "%04hx", target_addr);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jmp short", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jmp short", 1, arg_str);
            return;
        }
    }

    cpu->pc = target_addr;
}

// Input from variable port on byte
void op_EC_in(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 0), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "dx");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "in", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "in", 2, arg_str);
            return;
        }
    }
}

// Input from variable port on word
void op_ED_in(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_reg_str(0, 1), (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "dx");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "in", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "in", 2, arg_str);
            return;
        }
    }
}

// Output to variable port on byte
void op_EE_out(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char)), get_reg_str(0, 0)};
        sprintf(arg_str[0], "dx");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "out", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "out", 2, arg_str);
            return;
        }
    }
}

// Output to variable port on word
void op_EF_out(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {(char*)calloc(5, sizeof(char)), get_reg_str(0, 1)};
        sprintf(arg_str[0], "dx");
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "out", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "out", 2, arg_str);
            return;
        }
    }
}

#pragma endregion

#pragma region 0xF0

// Repeat ne
void op_F2_rep(CPU* cpu) {
    while (cpu->pc < cpu->header->a_text && get_byte(cpu, cpu->pc) == 0xF2) {
        cpu->op_start = cpu->pc;
        read_byte(cpu);
    }

    if (cpu->pc == cpu->header->a_text) {
        op_undef(cpu);
        return;
    }

    cpu->state = REPEATNE;
    op_codes[read_byte(cpu)](cpu);
}

// Repeat
void op_F3_rep(CPU* cpu) {
    while (cpu->pc < cpu->header->a_text && get_byte(cpu, cpu->pc) == 0xF3) {
        cpu->op_start = cpu->pc;
        read_byte(cpu);
    }

    if (cpu->pc == cpu->header->a_text) {
        op_undef(cpu);
        return;
    }

    cpu->state = REPEAT;
    op_codes[read_byte(cpu)](cpu);
}

// Halt
void op_F4_hlt(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "hlt", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "hlt", 0, NULL);
            return;
        }
    }
}

// Complement Carry
void op_F5_cmc(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cmc", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "cmc", 0, NULL);
            return;
        }
    }
}

// Test Immediate to Register/Memory on byte
void op_F6_test(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 0);

    uint8_t imm = read_byte(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%hx", imm);
        const char* op_str = (mod != 0b11) ? "test byte" : "test";
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {NULL, ea};
            print_debugging(cpu, op_str, 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, op_str, 2, arg_str);
            return;
        }
    }

    and8(cpu, *ea, imm);
}

// Invert on byte
void op_F6_not(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%02hx", reg);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "not", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "not", 2, arg_str);
            return;
        }
    }
}

// Change sign on byte
void op_F6_neg(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "neg", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "neg", 1, arg_str);
            return;
        }
    }
}

// Multiply (unsigned) on byte
void op_F6_mul(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "mul", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "mul", 1, arg_str);
            return;
        }
    }
}

// Integer Multiply (signed) on byte
void op_F6_imul(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "imul", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "imul", 1, arg_str);
            return;
        }
    }
}

// Divide (unsigned) on byte
void op_F6_div(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "div", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "div", 1, arg_str);
            return;
        }
    }
}

// Integer Divide (signed) on byte
void op_F6_idiv(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "idiv", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "idiv", 1, arg_str);
            return;
        }
    }
}

void op_F6(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_F6_test, &op_undef,  &op_F6_not,
                                 &op_F6_neg,  &op_F6_mul, &op_F6_imul,
                                 &op_F6_div,  &op_F6_idiv};
    (ops[op])(cpu);
}

// Test Immediate to Register/Memory on word
void op_F7_test(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    uint16_t imm = read_word(cpu);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%04hx", imm);
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "test", 2, arg_str, add_info);
        } else {
            print_disassembling(cpu, "test", 2, arg_str);
            return;
        }
    }

    and16(cpu, *ea, imm);
}

// Invert on word
void op_F7_not(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1),
                             (char*)calloc(5, sizeof(char))};
        sprintf(arg_str[1], "%02hx", reg);
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "not", 2, arg_str, NULL);
        } else {
            print_disassembling(cpu, "not", 2, arg_str);
            return;
        }
    }
}

// Change sign on word
void op_F7_neg(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "neg", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "neg", 1, arg_str);
            return;
        }
    }

    *ea = neg16(cpu, *ea);
}

// Multiply (unsigned) on word
void op_F7_mul(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "mul", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "mul", 1, arg_str);
            return;
        }
    }

    uint32_t res = *ea * *get_register_16(cpu, AX);
    set_register_16(cpu, DX, res >> 16);
    set_register_16(cpu, AX, res & 0xFFFF);

    set_flag(cpu, CF, (res >> 16) != 0);
    set_flag(cpu, OF, (res >> 16) != 0);
}

// Integer Multiply (signed) on word
void op_F7_imul(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "imul", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "imul", 1, arg_str);
            return;
        }
    }
}

// Divide (unsigned) on word
void op_F7_div(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "div", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "div", 1, arg_str);
            return;
        }
    }

    uint32_t a = *get_register_16(cpu, DX) << 16 | *get_register_16(cpu, AX);
    uint16_t b = *ea;
    set_register_16(cpu, AX, a / b);
    set_register_16(cpu, DX, a % b);
}

// Integer Divide (signed) on word
void op_F7_idiv(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "idiv", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "idiv", 1, arg_str);
            return;
        }
    }
}

void op_F7(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_F7_test, &op_undef,  &op_F7_not,
                                 &op_F7_neg,  &op_F7_mul, &op_F7_imul,
                                 &op_F7_div,  &op_F7_idiv};
    (ops[op])(cpu);
}

// Clear Carry
void op_F8_clc(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "clc", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "clc", 0, NULL);
            return;
        }
    }
}

// Set Carry
void op_F9_stc(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "stc", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "stc", 0, NULL);
            return;
        }
    }
}

// Clear Interrupt
void op_FA_cli(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cli", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "cli", 0, NULL);
            return;
        }
    }
}

// Set Interrupt
void op_FB_sti(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "sti", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "sti", 0, NULL);
            return;
        }
    }
}

// Clear Direction
void op_FC_cld(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "cld", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "cld", 0, NULL);
            return;
        }
    }

    set_flag(cpu, DF, 0);
}

// Set Direction
void op_FD_std(CPU* cpu) {
    if (cpu->running_mode != RUNNING) {
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "std", 0, NULL, NULL);
        } else {
            print_disassembling(cpu, "std", 0, NULL);
            return;
        }
    }
}

// Increment Register/Memory on byte
void op_FE_inc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0)};
        const char* op_str = (mod != 0b11) ? "inc byte" : "inc";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 1, arg_str);
            return;
        }
    }
}

// Decrement Register/Memory on byte
void op_FE_dec(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 0)};
        const char* op_str = (mod != 0b11) ? "dec byte" : "dec";
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, op_str, 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, op_str, 1, arg_str);
            return;
        }
    }
}

void op_FE(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_FE_inc, &op_FE_dec, &op_undef, &op_undef,
                                 &op_undef,  &op_undef,  &op_undef, &op_undef};
    (ops[op])(cpu);
}

// Push Register/Memory
void op_FF_push(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "push", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "push", 1, arg_str);
            return;
        }
    }

    push_word(cpu, *ea);
}

// Increment Register/Memory on word
void op_FF_inc(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "inc", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "inc", 1, arg_str);
            return;
        }
    }

    uint8_t cf = get_flag(cpu, CF);
    *ea = add16(cpu, *ea, 1);
    set_flag(cpu, CF, cf);
}

// Increment Register/Memory on word
void op_FF_dec(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "dec", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "dec", 1, arg_str);
            return;
        }
    }

    uint8_t cf = get_flag(cpu, CF);
    *ea = sub16(cpu, *ea, 1);
    set_flag(cpu, CF, cf);
}

// Call Indirect within Segment
void op_FF_call_seg(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "call", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "call", 1, arg_str);
            return;
        }
    }

    push_word(cpu, cpu->pc);
    cpu->pc = *ea;
}

// Call Indirect Intersegment
void op_FF_call_int(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "call", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "call", 1, arg_str);
            return;
        }
    }
}

// Unconditional Jump Indirect within Segment
void op_FF_jmp_seg(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);
    uint16_t* ea = (uint16_t*)get_ea(cpu, mod, rm, disp, 1);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            uint16_t* add_info[2] = {ea, ea};
            print_debugging(cpu, "jmp", 1, arg_str, add_info);
        } else {
            print_disassembling(cpu, "jmp", 1, arg_str);
            return;
        }
    }

    cpu->pc = *ea;
}

// Unconditional Jump Indirect Intersegment
void op_FF_jmp_int(CPU* cpu) {
    uint8_t mod, reg, rm;
    int16_t disp;

    get_mod_reg_rm_disp(cpu, &mod, &reg, &rm, &disp);

    if (cpu->running_mode != RUNNING) {
        char* arg_str[] = {get_rm_str(mod, rm, disp, 1)};
        if (cpu->running_mode == DEBUGGING) {
            print_debugging(cpu, "jmp", 1, arg_str, NULL);
        } else {
            print_disassembling(cpu, "jmp", 1, arg_str);
            return;
        }
    }
}

void op_FF(CPU* cpu) {
    uint8_t op = (get_byte(cpu, cpu->pc) >> 3) & 0b111;

    void (*ops[8])(CPU * cpu) = {&op_FF_inc,      &op_FF_dec,     &op_FF_call_seg,
                                 &op_FF_call_int, &op_FF_jmp_seg, &op_FF_jmp_int,
                                 &op_FF_push,     &op_undef};
    (ops[op])(cpu);
}

#pragma endregion