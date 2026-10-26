// Self-check for src/mods/mother3_rates_plugin.cpp (fake CPU + fake bus; no ROM needed).
// Run: M3_EXP_MULT=3 M3_DP_MULT=1000 ./mod_rates_test   (see tests/run_mod_rates_test.sh)
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <map>
static std::map<uint32_t, uint32_t> fake_mem;
#include "mod_function_hooks.h"
#include "runtime_arm.h"
uint32_t bus_read_u32(uint32_t a) { return fake_mem.at(a); }  // stub: real bus lives in the runtime
#include "../src/mods/mother3_rates_plugin.cpp"

int main() {
    ArmCpuState cpu{};
    fake_mem[0x02000000u + 0x118] = 100;  fake_mem[0x02000000u + 0x12C] = 0x10000000u;
    cpu.R[0] = 0x02000000u; cpu.R[14] = 0x08001235u;  // thumb caller
    assert(gba_mod_function_entry(0x080816B8u, 1, &cpu) == 1);       // EXP hook handled
    assert(cpu.R[0] == 300 && cpu.R[15] == 0x08001234u && (cpu.cpsr & 0x20));
    cpu.R[0] = 0x02000000u; cpu.R[14] = 0x08001234u;                  // ARM caller
    assert(gba_mod_function_entry(0x080816C4u, 1, &cpu) == 1);       // DP hook, x1000 saturates
    assert(cpu.R[0] == 0x7FFFFFFFu && cpu.R[15] == 0x08001234u && !(cpu.cpsr & 0x20));
    assert(gba_mod_function_entry(0x080008E0u, 1, &cpu) == 0);       // trace hook disabled
    std::puts("mod_rates_test OK");
}
