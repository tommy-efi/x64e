#include "utility/x64e.h"

utility::x64e_core g_x64e_core{};

static bool global_read_callback(utility::x64e_core* core, uint64_t address, void* value, uint16_t size, bool is_stack, bool is_dirty) {

    if (is_stack) {

        if (!is_dirty) {

            if (size == 64) {

                *reinterpret_cast<uint64_t*>(value) = {};

                // this is an override for fallback..
                // we don't read the real stack..
                // so we return true

                return true;
            }
        }

        // stack needs to be handled by fallback..
        // because stack isn't in the game..
        // so return false..

        return false;
    }

    // put your kernel/hv read here instead

    memcpy(value, reinterpret_cast<void*>(address), static_cast<size_t>(size));

    return true;
}

static bool global_write_callback(utility::x64e_core* core, uint64_t address, void const* value, uint16_t size, bool is_stack, bool is_dirty) {

    if (is_stack) {

        // stack needs to be handled by fallback..
        // because stack isn't in the game..
        // so return false..

        return false;
    }

    // put your kernel/hv write here instead

    memcpy(reinterpret_cast<void*>(address), value, static_cast<size_t>(size));

    return true;
}

struct basic_encryption_data {
    uint64_t m_result{};
    int8_t m_call_depth{};
};

static utility::x64e_debug_result dummy_decryption_debug_callback(utility::x64e_core* core, void* user_data) {

    // manually analyize the assembly so you know where you need to stop emulating
    // in this case, checking last ret will do. 

    auto* basic_enc_data = reinterpret_cast<basic_encryption_data*>(user_data);

    if (core->m_instruction.mnemonic == ZYDIS_MNEMONIC_CALL) {

        basic_enc_data->m_call_depth++;
    }

    if (core->m_instruction.mnemonic == ZYDIS_MNEMONIC_RET) {

        if (basic_enc_data->m_call_depth-- == 0) {

            // result for this function we're emulating is likely
            // in rax since we're just calling from start to finish
            // so just set result to rax value

            basic_enc_data->m_result = static_cast<uint64_t>(core->m_rax.m_val.m_dword.m_u);

            return utility::x64e_debug_result::exit;
        }
    }

    return utility::x64e_debug_result::step;
}

[[msvc::noinline]] uint32_t dummy_decryption(uint64_t some_key) {

    uint32_t part1 = static_cast<uint32_t>(some_key >> 32);
    uint32_t part2 = static_cast<uint32_t>(some_key & 0xFFFFFFFF);

    uint32_t result = (part1 ^ part2) ^ 0xA5A5A5A5;
    result += ((part1 >> 3) | (part2 << 5));
    result ^= (result << 13); 

    return result;
}

int main() {

    printf("Non-Emulated Result:%X\n", dummy_decryption(0x12345678ABCDEFFF));   

	utility::x64e_init_core(&g_x64e_core, true, false);

    utility::x64e_prepare_core(&g_x64e_core, dummy_decryption, global_read_callback, global_write_callback, dummy_decryption_debug_callback);

    basic_encryption_data basic_enc_data{};

    // key argument we used above
    g_x64e_core.m_rcx.m_val.m_qword.m_u = 0x12345678ABCDEFFF; 

    if (!utility::x64e_run_core(&g_x64e_core, &basic_enc_data)) {

        printf("Unable to run core for dummy decryption\n");
    }

    else {

        utility::x64e_print_registers(&g_x64e_core);

        printf("Emulated Result: %X\n", static_cast<uint32_t>(basic_enc_data.m_result));
    }
    
    return 0;
}

