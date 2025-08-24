#include "x64e.h"

namespace utility {

    [[msvc::noinline]] void x64e_init_core(x64e_core* core, bool allow_read, bool allow_write) {

        memset(core, 0, sizeof(x64e_core));

        std::construct_at(&core->m_icache);

        core->m_allow_read = allow_read;
        core->m_allow_write = allow_write;
    }

    [[msvc::noinline]] void x64e_prepare_core(x64e_core* core, void* entry, x64e_read_callback_t read_callback, x64e_write_callback_t write_callback, x64e_debug_callback_t debug_callback) {

        core->m_entry = entry;

        core->m_read_callback = read_callback;
        core->m_write_callback = write_callback;
        core->m_debug_callback = debug_callback;

        core->m_rip.m_val.m_qword.m_u = reinterpret_cast<uint64_t>(core->m_entry);

        core->m_rsp.m_val.m_qword.m_u = reinterpret_cast<uint64_t>(&core->m_stack[0]) + ((reinterpret_cast<uint64_t>(&core->m_stack[sizeof(core->m_stack)]) - reinterpret_cast<uint64_t>(&core->m_stack[0])) / 2); // middle of stack

        core->m_gs.m_val.m_qword.m_u = reinterpret_cast<uint64_t>(&core->m_teb[0]);

        core->m_flags.m_val.m_qword.m_u = 0;
        core->m_flags.m_fl.m_r1 = 1;
        core->m_flags.m_fl.m_if = 1;
    }

    [[msvc::noinline]] void x64e_destroy_core(x64e_core* core) {

        core->m_icache.clear();

        memset(core, 0, sizeof(x64e_core));
    }

    [[msvc::noinline]] void x64e_print_registers(x64e_core* core) {

        printf("===========================================\n");

        printf("RAX: %016I64X  RBX: %016I64X  RCX: %016I64X\n",
            core->m_rax.m_val.m_qword.m_u,
            core->m_rbx.m_val.m_qword.m_u,
            core->m_rcx.m_val.m_qword.m_u);

        printf("RDX: %016I64X  RSI: %016I64X  RDI: %016I64X\n",
            core->m_rdx.m_val.m_qword.m_u,
            core->m_rsi.m_val.m_qword.m_u,
            core->m_rdi.m_val.m_qword.m_u);

        printf("RIP: %016I64X  RSP: %016I64X  RBP: %016I64X\n",
            core->m_rip.m_val.m_qword.m_u,
            core->m_rsp.m_val.m_qword.m_u,
            core->m_rbp.m_val.m_qword.m_u);

        printf("R8:  %016I64X  R9:  %016I64X  R10: %016I64X\n",
            core->m_r8.m_val.m_qword.m_u,
            core->m_r9.m_val.m_qword.m_u,
            core->m_r10.m_val.m_qword.m_u);

        printf("R11: %016I64X  R12: %016I64X  R13: %016I64X\n",
            core->m_r11.m_val.m_qword.m_u,
            core->m_r12.m_val.m_qword.m_u,
            core->m_r13.m_val.m_qword.m_u);

        printf("R14: %016I64X  R15: %016I64X\n",
            core->m_r14.m_val.m_qword.m_u,
            core->m_r15.m_val.m_qword.m_u);

        printf("EFLAGS: %08I32X CF=%llu PF=%llu AF=%llu ZF=%llu SF=%llu TF=%llu IF=%llu DF=%llu OF=%llu\n",
            core->m_flags.m_val.m_dword.m_u,
            core->m_flags.m_fl.m_cf,
            core->m_flags.m_fl.m_pf,
            core->m_flags.m_fl.m_af,
            core->m_flags.m_fl.m_zf,
            core->m_flags.m_fl.m_sf,
            core->m_flags.m_fl.m_tf,
            core->m_flags.m_fl.m_if,
            core->m_flags.m_fl.m_df,
            core->m_flags.m_fl.m_of);

        printf("===========================================\n");
    }

    [[msvc::noinline]] void x64e_do_step(x64e_core* core) {

        core->m_rip.m_val.m_qword.m_u += core->m_instruction.length;
    }

    [[msvc::noinline]] void x64e_do_push64(x64e_core* core, uint64_t value) {

        core->m_rsp.m_val.m_qword.m_u -= sizeof(decltype(value));

        *reinterpret_cast<decltype(value)*>(core->m_rsp.m_val.m_qword.m_u) = value;
    }

    [[msvc::noinline]] void x64e_do_push32(x64e_core* core, uint32_t value) {

        core->m_rsp.m_val.m_qword.m_u -= sizeof(decltype(value));

        *reinterpret_cast<decltype(value)*>(core->m_rsp.m_val.m_qword.m_u) = value;
    }

    [[msvc::noinline]] void x64e_do_push16(x64e_core* core, uint16_t value) {

        core->m_rsp.m_val.m_qword.m_u -= sizeof(decltype(value));

        *reinterpret_cast<decltype(value)*>(core->m_rsp.m_val.m_qword.m_u) = value;
    }

    [[msvc::noinline]] uint64_t x64e_do_pop64(x64e_core* core) {

        uint64_t value = *reinterpret_cast<decltype(value)*>(core->m_rsp.m_val.m_qword.m_u);

        core->m_rsp.m_val.m_qword.m_u += sizeof(decltype(value));

        return value;
    }

    [[msvc::noinline]] uint32_t x64e_do_pop32(x64e_core* core) {

        uint32_t value = *reinterpret_cast<decltype(value)*>(core->m_rsp.m_val.m_qword.m_u);

        core->m_rsp.m_val.m_qword.m_u += sizeof(decltype(value));

        return value;
    }

    [[msvc::noinline]] uint16_t x64e_do_pop16(x64e_core* core) {

        uint16_t value = *reinterpret_cast<decltype(value)*>(core->m_rsp.m_val.m_qword.m_u);

        core->m_rsp.m_val.m_qword.m_u += sizeof(decltype(value));

        return value;
    }

    [[msvc::noinline]] void x64e_do_branch(x64e_core* core, uint64_t address, bool is_call) {

        if (is_call) {

            x64e_do_push64(core, core->m_rip.m_val.m_qword.m_u + core->m_instruction.length);
        }

        core->m_rip.m_val.m_qword.m_u = address;
    }

    [[msvc::noinline]] void x64e_do_return(x64e_core* core) {

        core->m_rip.m_val.m_qword.m_u = x64e_do_pop64(core);
    }

