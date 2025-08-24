#pragma once
#include "vendor/Zydis.h"

#include "stdafx.h"

namespace utility {
    
    enum class x64e_debug_result : uint8_t {
        fail, // an error happened, all code couldn't be ran as expected
        exit, // all code executed as expected, we don't need to execute anything else
        skip, // we don't need to execute this instruction, it's not important to us
        step  // instruction will not cause unexpected behaviour, it's cool to execute
    };

    typedef bool(*x64e_read_callback_t)(struct x64e_core* core, uint64_t address, void* value, uint16_t size, bool is_stack, bool is_dirty);
    typedef bool(*x64e_write_callback_t)(struct x64e_core* core, uint64_t address, void const* value, uint16_t size, bool is_stack, bool is_dirty);
    typedef x64e_debug_result(*x64e_debug_callback_t)(struct x64e_core* core, void* user_data);

    #pragma pack(push, 1)

    union x64e_val {

        uint8_t m_raw[0x10]{};

        __m512i m_zword; // zmm
        __m256i m_yword;   // ymm
        __m128i m_oword;   // xmm

        union {

            uint64_t m_u;
            int64_t m_s;

        } m_qword;

        union {

            uint32_t m_u;
            int32_t m_s;

        } m_dword;

        union {

            uint16_t m_u;
            int16_t m_s;

            struct {

                union {

                    uint8_t m_u;
                    int8_t m_s;

                } m_lo;

                union {

                    uint8_t m_u;
                    int8_t m_s;

                } m_hi;
            };

        } m_word;
    };

    union x64e_reg {

        x64e_val m_val{};
    
        struct {

            // flags

            uint64_t m_cf : 1;
            uint64_t m_r1 : 1;
            uint64_t m_pf : 1;
            uint64_t m_r2 : 1;
            uint64_t m_af : 1;
            uint64_t m_r3 : 1;
            uint64_t m_zf : 1;
            uint64_t m_sf : 1;
            uint64_t m_tf : 1;
            uint64_t m_if : 1;
            uint64_t m_df : 1;
            uint64_t m_of : 1;
            uint64_t m_iopl : 2;
            uint64_t m_nt : 1;
            uint64_t m_r4 : 1;

            // eflags

            uint64_t m_rf : 1;
            uint64_t m_vm : 1;
            uint64_t m_ac : 1;
            uint64_t m_vif : 1;
            uint64_t m_vip : 1;
            uint64_t m_id : 1;

            // rflags

            uint64_t m_r5 : 42;

        } m_fl;
    };

    #pragma pack(pop)

    struct x64e_core {

        x64e_reg m_rip{};

        x64e_reg m_cs{};
        x64e_reg m_ss{};
        x64e_reg m_ds{};
        x64e_reg m_es{};
        x64e_reg m_fs{};
        x64e_reg m_gs{};

        x64e_reg m_flags{};

        x64e_reg m_rax{};
        x64e_reg m_rcx{};
        x64e_reg m_rdx{};
        x64e_reg m_rbx{};
        x64e_reg m_rsp{};
        x64e_reg m_rbp{};
        x64e_reg m_rsi{};
        x64e_reg m_rdi{};
        x64e_reg m_r8{};
        x64e_reg m_r9{};
        x64e_reg m_r10{};
        x64e_reg m_r11{};
        x64e_reg m_r12{};
        x64e_reg m_r13{};
        x64e_reg m_r14{};
        x64e_reg m_r15{};

        x64e_reg m_zmm0{};
        x64e_reg m_zmm1{};
        x64e_reg m_zmm2{};
        x64e_reg m_zmm3{};
        x64e_reg m_zmm4{};
        x64e_reg m_zmm5{};
        x64e_reg m_zmm6{};
        x64e_reg m_zmm7{};
        x64e_reg m_zmm8{};
        x64e_reg m_zmm9{};
        x64e_reg m_zmm10{};
        x64e_reg m_zmm11{};
        x64e_reg m_zmm12{};
        x64e_reg m_zmm13{};
        x64e_reg m_zmm14{};
        x64e_reg m_zmm15{};
        x64e_reg m_zmm16{};
        x64e_reg m_zmm17{};
        x64e_reg m_zmm18{};
        x64e_reg m_zmm19{};
        x64e_reg m_zmm20{};
        x64e_reg m_zmm21{};
        x64e_reg m_zmm22{};
        x64e_reg m_zmm23{};
        x64e_reg m_zmm24{};
        x64e_reg m_zmm25{};
        x64e_reg m_zmm26{};
        x64e_reg m_zmm27{};
        x64e_reg m_zmm28{};
        x64e_reg m_zmm29{};
        x64e_reg m_zmm30{};
        x64e_reg m_zmm31{};

