// mother3_rates_plugin.cpp — example native mod via [[mod_function_hook]].
//
// Scales battle EXP / DP rewards by replacing the two tiny Monster getters
// (decomp: experience__C7Monster = *(this+0x118), money__C7Monster = *(this+0x12C)).
// Configure without rebuilding:  M3_EXP_MULT=2 M3_DP_MULT=3 ./Mother3RecompEN ...
// M3_HOOK_TRACE=1 counts seed_rng calls at boot and prints the total at exit (proves hooks fire).
#include <cstdio>
#include <cstdlib>

#include "mod_function_hooks.h"
#include "mod_runtime.h"
#include "runtime_arm.h"

namespace {

uint32_t g_exp_mult = 1, g_dp_mult = 1;

uint32_t env_mult(const char* name) {
    const char* v = std::getenv(name);
    if (!v || !*v) return 1;
    char* end = nullptr;
    unsigned long n = std::strtoul(v, &end, 10);
    return (*end || n > 1000) ? 1 : static_cast<uint32_t>(n);  // bad value -> stock behavior
}

// Return to the guest caller exactly as the original `bx lr` would.
void guest_return(ArmCpuState* cpu) {
    const uint32_t lr = cpu->R[14];
    cpu->cpsr = (lr & 1u) ? (cpu->cpsr | (1u << 5)) : (cpu->cpsr & ~(1u << 5));
    cpu->R[15] = lr & ~((lr & 1u) ? 1u : 3u);
}

int scaled_getter(ArmCpuState* cpu, uint32_t offset, uint32_t mult) {
    uint64_t v = static_cast<uint64_t>(bus_read_u32(cpu->R[0] + offset)) * mult;
    cpu->R[0] = v > 0x7FFFFFFFu ? 0x7FFFFFFFu : static_cast<uint32_t>(v);  // saturate
    guest_return(cpu);
    return 1;
}

int hook_exp(uint32_t, int, ArmCpuState* cpu) { return scaled_getter(cpu, 0x118, g_exp_mult); }
int hook_dp(uint32_t, int, ArmCpuState* cpu)  { return scaled_getter(cpu, 0x12C, g_dp_mult); }

uint64_t g_trace_calls = 0;  // game thread only
int hook_trace(uint32_t, int, ArmCpuState*) { ++g_trace_calls; return 0; }  // decline: original body runs
void print_hits() {
    std::fprintf(stderr, "[m3-mod] seed_rng_calls_seen=%llu\n",
                 static_cast<unsigned long long>(g_trace_calls));
}

}  // namespace

GBA_MOD_CONSTRUCTOR(register_mother3_rates) {
    g_exp_mult = env_mult("M3_EXP_MULT");
    g_dp_mult = env_mult("M3_DP_MULT");
    gba_mod_register_function_entry_plugin("mother3.rates.exp", 0x080816B8u, 1, hook_exp);
    gba_mod_register_function_entry_plugin("mother3.rates.dp", 0x080816C4u, 1, hook_dp);
    gba_mod_register_function_entry_plugin("mother3.trace.seed_rng", 0x080008E0u, 1, hook_trace);
    // Hooks stay disabled (zero overhead, stock behavior) unless asked for.
    if (g_exp_mult != 1) gba_mod_set_function_hook_enabled("mother3.rates.exp", 1);
    if (g_dp_mult != 1) gba_mod_set_function_hook_enabled("mother3.rates.dp", 1);
    if (const char* t = std::getenv("M3_HOOK_TRACE"); t && *t == '1') {
        gba_mod_set_function_hook_enabled("mother3.trace.seed_rng", 1);
        std::atexit(print_hits);
    }
}