    [[msvc::noinline]] bool x64e_do_read64(x64e_core* core, uint64_t address, x64e_val* val) {

        if (!core->m_read_callback(core, address, val, sizeof(uint64_t), false, false)) {

            printf("x64e: error reading %llX", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_do_read32(x64e_core* core, uint64_t address, x64e_val* val) {

        if (!core->m_read_callback(core, address, val, sizeof(uint32_t), false, false)) {

            printf("x64e: error reading %llX", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_do_read16(x64e_core* core, uint64_t address, x64e_val* val) {

        if (!core->m_read_callback(core, address, val, sizeof(uint16_t), false, false)) {

            printf("x64e: error reading %llX", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_do_read8(x64e_core* core, uint64_t address, x64e_val* val) {

        if (!core->m_read_callback(core, address, val, sizeof(uint8_t), false, false)) {

            printf("x64e: error reading %llX", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_do_write64(x64e_core* core, uint64_t address, x64e_val const* val) {

        if (!core->m_write_callback(core, address, val, sizeof(uint64_t), false, false)) {

            printf("x64e: error writing %llX", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_do_write32(x64e_core* core, uint64_t address, x64e_val const* val) {

        if (!core->m_write_callback(core, address, val, sizeof(uint32_t), false, false)) {

            printf("x64e: error writing %llX", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_do_write16(x64e_core* core, uint64_t address, x64e_val const* val) {

        if (!core->m_write_callback(core, address, val, sizeof(uint16_t), false, false)) {

            printf("x64e: error writing %llX\n", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_do_write8(x64e_core* core, uint64_t address, x64e_val const* val) {

        if (!core->m_write_callback(core, address, val, sizeof(uint8_t), false, false)) {

            printf("x64e: error writing %llX\n", address);

            return false;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_sign_extend(x64e_core* core, uint16_t cur_size, uint16_t new_size, x64e_val* val) {

        switch (new_size) {

            case 64:

                if (cur_size == 8) {

                    val->m_qword.m_s = static_cast<int64_t>(val->m_word.m_lo.m_s);
                }

                else if (cur_size == 16) {

                    val->m_qword.m_s = static_cast<int64_t>(val->m_word.m_s);
                }

                else if (cur_size == 32) {

                    val->m_qword.m_s = static_cast<int64_t>(val->m_dword.m_s);
                }

                break;

            case 32:

                if (cur_size == 8) {

                    val->m_dword.m_s = static_cast<int32_t>(val->m_word.m_lo.m_s);
                }

                else if (cur_size == 16) {

                    val->m_dword.m_s = static_cast<int32_t>(val->m_word.m_s);
                }

                break;


            case 16:

                if (cur_size == 8) {
                
                    val->m_word.m_s = static_cast<int16_t>(val->m_word.m_lo.m_s);
                }

                break;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_zero_extend(x64e_core* core, uint16_t cur_size, uint16_t new_size, x64e_val* val) {

        switch (new_size) {

            case 64:

                if (cur_size == 8) {

                    val->m_qword.m_u = static_cast<uint64_t>(val->m_word.m_lo.m_u);
                }

                else if (cur_size == 16) {

                    val->m_qword.m_u = static_cast<uint64_t>(val->m_word.m_u);
                }

                else if (cur_size == 32) {

                    val->m_qword.m_u = static_cast<uint64_t>(val->m_dword.m_u);
                }

                break;

            case 32:

                if (cur_size == 8) {

                    val->m_dword.m_u = static_cast<uint32_t>(val->m_word.m_lo.m_u);
                }

                else if (cur_size == 16) {

                    val->m_dword.m_u = static_cast<uint32_t>(val->m_word.m_u);
                }

                break;


            case 16:

                if (cur_size == 8) {

                    val->m_word.m_u = static_cast<uint16_t>(val->m_word.m_lo.m_u);
                }

                break;
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_is_operand_high_register(x64e_core* core, uint8_t operand) {

        if (core->m_operands[operand].type != ZYDIS_OPERAND_TYPE_REGISTER) {

            return false;
        }

        return core->m_operands[operand].reg.value == ZYDIS_REGISTER_AH
            || core->m_operands[operand].reg.value == ZYDIS_REGISTER_CH
            || core->m_operands[operand].reg.value == ZYDIS_REGISTER_DH
            || core->m_operands[operand].reg.value == ZYDIS_REGISTER_BH;
    }

    [[msvc::noinline]] bool x64e_get_memory_operand_segment_reg(x64e_core* core, uint8_t operand, x64e_reg** reg) {

        if (operand >= core->m_instruction.operand_count) {

            return false;
        }

        switch (core->m_operands[operand].mem.segment) {

            case ZYDIS_REGISTER_ES: *reg = &core->m_es; return true;
            case ZYDIS_REGISTER_CS: *reg = &core->m_cs; return true;
            case ZYDIS_REGISTER_SS: *reg = &core->m_ss; return true;
            case ZYDIS_REGISTER_DS: *reg = &core->m_ds; return true;
            case ZYDIS_REGISTER_FS: *reg = &core->m_fs; return true;
            case ZYDIS_REGISTER_GS: *reg = &core->m_gs; return true;
        }

        return false;
    }

    [[msvc::noinline]] bool x64e_get_register_operand_reg(x64e_core* core, uint8_t operand, x64e_reg** reg) {

        if (operand >= core->m_instruction.operand_count) {

            return false;
        }

        switch (core->m_operands[operand].reg.value) {
        
            case ZYDIS_REGISTER_RIP:
                *reg = &core->m_rip; return true;

            case ZYDIS_REGISTER_AL:
            case ZYDIS_REGISTER_AH:
            case ZYDIS_REGISTER_AX:
            case ZYDIS_REGISTER_EAX:
            case ZYDIS_REGISTER_RAX:
                *reg = &core->m_rax; return true;

            case ZYDIS_REGISTER_CL:
            case ZYDIS_REGISTER_CH:
            case ZYDIS_REGISTER_CX:
            case ZYDIS_REGISTER_ECX:
            case ZYDIS_REGISTER_RCX:
                *reg = &core->m_rcx; return true;

            case ZYDIS_REGISTER_DL:
            case ZYDIS_REGISTER_DH:
            case ZYDIS_REGISTER_DX:
            case ZYDIS_REGISTER_EDX:
            case ZYDIS_REGISTER_RDX:
                *reg = &core->m_rdx; return true;

            case ZYDIS_REGISTER_BL:
            case ZYDIS_REGISTER_BH:
            case ZYDIS_REGISTER_BX:
            case ZYDIS_REGISTER_EBX:
            case ZYDIS_REGISTER_RBX:
                *reg = &core->m_rbx; return true;

            case ZYDIS_REGISTER_SPL:
            case ZYDIS_REGISTER_SP:
            case ZYDIS_REGISTER_ESP:
            case ZYDIS_REGISTER_RSP:
                *reg = &core->m_rsp; return true;

            case ZYDIS_REGISTER_BPL:
            case ZYDIS_REGISTER_BP:
            case ZYDIS_REGISTER_EBP:
            case ZYDIS_REGISTER_RBP:
                *reg = &core->m_rbp; return true;

            case ZYDIS_REGISTER_SIL:
            case ZYDIS_REGISTER_SI:
            case ZYDIS_REGISTER_ESI:
            case ZYDIS_REGISTER_RSI:
                *reg = &core->m_rsi; return true;

            case ZYDIS_REGISTER_DIL:
            case ZYDIS_REGISTER_DI:
            case ZYDIS_REGISTER_EDI:
            case ZYDIS_REGISTER_RDI:
                *reg = &core->m_rdi; return true;

            case ZYDIS_REGISTER_R8B:
            case ZYDIS_REGISTER_R8W:
            case ZYDIS_REGISTER_R8D:
            case ZYDIS_REGISTER_R8:
                *reg = &core->m_r8; return true;

            case ZYDIS_REGISTER_R9B:
            case ZYDIS_REGISTER_R9W:
            case ZYDIS_REGISTER_R9D:
            case ZYDIS_REGISTER_R9:
                *reg = &core->m_r9; return true;

            case ZYDIS_REGISTER_R10B:
            case ZYDIS_REGISTER_R10W:
            case ZYDIS_REGISTER_R10D:
            case ZYDIS_REGISTER_R10:
                *reg = &core->m_r10; return true;

            case ZYDIS_REGISTER_R11B:
            case ZYDIS_REGISTER_R11W:
            case ZYDIS_REGISTER_R11D:
            case ZYDIS_REGISTER_R11:
                *reg = &core->m_r11; return true;

            case ZYDIS_REGISTER_R12B:
            case ZYDIS_REGISTER_R12W:
            case ZYDIS_REGISTER_R12D:
            case ZYDIS_REGISTER_R12:
                *reg = &core->m_r12; return true;

            case ZYDIS_REGISTER_R13B:
            case ZYDIS_REGISTER_R13W:
            case ZYDIS_REGISTER_R13D:
            case ZYDIS_REGISTER_R13:
                *reg = &core->m_r13; return true;

            case ZYDIS_REGISTER_R14B:
            case ZYDIS_REGISTER_R14W:
            case ZYDIS_REGISTER_R14D:
            case ZYDIS_REGISTER_R14:
                *reg = &core->m_r14; return true;

            case ZYDIS_REGISTER_R15B:
            case ZYDIS_REGISTER_R15W:
            case ZYDIS_REGISTER_R15D:
            case ZYDIS_REGISTER_R15:
                *reg = &core->m_r15; return true;
        }

        return false;
    }

    [[msvc::noinline]] bool x64e_get_memory_operand_reg(x64e_core* core, uint8_t operand, bool base, x64e_reg** reg) {
    
        if (operand >= core->m_instruction.operand_count) {

            return false;
        }

        switch (base ? core->m_operands[operand].mem.base : core->m_operands[operand].mem.index) {

            case ZYDIS_REGISTER_RIP:
                *reg = &core->m_rip; return true;

            case ZYDIS_REGISTER_AL:
            case ZYDIS_REGISTER_AH:
            case ZYDIS_REGISTER_AX:
            case ZYDIS_REGISTER_EAX:
            case ZYDIS_REGISTER_RAX:
                *reg = &core->m_rax; return true;

            case ZYDIS_REGISTER_CL:
            case ZYDIS_REGISTER_CH:
            case ZYDIS_REGISTER_CX:
            case ZYDIS_REGISTER_ECX:
            case ZYDIS_REGISTER_RCX:
                *reg = &core->m_rcx; return true;

            case ZYDIS_REGISTER_DL:
            case ZYDIS_REGISTER_DH:
            case ZYDIS_REGISTER_DX:
            case ZYDIS_REGISTER_EDX:
            case ZYDIS_REGISTER_RDX:
                *reg = &core->m_rdx; return true;

            case ZYDIS_REGISTER_BL:
            case ZYDIS_REGISTER_BH:
            case ZYDIS_REGISTER_BX:
            case ZYDIS_REGISTER_EBX:
            case ZYDIS_REGISTER_RBX:
                *reg = &core->m_rbx; return true;

            case ZYDIS_REGISTER_SPL:
            case ZYDIS_REGISTER_SP:
            case ZYDIS_REGISTER_ESP:
            case ZYDIS_REGISTER_RSP:
                *reg = &core->m_rsp; return true;

            case ZYDIS_REGISTER_BPL:
            case ZYDIS_REGISTER_BP:
            case ZYDIS_REGISTER_EBP:
            case ZYDIS_REGISTER_RBP:
                *reg = &core->m_rbp; return true;

            case ZYDIS_REGISTER_SIL:
            case ZYDIS_REGISTER_SI:
            case ZYDIS_REGISTER_ESI:
            case ZYDIS_REGISTER_RSI:
                *reg = &core->m_rsi; return true;

            case ZYDIS_REGISTER_DIL:
            case ZYDIS_REGISTER_DI:
            case ZYDIS_REGISTER_EDI:
            case ZYDIS_REGISTER_RDI:
                *reg = &core->m_rdi; return true;

            case ZYDIS_REGISTER_R8B:
            case ZYDIS_REGISTER_R8W:
            case ZYDIS_REGISTER_R8D:
            case ZYDIS_REGISTER_R8:
                *reg = &core->m_r8; return true;

            case ZYDIS_REGISTER_R9B:
            case ZYDIS_REGISTER_R9W:
            case ZYDIS_REGISTER_R9D:
            case ZYDIS_REGISTER_R9:
                *reg = &core->m_r9; return true;

            case ZYDIS_REGISTER_R10B:
            case ZYDIS_REGISTER_R10W:
            case ZYDIS_REGISTER_R10D:
            case ZYDIS_REGISTER_R10:
                *reg = &core->m_r10; return true;

            case ZYDIS_REGISTER_R11B:
            case ZYDIS_REGISTER_R11W:
            case ZYDIS_REGISTER_R11D:
            case ZYDIS_REGISTER_R11:
                *reg = &core->m_r11; return true;

            case ZYDIS_REGISTER_R12B:
            case ZYDIS_REGISTER_R12W:
            case ZYDIS_REGISTER_R12D:
            case ZYDIS_REGISTER_R12:
                *reg = &core->m_r12; return true;

            case ZYDIS_REGISTER_R13B:
            case ZYDIS_REGISTER_R13W:
            case ZYDIS_REGISTER_R13D:
            case ZYDIS_REGISTER_R13:
                *reg = &core->m_r13; return true;

            case ZYDIS_REGISTER_R14B:
            case ZYDIS_REGISTER_R14W:
            case ZYDIS_REGISTER_R14D:
            case ZYDIS_REGISTER_R14:
                *reg = &core->m_r14; return true;

            case ZYDIS_REGISTER_R15B:
            case ZYDIS_REGISTER_R15W:
            case ZYDIS_REGISTER_R15D:
            case ZYDIS_REGISTER_R15:
                *reg = &core->m_r15; return true;
        }

        return false;
    }

    [[msvc::noinline]] bool x64e_get_operand_effective_address(x64e_core* core, uint8_t operand, x64e_val* val) {

        uint64_t address{};

        if (core->m_operands[operand].type != ZYDIS_OPERAND_TYPE_MEMORY) {

            return false;
        }

        uint64_t scale = static_cast<uint64_t>(core->m_operands[operand].mem.scale);
        uint64_t disp = static_cast<uint64_t>(core->m_operands[operand].mem.disp.value);

        if (core->m_operands[operand].mem.segment != ZYDIS_REGISTER_NONE) {

            x64e_reg* segment_reg{};

            if (!x64e_get_memory_operand_segment_reg(core, operand, &segment_reg)) {

                return false;
            }

            address += segment_reg->m_val.m_qword.m_u;
        }

        if (core->m_operands[operand].mem.base != ZYDIS_REGISTER_NONE) {
        
            x64e_reg* base_reg{};
        
            if (!x64e_get_memory_operand_reg(core, operand, true, &base_reg)) {

                return false;
            }
        
            switch (core->m_instruction.address_width) {

                case 64: address += static_cast<uint64_t>(base_reg->m_val.m_qword.m_u); break;
                case 32: address += static_cast<uint64_t>(base_reg->m_val.m_qword.m_u); break;
            }

            if (core->m_operands[operand].mem.base == ZYDIS_REGISTER_RIP) {

                address += core->m_instruction.length;
            }
        }

        if (core->m_operands[operand].mem.index != ZYDIS_REGISTER_NONE) {

            x64e_reg* index_reg{};

            if (!x64e_get_memory_operand_reg(core, operand, false, &index_reg)) {

                return false;
            }
        
            switch (core->m_instruction.address_width) {

                case 64: address += static_cast<uint64_t>(index_reg->m_val.m_qword.m_u) * scale; break;
                case 32: address += static_cast<uint64_t>(index_reg->m_val.m_dword.m_u) * scale; break;
            }
        }

        val->m_qword.m_u = address + disp;

        return true;
    }

    [[msvc::noinline]] bool x64e_get_operand_value(x64e_core* core, uint8_t operand, uint16_t size, x64e_val* val) {

        if (core->m_operands[operand].type == ZYDIS_OPERAND_TYPE_REGISTER) {

            x64e_reg* reg{};

            if (!x64e_get_register_operand_reg(core, operand, &reg)) {

                return false;
            }

            if (x64e_is_operand_high_register(core, operand)) {

                val->m_word.m_lo.m_u = reg->m_val.m_word.m_hi.m_u;

                return true;
            }
        
            switch (size) {

                case 64: val->m_qword.m_u = reg->m_val.m_qword.m_u; break;
                case 32: val->m_dword.m_u = reg->m_val.m_dword.m_u; break;
                case 16: val->m_word.m_u = reg->m_val.m_word.m_u; break;
                case 8: val->m_word.m_lo.m_u = reg->m_val.m_word.m_lo.m_u; break;
            }

            return true;
        }

        else if (core->m_operands[operand].type == ZYDIS_OPERAND_TYPE_MEMORY) {

            x64e_val addr{};

            if (!x64e_get_operand_effective_address(core, operand, &addr)) {

                return false;
            }

            if (core->m_operands[operand].mem.base == ZYDIS_REGISTER_RSP ||
                core->m_operands[operand].mem.base == ZYDIS_REGISTER_RBP) {

                uint64_t stack_high = reinterpret_cast<uint64_t>(&core->m_stack[0]);
                uint64_t stack_low = reinterpret_cast<uint64_t>(&core->m_stack[sizeof(core->m_stack)]);

                if ((addr.m_qword.m_u >= stack_high) && (addr.m_qword.m_u < stack_low)) {

                    if ((addr.m_qword.m_u + size) >= stack_low) {

                        return false;
                    }
                    
                    uint64_t stack_off = addr.m_qword.m_u - stack_high;

                    if (core->m_read_callback(core, addr.m_qword.m_u, val, size, true, static_cast<bool>(_bittest64(reinterpret_cast<LONG64*>(&core->m_stack_dirty[stack_off >> 6]), stack_off & 0x3F)))) {
                        
                        return true;
                    }
                    
                    switch (size) {

                        case 64: val->m_qword.m_u = *reinterpret_cast<uint64_t*>(addr.m_qword.m_u); return true;
                        case 32: val->m_dword.m_u = *reinterpret_cast<uint32_t*>(addr.m_qword.m_u); return true;
                        case 16: val->m_word.m_u = *reinterpret_cast<uint16_t*>(addr.m_qword.m_u); return true;
                        case 8: val->m_word.m_lo.m_u = *reinterpret_cast<uint8_t*>(addr.m_qword.m_u); return true;
                    }
                }

                printf("x64e: rsp/rbp bad. cannot continue.\n");

                return false;
            }

            if (core->m_operands[operand].mem.segment == ZYDIS_REGISTER_GS) {
                
                uint64_t teb_high = reinterpret_cast<uint64_t>(&core->m_teb[0]);
                uint64_t teb_low = reinterpret_cast<uint64_t>(&core->m_teb[sizeof(core->m_teb)]);

                if ((addr.m_qword.m_u >= teb_high) && (addr.m_qword.m_u < teb_low)) {

                    if ((addr.m_qword.m_u + size) >= teb_low) {

                        return false;
                    }

                    switch (size) {

                        case 64: val->m_qword.m_u = *reinterpret_cast<uint64_t*>(addr.m_qword.m_u); return true;
                        case 32: val->m_dword.m_u = *reinterpret_cast<uint32_t*>(addr.m_qword.m_u); return true;
                        case 16: val->m_word.m_u = *reinterpret_cast<uint16_t*>(addr.m_qword.m_u); return true;
                        case 8: val->m_word.m_lo.m_u = *reinterpret_cast<uint8_t*>(addr.m_qword.m_u); return true;
                    }
                }

                printf("x64e: gs bad. cannot continue.\n");

                return false;
            }

            if (!core->m_allow_read) {

                // we don't want to read anything from the process
                // we only want to allow reads from our emulated stack

                printf("x64e: read forbidden. cannot continue.\n");

                return false;
            }

            switch (size) {

                case 64: return x64e_do_read64(core, addr.m_qword.m_u, val);
                case 32: return x64e_do_read32(core, addr.m_qword.m_u, val);
                case 16: return x64e_do_read16(core, addr.m_qword.m_u, val);
                case 8: return x64e_do_read8(core, addr.m_qword.m_u, val);
            }
        }

        else if (core->m_operands[operand].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {

            val->m_qword.m_u = core->m_operands[operand].imm.value.u;

            return true;
        }

        return false;
    }

    [[msvc::noinline]] bool x64e_set_operand_value(x64e_core* core, uint8_t operand, uint16_t size, x64e_val const* val) {
    
        if (operand != 0) {

            return false;
        }

        if (core->m_operands[operand].type == ZYDIS_OPERAND_TYPE_REGISTER) {

            x64e_reg* reg{};

            if (!x64e_get_register_operand_reg(core, operand, &reg)) {

                return false;
            }

            if (x64e_is_operand_high_register(core, operand)) {
            
                reg->m_val.m_word.m_hi.m_u = val->m_word.m_lo.m_u;

                return true;
            }
        
            switch (size) {

                case 64: reg->m_val.m_qword.m_u = val->m_qword.m_u; break;
                case 32: reg->m_val.m_dword.m_u = val->m_dword.m_u; break;
                case 16: reg->m_val.m_word.m_u = val->m_word.m_u; break;
                case 8: reg->m_val.m_word.m_lo.m_u = val->m_word.m_lo.m_u; break;
            }

            if (size == 32) {

                if (!x64e_zero_extend(core, size, 64, &reg->m_val)) {

                    return false;
                }
            }

            return true;
        }

        else if (core->m_operands[operand].type == ZYDIS_OPERAND_TYPE_MEMORY) {
        
            x64e_val addr{};

            if (!x64e_get_operand_effective_address(core, operand, &addr)) {

                return false;
            }
        
            if (core->m_operands[operand].mem.base == ZYDIS_REGISTER_RSP ||
                core->m_operands[operand].mem.base == ZYDIS_REGISTER_RBP) {

                uint64_t stack_high = reinterpret_cast<uint64_t>(&core->m_stack[0]);
                uint64_t stack_low = reinterpret_cast<uint64_t>(&core->m_stack[sizeof(core->m_stack)]);

                if ((addr.m_qword.m_u >= stack_high) && (addr.m_qword.m_u < stack_low)) {

                    if ((addr.m_qword.m_u + size) >= stack_low) {

                        return false;
                    }
                    
                    uint64_t stack_off = addr.m_qword.m_u - stack_high;

                    if (core->m_write_callback(core, addr.m_qword.m_u, val, size, true, static_cast<bool>(_bittestandset64(reinterpret_cast<LONG64*>(&core->m_stack_dirty[stack_off >> 6]), stack_off & 0x3F)))) {
                        
                        return true;
                    }

                    switch (size) {

                        case 64: *reinterpret_cast<uint64_t*>(addr.m_qword.m_u) = val->m_qword.m_u; return true;
                        case 32: *reinterpret_cast<uint32_t*>(addr.m_qword.m_u) = val->m_dword.m_u; return true;
                        case 16: *reinterpret_cast<uint16_t*>(addr.m_qword.m_u) = val->m_word.m_u; return true;
                        case 8: *reinterpret_cast<uint8_t*>(addr.m_qword.m_u) = val->m_word.m_lo.m_u; return true;
                    }
                }

                printf("x64e: rsp/rbp bad. cannot continue.\n");

                return false;
            }

            if (core->m_operands[operand].mem.segment == ZYDIS_REGISTER_GS) {
                
                uint64_t teb_high = reinterpret_cast<uint64_t>(&core->m_teb[0]);
                uint64_t teb_low = reinterpret_cast<uint64_t>(&core->m_teb[sizeof(core->m_teb)]);

                if ((addr.m_qword.m_u >= teb_high) && (addr.m_qword.m_u < teb_low)) {

                    if ((addr.m_qword.m_u + size) >= teb_low) {

                        return false;
                    }
                    
                    switch (size) {

                        case 64: *reinterpret_cast<uint64_t*>(addr.m_qword.m_u) = val->m_qword.m_u; return true;
                        case 32: *reinterpret_cast<uint32_t*>(addr.m_qword.m_u) = val->m_dword.m_u; return true;
                        case 16: *reinterpret_cast<uint16_t*>(addr.m_qword.m_u) = val->m_word.m_u; return true;
                        case 8: *reinterpret_cast<uint8_t*>(addr.m_qword.m_u) = val->m_word.m_lo.m_u; return true;
                    }
                }

                printf("x64e: gs bad. cannot continue.\n");

                return false;
            }

            if (!core->m_allow_write) {

                // we don't want to write to anything in the process
                // we only want to allow writes to our emulated stack

                printf("x64e: write forbidden. cannot continue.\n");

                return false;
            }

            switch (size) {

                case 64: return x64e_do_write64(core, addr.m_qword.m_u, val);
                case 32: return x64e_do_write32(core, addr.m_qword.m_u, val);
                case 16: return x64e_do_write16(core, addr.m_qword.m_u, val);
                case 8: return x64e_do_write8(core, addr.m_qword.m_u, val);
            }
        }

        return false;
    }

    [[msvc::noinline]] bool x64e_emulate_bswap(x64e_core* core) {

        x64e_val src_val{};

        if (!x64e_get_operand_value(core, 0, core->m_operands[0].size, &src_val)) {

            return false;
        }

        switch (core->m_operands[0].size) {

            case 64: src_val.m_qword.m_u = _byteswap_uint64(src_val.m_qword.m_u); break;
            case 32: src_val.m_dword.m_u = _byteswap_ulong(src_val.m_dword.m_u); break;
        }

        if (!x64e_set_operand_value(core, 0, core->m_operands[0].size, &src_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_bt(x64e_core* core) {
        
        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: x64e_asm_bt64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: x64e_asm_bt32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: x64e_asm_bt16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_btc(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_btr(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_bts(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_call(x64e_core* core) {

        x64e_val opd1_val{};

        if (!x64e_get_operand_value(core, 0, core->m_operands[0].size, &opd1_val)) {

            return false;
        }

        if (core->m_operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {

            opd1_val.m_qword.m_u = static_cast<uint64_t>(core->m_rip.m_val.m_qword.m_s + core->m_instruction.length + opd1_val.m_qword.m_s);
        }

        x64e_do_branch(core, opd1_val.m_qword.m_u, true);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_cbw_cwde_cdqe(x64e_core* core) {

        auto& opd1 = core->m_operands[0];

        if (!x64e_sign_extend(core, opd1.size, opd1.size << 1, &core->m_rax.m_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_cwd(x64e_core* core) {

        auto& opd1 = core->m_operands[0];

        switch (opd1.size) {

            case 64: core->m_rdx.m_val.m_qword.m_s = (core->m_rax.m_val.m_qword.m_s < 0) ? -1 : 0; break;
            case 32: core->m_rdx.m_val.m_dword.m_s = (core->m_rax.m_val.m_dword.m_s < 0) ? -1 : 0; break;
            case 16: core->m_rdx.m_val.m_word.m_s = (core->m_rax.m_val.m_word.m_s < 0) ? -1 : 0; break;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_clc(x64e_core* core) {

        core->m_flags.m_fl.m_cf = 0;

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_cld(x64e_core* core) {

        core->m_flags.m_fl.m_df = 0;

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_stc(x64e_core* core) {

        core->m_flags.m_fl.m_cf = 1;

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_std(x64e_core* core) {

        core->m_flags.m_fl.m_df = 1;

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_cmc(x64e_core* core) {

        core->m_flags.m_fl.m_cf = ~core->m_flags.m_fl.m_cf;

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_cmovcc(x64e_core* core) {

        auto& dst_opd = core->m_operands[0];
        auto& src_opd = core->m_operands[1];

        x64e_val src{};

        bool condition{};

        switch (core->m_instruction.mnemonic) {

            case ZYDIS_MNEMONIC_CMOVNBE: condition = (core->m_flags.m_fl.m_cf == 0) && (core->m_flags.m_fl.m_zf == 0); break;
            case ZYDIS_MNEMONIC_CMOVBE: condition = (core->m_flags.m_fl.m_cf == 1) || (core->m_flags.m_fl.m_zf == 1); break;
            case ZYDIS_MNEMONIC_CMOVNB: condition = (core->m_flags.m_fl.m_cf == 0); break;
            case ZYDIS_MNEMONIC_CMOVB: condition = (core->m_flags.m_fl.m_cf == 1); break;
            case ZYDIS_MNEMONIC_CMOVZ: condition = (core->m_flags.m_fl.m_zf == 1); break;
            case ZYDIS_MNEMONIC_CMOVS: condition = (core->m_flags.m_fl.m_sf == 1); break;
            case ZYDIS_MNEMONIC_CMOVNP: condition = (core->m_flags.m_fl.m_pf == 0); break;
            case ZYDIS_MNEMONIC_CMOVP: condition = (core->m_flags.m_fl.m_pf == 1); break;
            case ZYDIS_MNEMONIC_CMOVO: condition = (core->m_flags.m_fl.m_of == 1); break;
            case ZYDIS_MNEMONIC_CMOVNZ: condition = (core->m_flags.m_fl.m_zf == 0); break;
            case ZYDIS_MNEMONIC_CMOVNS: condition = (core->m_flags.m_fl.m_sf == 0); break;
            case ZYDIS_MNEMONIC_CMOVNO: condition = (core->m_flags.m_fl.m_of == 0); break;
            case ZYDIS_MNEMONIC_CMOVNLE: condition = (core->m_flags.m_fl.m_zf == 0) && (core->m_flags.m_fl.m_sf == core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_CMOVNL: condition = (core->m_flags.m_fl.m_sf == core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_CMOVLE: condition = (core->m_flags.m_fl.m_zf == 1) || (core->m_flags.m_fl.m_sf != core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_CMOVL: condition = (core->m_flags.m_fl.m_sf != core->m_flags.m_fl.m_of); break;
        }

        if (condition) {

            if (!x64e_get_operand_value(core, 1, dst_opd.size, &src)) {

                return false;
            }

            if (!x64e_set_operand_value(core, 0, dst_opd.size, &src)) {

                return false;
            }
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_cmp(x64e_core* core) {
    
        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (opd1.size > opd2.size) {

            if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_sub64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_sub32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_sub16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_sub8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        x64e_do_step(core);

        return true;
    }

    /*fail*/ [[msvc::noinline]] bool x64e_emulate_cpuid(x64e_core* core) {

        return false;
    }

#pragma region math

    [[msvc::noinline]] bool x64e_emulate_adc(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_add(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (opd1.size > opd2.size) {

            if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_add64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_add32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_add16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_add8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_xadd(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_sub(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (opd1.size > opd2.size) {

            if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_sub64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_sub32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_sub16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_sub8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        //x64e_print_registers(core);

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_inc(x64e_core* core) {

        x64e_val opd1_val{};

        auto& opd1 = core->m_operands[0];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_inc64(opd1_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_inc32(opd1_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_inc16(opd1_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_inc8(opd1_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_dec(x64e_core* core) {

        x64e_val opd1_val{};

        auto& opd1 = core->m_operands[0];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_dec64(opd1_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_dec32(opd1_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_dec16(opd1_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_dec8(opd1_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_mul(x64e_core* core) {
    
        x64e_val opd1_val{};

        auto& opd1 = core->m_operands[0];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }
    
        x64e_reg flags{};

        switch (opd1.size) {

            case 64: x64e_asm_mul64(&core->m_rdx.m_val.m_qword.m_u, &core->m_rax.m_val.m_qword.m_u, opd1_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: x64e_asm_mul32(&core->m_rdx.m_val.m_dword.m_u, &core->m_rax.m_val.m_dword.m_u, opd1_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: x64e_asm_mul16(&core->m_rdx.m_val.m_word.m_u, &core->m_rax.m_val.m_word.m_u, opd1_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: x64e_asm_mul8(&core->m_rax.m_val.m_word.m_u, &core->m_rax.m_val.m_word.m_lo.m_u, opd1_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }
        
        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_imul(x64e_core* core) {
    
        x64e_val opd1_val{};
        x64e_val opd2_val{};
        x64e_val opd3_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];
        auto& opd3 = core->m_operands[2];

        if ((core->m_instruction.operand_count_visible >= 1 && !x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) ||
            (core->m_instruction.operand_count_visible >= 2 && !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) ||
            (core->m_instruction.operand_count_visible >= 3 && !x64e_get_operand_value(core, 2, opd3.size, &opd3_val))) {

            return false;
        }

        if (core->m_instruction.operand_count_visible == 1) {

            x64e_reg flags{};

            switch (opd1.size) {

                case 64: x64e_asm_imul64(&core->m_rdx.m_val.m_qword.m_u, &core->m_rax.m_val.m_qword.m_u, opd1_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
                case 32: x64e_asm_imul32(&core->m_rdx.m_val.m_dword.m_u, &core->m_rax.m_val.m_dword.m_u, opd1_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
                case 16: x64e_asm_imul16(&core->m_rdx.m_val.m_word.m_u, &core->m_rax.m_val.m_word.m_u, opd1_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
                case 8: x64e_asm_imul8(&core->m_rax.m_val.m_word.m_u, &core->m_rax.m_val.m_word.m_lo.m_u, opd1_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            }
        
            core->m_flags.m_fl.m_of = flags.m_fl.m_of;
            core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;
        }

        else if (core->m_instruction.operand_count_visible == 2) {

            if (opd1.size > opd2.size) {

                if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                    return false;
                }
            }

            x64e_reg flags{};

            switch (opd1.size) {

                case 64: x64e_asm_imuln64(&opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
                case 32: x64e_asm_imuln32(&opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
                case 16: x64e_asm_imuln16(&opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            }

            core->m_flags.m_fl.m_of = flags.m_fl.m_of;
            core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

            if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

                return false;
            }
        }

        else if (core->m_instruction.operand_count_visible == 3) {

            if (opd2.size > opd3.size) {

                if (!x64e_sign_extend(core, opd3.size, opd2.size, &opd3_val)) {

                    return false;
                }
            }

            x64e_reg flags{};

            switch (opd2.size) {

                case 64: x64e_asm_imuln64(&opd2_val.m_qword.m_u, opd3_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
                case 32: x64e_asm_imuln32(&opd2_val.m_dword.m_u, opd3_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
                case 16: x64e_asm_imuln16(&opd2_val.m_word.m_u, opd3_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            }

            core->m_flags.m_fl.m_of = flags.m_fl.m_of;
            core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

            if (!x64e_set_operand_value(core, 0, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_div(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_idiv(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_rcl(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_rcl64(opd1_val.m_qword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_rcl32(opd1_val.m_dword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_rcl16(opd1_val.m_word.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_rcl8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }
    
        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_rcr(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_rcr64(opd1_val.m_qword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_rcr32(opd1_val.m_dword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_rcr16(opd1_val.m_word.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_rcr8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }
    
        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_rol(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_rol64(opd1_val.m_qword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_rol32(opd1_val.m_dword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_rol16(opd1_val.m_word.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_rol8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }
    
        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_ror(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_ror64(opd1_val.m_qword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_ror32(opd1_val.m_dword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_ror16(opd1_val.m_word.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_ror8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }
    
        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_sar(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_sar64(opd1_val.m_qword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_sar32(opd1_val.m_dword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_sar16(opd1_val.m_word.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_sar8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_shl(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_shl64(opd1_val.m_qword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_shl32(opd1_val.m_dword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_shl16(opd1_val.m_word.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_shl8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_shr(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_shr64(opd1_val.m_qword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_shr32(opd1_val.m_dword.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_shr16(opd1_val.m_word.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_shr8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_xor(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (opd1.size > opd2.size) {

            if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_xor64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_xor32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_xor16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_xor8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_and(x64e_core* core) {
    
        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (opd1.size > opd2.size) {

            if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_and64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_and32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_and16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_and8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_andn(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_neg(x64e_core* core) {

        x64e_val opd1_val{};

        auto& opd1 = core->m_operands[0];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_neg64(opd1_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_neg32(opd1_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_neg16(opd1_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_neg8(opd1_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_not(x64e_core* core) {

        x64e_val opd1_val{};

        auto& opd1 = core->m_operands[0];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_not64(opd1_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_not32(opd1_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_not16(opd1_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_not8(opd1_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_or(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (opd1.size > opd2.size) {

            if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_or64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_or32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_or16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_or8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

#pragma endregion

    /*done*/ [[msvc::noinline]] bool x64e_emulate_int3(x64e_core* core) {

        x64e_print_registers(core);

        return false;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_jmp(x64e_core* core) {

        x64e_val opd1_val{};

        if (!x64e_get_operand_value(core, 0, core->m_operands[0].size, &opd1_val)) {

            return false;
        }

        if (core->m_operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {

            opd1_val.m_qword.m_u = static_cast<uint64_t>(core->m_rip.m_val.m_qword.m_s + core->m_instruction.length + opd1_val.m_qword.m_s);
        }

        x64e_do_branch(core, opd1_val.m_qword.m_u, false);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_jcc(x64e_core* core) {

        bool condition{};
    
        x64e_val opd1_val{};

        switch (core->m_instruction.mnemonic) {

            case ZYDIS_MNEMONIC_JB: condition = (core->m_flags.m_fl.m_cf == 1); break;
            case ZYDIS_MNEMONIC_JBE: condition = (core->m_flags.m_fl.m_cf == 1) || (core->m_flags.m_fl.m_zf == 1); break;
            case ZYDIS_MNEMONIC_JCXZ: condition = (core->m_rcx.m_val.m_word.m_u == 0); break;
            case ZYDIS_MNEMONIC_JECXZ: condition = (core->m_rcx.m_val.m_dword.m_u == 0); break;
            case ZYDIS_MNEMONIC_JL: condition = (core->m_flags.m_fl.m_sf != core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_JLE: condition = (core->m_flags.m_fl.m_zf == 1) || (core->m_flags.m_fl.m_sf != core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_JNB: condition = (core->m_flags.m_fl.m_cf == 0); break;
            case ZYDIS_MNEMONIC_JNBE: condition = (core->m_flags.m_fl.m_cf == 0) && (core->m_flags.m_fl.m_zf == 0); break;
            case ZYDIS_MNEMONIC_JNL: condition = (core->m_flags.m_fl.m_sf == core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_JNLE: condition = (core->m_flags.m_fl.m_zf == 0) && (core->m_flags.m_fl.m_sf == core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_JNO: condition = (core->m_flags.m_fl.m_of == 0); break;
            case ZYDIS_MNEMONIC_JNP: condition = (core->m_flags.m_fl.m_pf == 0); break;
            case ZYDIS_MNEMONIC_JNS: condition = (core->m_flags.m_fl.m_sf == 0); break;
            case ZYDIS_MNEMONIC_JNZ: condition = (core->m_flags.m_fl.m_zf == 0); break;
            case ZYDIS_MNEMONIC_JO: condition = (core->m_flags.m_fl.m_of == 1); break;
            case ZYDIS_MNEMONIC_JP: condition = (core->m_flags.m_fl.m_pf == 1); break;
            case ZYDIS_MNEMONIC_JRCXZ: condition = (core->m_rcx.m_val.m_qword.m_u == 0); break;
            case ZYDIS_MNEMONIC_JS: condition = (core->m_flags.m_fl.m_sf == 1); break;
            case ZYDIS_MNEMONIC_JZ: condition = (core->m_flags.m_fl.m_zf == 1); break;
        }

        if (condition) {

            if (!x64e_get_operand_value(core, 0, core->m_operands[0].size, &opd1_val)) {

                return false;
            }

            if (core->m_operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {

                opd1_val.m_qword.m_u = static_cast<uint64_t>(core->m_rip.m_val.m_qword.m_s + core->m_instruction.length + opd1_val.m_qword.m_s);
            }

            x64e_do_branch(core, opd1_val.m_qword.m_u, false);

        } else {

            x64e_do_step(core);
        }
    
        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_lea(x64e_core* core) {

        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (opd2.type == ZYDIS_OPERAND_TYPE_MEMORY) {

            if (!x64e_get_operand_effective_address(core, 1, &opd2_val)) {

                return false;
            }

        }  else {

            if (!x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

                return false;
            }
        }

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd2_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_loopcc(x64e_core* core) {

        bool condition{};

        auto& opd1 = core->m_operands[0];

        x64e_val opd1_val{};

        switch (core->m_instruction.address_width) {

            case 64: condition = (--core->m_rcx.m_val.m_qword.m_u != 0); break;
            case 32: condition = (--core->m_rcx.m_val.m_dword.m_u != 0); break;
            case 16: condition = (--core->m_rcx.m_val.m_word.m_u != 0); break;
        }

        switch (core->m_instruction.mnemonic) {

            case ZYDIS_MNEMONIC_LOOPE: condition = (condition && (core->m_flags.m_fl.m_zf == 1)); break;
            case ZYDIS_MNEMONIC_LOOPNE: condition = (condition && (core->m_flags.m_fl.m_zf == 0)); break;
        }

        if (condition) {

            if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

                return false;
            }

            if (!x64e_sign_extend(core, opd1.size, 64, &opd1_val)) {

                return false;
            }

            x64e_do_branch(core, static_cast<uint64_t>(opd1_val.m_qword.m_u + core->m_rip.m_val.m_qword.m_u + core->m_instruction.length), false);

        } else {

            x64e_do_step(core);
        }

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_lzcnt(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_tzcnt(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_mov(x64e_core* core) {

        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd2_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_movzx(x64e_core* core) {

        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (!x64e_zero_extend(core, opd2.size, opd1.size, &opd2_val)) {

            return false;
        }

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd2_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_movsx(x64e_core* core) {

        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

            return false;
        }

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd2_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_nop(x64e_core* core) {

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_pop(x64e_core* core) {

        x64e_val opd1_val{};

        auto& opd1 = core->m_operands[0];

        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_do_pop64(core); break;
            case 16: opd1_val.m_word.m_u = x64e_do_pop16(core); break;
        }

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_popfq(x64e_core* core) {

        auto& opd1 = core->m_operands[0];

        x64e_reg flags{};

        switch (opd1.size) {

            case 64: flags.m_val.m_qword.m_u = x64e_do_pop64(core); break;
            case 16: flags.m_val.m_word.m_u = x64e_do_pop16(core); break;
        }

        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_af = flags.m_fl.m_af;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_tf = flags.m_fl.m_tf;
        core->m_flags.m_fl.m_df = flags.m_fl.m_df;
        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_nt = flags.m_fl.m_nt;
        core->m_flags.m_fl.m_rf = 0;
        core->m_flags.m_fl.m_ac = flags.m_fl.m_ac;
        core->m_flags.m_fl.m_id = flags.m_fl.m_id;

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_push(x64e_core* core) {

        x64e_val opd1_val{};

        auto& opd1 = core->m_operands[0];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        }

        switch (opd1.size) {

            case 64: x64e_do_push64(core, opd1_val.m_qword.m_u); break;
            case 16: x64e_do_push16(core, opd1_val.m_word.m_u); break;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_pushfq(x64e_core* core) {

        auto& opd1 = core->m_operands[0];

        x64e_reg flags = core->m_flags;

        flags.m_val.m_qword.m_u &= 0xFFFFFF;

        flags.m_fl.m_vm = 0;
        flags.m_fl.m_rf = 0;

        switch (opd1.size) {

            case 64: x64e_do_push64(core, flags.m_val.m_qword.m_u); break;
            case 16: x64e_do_push16(core, flags.m_val.m_word.m_u); break;
        }

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_ret(x64e_core* core) {

        x64e_do_return(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_setcc(x64e_core* core) {
        
        x64e_val in_val{};

        bool condition{};

        auto& dst_opd = core->m_operands[0];

        switch (core->m_instruction.mnemonic) {

            case ZYDIS_MNEMONIC_SETB: condition = (core->m_flags.m_fl.m_cf == 1);  break;
            case ZYDIS_MNEMONIC_SETBE: condition = (core->m_flags.m_fl.m_cf == 1) || (core->m_flags.m_fl.m_zf == 1); break;
            case ZYDIS_MNEMONIC_SETL: condition = (core->m_flags.m_fl.m_sf != core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_SETLE: condition = (core->m_flags.m_fl.m_zf == 1) || (core->m_flags.m_fl.m_sf != core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_SETNB: condition = (core->m_flags.m_fl.m_cf == 0); break;
            case ZYDIS_MNEMONIC_SETNBE: condition = (core->m_flags.m_fl.m_cf == 0) && (core->m_flags.m_fl.m_zf == 0); break;
            case ZYDIS_MNEMONIC_SETNL: condition = (core->m_flags.m_fl.m_sf == core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_SETNLE: condition = (core->m_flags.m_fl.m_zf == 0) && (core->m_flags.m_fl.m_sf == core->m_flags.m_fl.m_of); break;
            case ZYDIS_MNEMONIC_SETNO: condition = (core->m_flags.m_fl.m_of == 0); break;
            case ZYDIS_MNEMONIC_SETNP: condition = (core->m_flags.m_fl.m_pf == 0); break;
            case ZYDIS_MNEMONIC_SETNS: condition = (core->m_flags.m_fl.m_sf == 0); break;
            case ZYDIS_MNEMONIC_SETNZ: condition = (core->m_flags.m_fl.m_zf == 0); break;
            case ZYDIS_MNEMONIC_SETO: condition = (core->m_flags.m_fl.m_of == 1); break;
            case ZYDIS_MNEMONIC_SETP: condition = (core->m_flags.m_fl.m_pf == 1); break;
            case ZYDIS_MNEMONIC_SETS: condition = (core->m_flags.m_fl.m_sf == 1); break;
            case ZYDIS_MNEMONIC_SETSSBSY: return false; // can't be executed if cpl > 0
            case ZYDIS_MNEMONIC_SETZ: condition = (core->m_flags.m_fl.m_zf == 1); break;
        }

        in_val.m_qword.m_u = condition ? 1 : 0;

        if (!x64e_set_operand_value(core, 0, dst_opd.size, &in_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_emulate_stos(x64e_core* core) {

        // TODO: implement

        x64e_do_step(core);

        return true;
    }

    /*fail*/ [[msvc::noinline]] bool x64e_emulate_syscall(x64e_core* core) {

        return false;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_test(x64e_core* core) {
    
        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val) ||
            !x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (opd1.size > opd2.size) {

            if (!x64e_sign_extend(core, opd2.size, opd1.size, &opd2_val)) {

                return false;
            }
        }

        x64e_reg flags{};
    
        switch (opd1.size) {

            case 64: opd1_val.m_qword.m_u = x64e_asm_and64(opd1_val.m_qword.m_u, opd2_val.m_qword.m_u, &flags.m_val.m_qword.m_u); break;
            case 32: opd1_val.m_dword.m_u = x64e_asm_and32(opd1_val.m_dword.m_u, opd2_val.m_dword.m_u, &flags.m_val.m_qword.m_u); break;
            case 16: opd1_val.m_word.m_u = x64e_asm_and16(opd1_val.m_word.m_u, opd2_val.m_word.m_u, &flags.m_val.m_qword.m_u); break;
            case 8: opd1_val.m_word.m_lo.m_u = x64e_asm_and8(opd1_val.m_word.m_lo.m_u, opd2_val.m_word.m_lo.m_u, &flags.m_val.m_qword.m_u); break;
        }

        core->m_flags.m_fl.m_of = flags.m_fl.m_of;
        core->m_flags.m_fl.m_sf = flags.m_fl.m_sf;
        core->m_flags.m_fl.m_zf = flags.m_fl.m_zf;
        core->m_flags.m_fl.m_pf = flags.m_fl.m_pf;
        core->m_flags.m_fl.m_cf = flags.m_fl.m_cf;

        x64e_do_step(core);

        return true;
    }

    /*done*/ [[msvc::noinline]] bool x64e_emulate_xchg(x64e_core* core) {

        x64e_val opd1_val{};
        x64e_val opd2_val{};

        auto& opd1 = core->m_operands[0];
        auto& opd2 = core->m_operands[1];

        if (!x64e_get_operand_value(core, 0, opd1.size, &opd1_val)) {

            return false;
        } 
    
        if (!x64e_get_operand_value(core, 1, opd2.size, &opd2_val)) {

            return false;
        }

        if (!x64e_set_operand_value(core, 0, opd1.size, &opd2_val)) {

            return false;
        }
    
        if (!x64e_set_operand_value(core, 1, opd2.size, &opd1_val)) {

            return false;
        }

        x64e_do_step(core);

        return true;
    }

    [[msvc::noinline]] bool x64e_run_core(x64e_core* core, void* user_data) {

        ZydisDecoder decoder{};

        if (ZYAN_FAILED(core->m_status = ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64))) {

            printf("x64e_run_core unable to initialize decoder\n");

            return false;
        }
        
        while (true) {

            if (!core->m_icache.contains(core->m_rip.m_val.m_qword.m_u)) {

                x64e_val instruction_bytes{};

                if (!core->m_read_callback(core, core->m_rip.m_val.m_qword.m_u, &instruction_bytes, ZYDIS_MAX_INSTRUCTION_LENGTH, false, false)) {

                    printf("x64e: cannot read instruction: %llX\n", core->m_rip.m_val.m_qword.m_u);

                    break;
                }

                if (ZYAN_FAILED(core->m_status = ZydisDecoderDecodeFull(&decoder, reinterpret_cast<const void*>(&instruction_bytes.m_raw[0]), ZYDIS_MAX_INSTRUCTION_LENGTH, &core->m_instruction, core->m_operands))) {

                    printf("x64e: cannot decode instruction: %llX\n", core->m_rip.m_val.m_qword.m_u);

                    break;
                }

                auto& [instruction, operands] = core->m_icache[core->m_rip.m_val.m_qword.m_u];

                instruction = core->m_instruction;
                operands[0] = core->m_operands[0];
                operands[1] = core->m_operands[1];
                operands[2] = core->m_operands[2];
                operands[3] = core->m_operands[3];
                operands[4] = core->m_operands[4];
                operands[5] = core->m_operands[5];
                operands[6] = core->m_operands[6];
                operands[7] = core->m_operands[7];
                operands[8] = core->m_operands[8];
                operands[9] = core->m_operands[9];
            }

            auto& [instruction, operands] = core->m_icache[core->m_rip.m_val.m_qword.m_u];

            core->m_instruction = instruction;
            core->m_operands[0] = operands[0];
            core->m_operands[1] = operands[1];
            core->m_operands[2] = operands[2];
            core->m_operands[3] = operands[3];
            core->m_operands[4] = operands[4];
            core->m_operands[5] = operands[5];
            core->m_operands[6] = operands[6];
            core->m_operands[7] = operands[7];
            core->m_operands[8] = operands[8];
            core->m_operands[9] = operands[9];

            if (core->m_instruction.attributes & ZYDIS_ATTRIB_IS_PRIVILEGED) {

                // we're not interested in these instructions...
                // there's no plans to use this below ring 3...

                core->m_status = ZYDIS_STATUS_DECODING_ERROR;

                break;
            }

            switch (core->m_debug_callback(core, user_data)) {

                case x64e_debug_result::fail:
                    //printf("debug fail @ %llx (mnemonic: %u)", core->m_rip.m_val.m_qword.m_u, core->m_instruction.mnemonic);
                    return false;

                case x64e_debug_result::exit:
                    //printf("debug exit @ %llx (mnemonic: %u)", core->m_rip.m_val.m_qword.m_u, core->m_instruction.mnemonic);
                    return true;

                case x64e_debug_result::skip:
                    //printf("debug skip @ %llx (mnemonic: %u)", core->m_rip.m_val.m_qword.m_u, core->m_instruction.mnemonic);
                    x64e_do_step(core);
                    continue;

                case x64e_debug_result::step:
                    //printf("debug step @ %llx (mnemonic: %u)", core->m_rip.m_val.m_qword.m_u, core->m_instruction.mnemonic);
                    break;
            }

            bool emulated{};

            switch (core->m_instruction.mnemonic) {

                case ZYDIS_MNEMONIC_CALL: emulated = x64e_emulate_call(core); break;
                case ZYDIS_MNEMONIC_JMP: emulated = x64e_emulate_jmp(core); break;
                case ZYDIS_MNEMONIC_PUSH: emulated = x64e_emulate_push(core); break;
                case ZYDIS_MNEMONIC_POP: emulated = x64e_emulate_pop(core); break;
                case ZYDIS_MNEMONIC_PUSHFQ: emulated = x64e_emulate_pushfq(core); break;
                case ZYDIS_MNEMONIC_POPFQ: emulated = x64e_emulate_popfq(core); break;
                case ZYDIS_MNEMONIC_RET: emulated = x64e_emulate_ret(core); break;
                case ZYDIS_MNEMONIC_LEA: emulated = x64e_emulate_lea(core); break;
                case ZYDIS_MNEMONIC_MOV: emulated = x64e_emulate_mov(core); break;
                case ZYDIS_MNEMONIC_MOVZX: emulated = x64e_emulate_movzx(core); break;
                case ZYDIS_MNEMONIC_ADD: emulated = x64e_emulate_add(core); break;
                case ZYDIS_MNEMONIC_AND: emulated = x64e_emulate_and(core); break;
                case ZYDIS_MNEMONIC_SUB: emulated = x64e_emulate_sub(core); break;
                case ZYDIS_MNEMONIC_XCHG: emulated = x64e_emulate_xchg(core); break;
                case ZYDIS_MNEMONIC_CMP: emulated = x64e_emulate_cmp(core); break;
                case ZYDIS_MNEMONIC_TEST: emulated = x64e_emulate_test(core); break;
                case ZYDIS_MNEMONIC_DEC: emulated = x64e_emulate_dec(core); break;
                case ZYDIS_MNEMONIC_INC: emulated = x64e_emulate_inc(core); break;
                case ZYDIS_MNEMONIC_BSWAP: emulated = x64e_emulate_bswap(core); break;
                case ZYDIS_MNEMONIC_CLC: emulated = x64e_emulate_clc(core); break;
                case ZYDIS_MNEMONIC_CLD: emulated = x64e_emulate_cld(core); break;
                case ZYDIS_MNEMONIC_STC: emulated = x64e_emulate_clc(core); break;
                case ZYDIS_MNEMONIC_STD: emulated = x64e_emulate_cld(core); break;
                case ZYDIS_MNEMONIC_CMC: emulated = x64e_emulate_cmc(core); break;
                case ZYDIS_MNEMONIC_CWD: emulated = x64e_emulate_cwd(core); break;
                case ZYDIS_MNEMONIC_NEG: emulated = x64e_emulate_neg(core); break;
                case ZYDIS_MNEMONIC_NOT: emulated = x64e_emulate_not(core); break;
                case ZYDIS_MNEMONIC_OR: emulated = x64e_emulate_or(core); break;
                case ZYDIS_MNEMONIC_XOR: emulated = x64e_emulate_xor(core); break;
                case ZYDIS_MNEMONIC_RCL: emulated = x64e_emulate_rcl(core); break;
                case ZYDIS_MNEMONIC_RCR: emulated = x64e_emulate_rcr(core); break;
                case ZYDIS_MNEMONIC_ROL: emulated = x64e_emulate_rol(core); break;
                case ZYDIS_MNEMONIC_ROR: emulated = x64e_emulate_ror(core); break;
                case ZYDIS_MNEMONIC_SAR: emulated = x64e_emulate_sar(core); break;
                case ZYDIS_MNEMONIC_SHL: emulated = x64e_emulate_shl(core); break;
                case ZYDIS_MNEMONIC_SHR: emulated = x64e_emulate_shr(core); break;
                case ZYDIS_MNEMONIC_MUL: emulated = x64e_emulate_mul(core); break;
                case ZYDIS_MNEMONIC_IMUL: emulated = x64e_emulate_imul(core); break;
                case ZYDIS_MNEMONIC_INT3: emulated = x64e_emulate_int3(core); break;
                case ZYDIS_MNEMONIC_NOP: emulated = x64e_emulate_nop(core); break;
                case ZYDIS_MNEMONIC_BT: emulated = x64e_emulate_bt(core); break;

                case ZYDIS_MNEMONIC_CBW: 
                case ZYDIS_MNEMONIC_CWDE: 
                case ZYDIS_MNEMONIC_CDQE: 
                    emulated = x64e_emulate_cbw_cwde_cdqe(core);
                    break;
                

                case ZYDIS_MNEMONIC_MOVSX:
                case ZYDIS_MNEMONIC_MOVSXD:
                    emulated = x64e_emulate_movsx(core);
                    break;

                case ZYDIS_MNEMONIC_LOOP:
                case ZYDIS_MNEMONIC_LOOPE:
                case ZYDIS_MNEMONIC_LOOPNE:
                    emulated = x64e_emulate_loopcc(core);
                    break;

                case ZYDIS_MNEMONIC_CMOVNBE:
                case ZYDIS_MNEMONIC_CMOVBE: 
                case ZYDIS_MNEMONIC_CMOVNB: 
                case ZYDIS_MNEMONIC_CMOVB: 
                case ZYDIS_MNEMONIC_CMOVZ: 
                case ZYDIS_MNEMONIC_CMOVS: 
                case ZYDIS_MNEMONIC_CMOVNP: 
                case ZYDIS_MNEMONIC_CMOVP: 
                case ZYDIS_MNEMONIC_CMOVO: 
                case ZYDIS_MNEMONIC_CMOVNZ: 
                case ZYDIS_MNEMONIC_CMOVNS: 
                case ZYDIS_MNEMONIC_CMOVNO: 
                case ZYDIS_MNEMONIC_CMOVNLE:
                case ZYDIS_MNEMONIC_CMOVNL: 
                case ZYDIS_MNEMONIC_CMOVLE: 
                case ZYDIS_MNEMONIC_CMOVL: 
                    emulated = x64e_emulate_cmovcc(core);
                    break;

                case ZYDIS_MNEMONIC_JB:
                case ZYDIS_MNEMONIC_JBE:
                case ZYDIS_MNEMONIC_JCXZ: 
                case ZYDIS_MNEMONIC_JECXZ:
                case ZYDIS_MNEMONIC_JL:
                case ZYDIS_MNEMONIC_JLE:
                case ZYDIS_MNEMONIC_JNB:
                case ZYDIS_MNEMONIC_JNBE:
                case ZYDIS_MNEMONIC_JNL:
                case ZYDIS_MNEMONIC_JNLE:
                case ZYDIS_MNEMONIC_JNO:
                case ZYDIS_MNEMONIC_JNP:
                case ZYDIS_MNEMONIC_JNS:
                case ZYDIS_MNEMONIC_JNZ:
                case ZYDIS_MNEMONIC_JO:
                case ZYDIS_MNEMONIC_JP:
                case ZYDIS_MNEMONIC_JRCXZ:
                case ZYDIS_MNEMONIC_JS:
                case ZYDIS_MNEMONIC_JZ:
                    emulated = x64e_emulate_jcc(core);
                    break;

                case ZYDIS_MNEMONIC_SETB: 
                case ZYDIS_MNEMONIC_SETBE: 
                case ZYDIS_MNEMONIC_SETL: 
                case ZYDIS_MNEMONIC_SETLE: 
                case ZYDIS_MNEMONIC_SETNB: 
                case ZYDIS_MNEMONIC_SETNBE: 
                case ZYDIS_MNEMONIC_SETNL:
                case ZYDIS_MNEMONIC_SETNLE:
                case ZYDIS_MNEMONIC_SETNO:
                case ZYDIS_MNEMONIC_SETNP:
                case ZYDIS_MNEMONIC_SETNS:
                case ZYDIS_MNEMONIC_SETNZ:
                case ZYDIS_MNEMONIC_SETO:
                case ZYDIS_MNEMONIC_SETP:
                case ZYDIS_MNEMONIC_SETS:
                case ZYDIS_MNEMONIC_SETSSBSY:
                case ZYDIS_MNEMONIC_SETZ:
                    emulated = x64e_emulate_setcc(core);
                    break;
            }

            if (!emulated) {

                printf("x64e: emulation failed, an instruction may not be implemented (mnemonic: %u) (status: %x)", core->m_instruction.mnemonic, core->m_status);

                x64e_print_registers(core);

                // __debugbreak();

                break;
            }
        }

        return false;
    }
}