        std::map<uint64_t, std::pair<ZydisDecodedInstruction, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>>> m_icache{};

        ZydisDecodedInstruction m_instruction{};
        ZydisDecodedOperand m_operands[ZYDIS_MAX_OPERAND_COUNT]{};
        ZyanStatus m_status{};

        bool m_allow_write{};
        bool m_allow_read{};

        x64e_read_callback_t m_read_callback{};
        x64e_write_callback_t m_write_callback{};
        x64e_debug_callback_t m_debug_callback{};

        uint64_t m_stack_dirty[256]{};

        uint8_t m_stack[0x4000]{};
        uint8_t m_teb[0x2000]{};

        void* m_entry{};
    };

    extern "C" uint8_t  x64e_asm_rcl8(uint8_t  v1, uint8_t count, uint64_t* flags);
    extern "C" uint16_t x64e_asm_rcl16(uint16_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint32_t x64e_asm_rcl32(uint32_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint64_t x64e_asm_rcl64(uint64_t v1, uint8_t count, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_rcr8(uint8_t  v1, uint8_t count, uint64_t* flags);
    extern "C" uint16_t x64e_asm_rcr16(uint16_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint32_t x64e_asm_rcr32(uint32_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint64_t x64e_asm_rcr64(uint64_t v1, uint8_t count, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_rol8(uint8_t  v1, uint8_t count, uint64_t* flags);
    extern "C" uint16_t x64e_asm_rol16(uint16_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint32_t x64e_asm_rol32(uint32_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint64_t x64e_asm_rol64(uint64_t v1, uint8_t count, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_ror8(uint8_t  v1, uint8_t count, uint64_t* flags);
    extern "C" uint16_t x64e_asm_ror16(uint16_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint32_t x64e_asm_ror32(uint32_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint64_t x64e_asm_ror64(uint64_t v1, uint8_t count, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_sar8(uint8_t  v1, uint8_t count, uint64_t* flags);
    extern "C" uint16_t x64e_asm_sar16(uint16_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint32_t x64e_asm_sar32(uint32_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint64_t x64e_asm_sar64(uint64_t v1, uint8_t count, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_shl8(uint8_t  v1, uint8_t count, uint64_t* flags);
    extern "C" uint16_t x64e_asm_shl16(uint16_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint32_t x64e_asm_shl32(uint32_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint64_t x64e_asm_shl64(uint64_t v1, uint8_t count, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_shr8(uint8_t  v1, uint8_t count, uint64_t* flags);
    extern "C" uint16_t x64e_asm_shr16(uint16_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint32_t x64e_asm_shr32(uint32_t v1, uint8_t count, uint64_t* flags);
    extern "C" uint64_t x64e_asm_shr64(uint64_t v1, uint8_t count, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_xor8(uint8_t  v1, uint8_t  v2, uint64_t* flags);
    extern "C" uint16_t x64e_asm_xor16(uint16_t v1, uint16_t v2, uint64_t* flags);
    extern "C" uint32_t x64e_asm_xor32(uint32_t v1, uint32_t v2, uint64_t* flags);
    extern "C" uint64_t x64e_asm_xor64(uint64_t v1, uint64_t v2, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_and8(uint8_t  v1, uint8_t  v2, uint64_t* flags);
    extern "C" uint16_t x64e_asm_and16(uint16_t v1, uint16_t v2, uint64_t* flags);
    extern "C" uint32_t x64e_asm_and32(uint32_t v1, uint32_t v2, uint64_t* flags);
    extern "C" uint64_t x64e_asm_and64(uint64_t v1, uint64_t v2, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_or8(uint8_t  v1, uint8_t  v2, uint64_t* flags);
    extern "C" uint16_t x64e_asm_or16(uint16_t v1, uint16_t v2, uint64_t* flags);
    extern "C" uint32_t x64e_asm_or32(uint32_t v1, uint32_t v2, uint64_t* flags);
    extern "C" uint64_t x64e_asm_or64(uint64_t v1, uint64_t v2, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_not8(uint8_t  v1, uint64_t* flags);
    extern "C" uint16_t x64e_asm_not16(uint16_t v1, uint64_t* flags);
    extern "C" uint32_t x64e_asm_not32(uint32_t v1, uint64_t* flags);
    extern "C" uint64_t x64e_asm_not64(uint64_t v1, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_neg8(uint8_t  v1, uint64_t* flags);
    extern "C" uint16_t x64e_asm_neg16(uint16_t v1, uint64_t* flags);
    extern "C" uint32_t x64e_asm_neg32(uint32_t v1, uint64_t* flags);
    extern "C" uint64_t x64e_asm_neg64(uint64_t v1, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_sub8(uint8_t  v1, uint8_t  v2, uint64_t* flags);
    extern "C" uint16_t x64e_asm_sub16(uint16_t v1, uint16_t v2, uint64_t* flags);
    extern "C" uint32_t x64e_asm_sub32(uint32_t v1, uint32_t v2, uint64_t* flags);
    extern "C" uint64_t x64e_asm_sub64(uint64_t v1, uint64_t v2, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_add8(uint8_t  v1, uint8_t  v2, uint64_t* flags);
    extern "C" uint16_t x64e_asm_add16(uint16_t v1, uint16_t v2, uint64_t* flags);
    extern "C" uint32_t x64e_asm_add32(uint32_t v1, uint32_t v2, uint64_t* flags);
    extern "C" uint64_t x64e_asm_add64(uint64_t v1, uint64_t v2, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_inc8(uint8_t  v1, uint64_t* flags);
    extern "C" uint16_t x64e_asm_inc16(uint16_t v1, uint64_t* flags);
    extern "C" uint32_t x64e_asm_inc32(uint32_t v1, uint64_t* flags);
    extern "C" uint64_t x64e_asm_inc64(uint64_t v1, uint64_t* flags);

    extern "C" uint8_t  x64e_asm_dec8(uint8_t  v1, uint64_t* flags);
    extern "C" uint16_t x64e_asm_dec16(uint16_t v1, uint64_t* flags);
    extern "C" uint32_t x64e_asm_dec32(uint32_t v1, uint64_t* flags);
    extern "C" uint64_t x64e_asm_dec64(uint64_t v1, uint64_t* flags);

    extern "C" void x64e_asm_mul8(uint16_t* ax, uint8_t* al, uint8_t src, uint64_t* flags);
    extern "C" void x64e_asm_mul16(uint16_t* dx, uint16_t* ax, uint16_t src, uint64_t* flags);
    extern "C" void x64e_asm_mul32(uint32_t* edx, uint32_t* eax, uint32_t src, uint64_t* flags);
    extern "C" void x64e_asm_mul64(uint64_t* rdx, uint64_t* rax, uint64_t src, uint64_t* flags);

    extern "C" void x64e_asm_imul8(uint16_t* ax, uint8_t* al, uint8_t src, uint64_t* flags);
    extern "C" void x64e_asm_imul16(uint16_t* dx, uint16_t* ax, uint16_t src, uint64_t* flags);
    extern "C" void x64e_asm_imul32(uint32_t* edx, uint32_t* eax, uint32_t src, uint64_t* flags);
    extern "C" void x64e_asm_imul64(uint64_t* rdx, uint64_t* rax, uint64_t src, uint64_t* flags);

    extern "C" void x64e_asm_imuln16(uint16_t* dst, uint16_t src, uint64_t* flags);
    extern "C" void x64e_asm_imuln32(uint32_t* dst, uint32_t src, uint64_t* flags);
    extern "C" void x64e_asm_imuln64(uint64_t* dst, uint64_t src, uint64_t* flags);

    extern "C" void x64e_asm_bt16(uint16_t v1, uint16_t v2, uint64_t* flags);
    extern "C" void x64e_asm_bt32(uint32_t v1, uint32_t v2, uint64_t* flags);
    extern "C" void x64e_asm_bt64(uint64_t v1, uint64_t v2, uint64_t* flags);

    [[msvc::noinline]] void x64e_init_core(x64e_core* core, bool allow_read, bool allow_write);

    [[msvc::noinline]] void x64e_prepare_core(x64e_core* core, void* entry, x64e_read_callback_t read_callback, x64e_write_callback_t write_callback, x64e_debug_callback_t debug_callback);
    
    [[msvc::noinline]] void x64e_destroy_core(x64e_core* core);

    [[msvc::noinline]] void x64e_print_registers(x64e_core* core);

    [[msvc::noinline]] void x64e_do_step(x64e_core* core);

    [[msvc::noinline]] void x64e_do_push64(x64e_core* core, uint64_t value);
    [[msvc::noinline]] void x64e_do_push32(x64e_core* core, uint32_t value);
    [[msvc::noinline]] void x64e_do_push16(x64e_core* core, uint16_t value);

    [[msvc::noinline]] uint64_t x64e_do_pop64(x64e_core* core);
    [[msvc::noinline]] uint32_t x64e_do_pop32(x64e_core* core);
    [[msvc::noinline]] uint16_t x64e_do_pop16(x64e_core* core);

    [[msvc::noinline]] void x64e_do_branch(x64e_core* core, uint64_t address, bool is_call);
    [[msvc::noinline]] void x64e_do_return(x64e_core* core);

    [[msvc::noinline]] bool x64e_do_read64(x64e_core* core, uint64_t address, x64e_val* val);
    [[msvc::noinline]] bool x64e_do_read32(x64e_core* core, uint64_t address, x64e_val* val);
    [[msvc::noinline]] bool x64e_do_read16(x64e_core* core, uint64_t address, x64e_val* val);
    [[msvc::noinline]] bool x64e_do_read8(x64e_core* core, uint64_t address, x64e_val* val);

    [[msvc::noinline]] bool x64e_do_write64(x64e_core* core, uint64_t address, x64e_val const* val);
    [[msvc::noinline]] bool x64e_do_write32(x64e_core* core, uint64_t address, x64e_val const* val);
    [[msvc::noinline]] bool x64e_do_write16(x64e_core* core, uint64_t address, x64e_val const* val);
    [[msvc::noinline]] bool x64e_do_write8(x64e_core* core, uint64_t address, x64e_val const* val);

    [[msvc::noinline]] bool x64e_sign_extend(x64e_core* core, uint16_t cur_size, uint16_t new_size, x64e_val* val);
    [[msvc::noinline]] bool x64e_zero_extend(x64e_core* core, uint16_t cur_size, uint16_t new_size, x64e_val* val);

    [[msvc::noinline]] bool x64e_is_operand_high_register(x64e_core* core, uint8_t operand);

    [[msvc::noinline]] bool x64e_get_memory_operand_segment_reg(x64e_core* core, uint8_t operand, x64e_reg** reg);
    [[msvc::noinline]] bool x64e_get_register_operand_reg(x64e_core* core, uint8_t operand, x64e_reg** reg);
    [[msvc::noinline]] bool x64e_get_memory_operand_reg(x64e_core* core, uint8_t operand, bool base, x64e_reg** reg);
    [[msvc::noinline]] bool x64e_get_operand_effective_address(x64e_core* core, uint8_t operand, x64e_val* val);

    [[msvc::noinline]] bool x64e_get_operand_value(x64e_core* core, uint8_t operand, uint16_t size, x64e_val* val);
    [[msvc::noinline]] bool x64e_set_operand_value(x64e_core* core, uint8_t operand, uint16_t size, x64e_val const* val);

    [[msvc::noinline]] bool x64e_emulate_bswap(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_bt(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_btc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_btr(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_bts(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_call(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_cbw_cwde_cdqe(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_clc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_cld(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_stc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_std(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_cmc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_cmovcc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_cmp(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_cpuid(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_adc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_add(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_xadd(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_sub(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_inc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_dec(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_mul(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_imul(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_div(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_idiv(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_rcl(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_rcr(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_rol(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_ror(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_sar(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_shl(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_shr(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_xor(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_and(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_andn(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_neg(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_not(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_or(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_int3(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_jmp(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_jcc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_lea(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_loopcc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_lzcnt(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_tzcnt(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_mov(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_movzx(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_movsx(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_nop(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_pop(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_popfq(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_push(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_pushfq(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_ret(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_setcc(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_stos(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_syscall(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_test(x64e_core* core);
    [[msvc::noinline]] bool x64e_emulate_xchg(x64e_core* core);

    [[msvc::noinline]] bool x64e_run_core(x64e_core* core, void* user_data);
}