// Self-check for gbarecomp/src/runtime/rom_ips.h
#include <cassert>
#include <cstdio>
#include "../gbarecomp/src/runtime/rom_ips.h"
static std::vector<uint8_t> ips(std::initializer_list<uint8_t> body) {
    std::vector<uint8_t> p{'P','A','T','C','H'}; p.insert(p.end(), body); p.insert(p.end(), {'E','O','F'}); return p;
}
int main() {
    std::vector<uint8_t> rom(16, 0); std::string err; size_t n = 0;
    assert(rom_ips_apply(rom, ips({0,0,2, 0,2, 0xAA,0xBB, 0,0,8, 0,0, 0,3, 0x7F}), &n, &err));  // literal + RLE
    assert(n == 2 && rom[2]==0xAA && rom[3]==0xBB && rom[8]==0x7F && rom[10]==0x7F && rom[11]==0);
    auto before = rom;
    assert(!rom_ips_apply(rom, ips({0,0,15, 0,2, 1,2}), &n, &err) && rom == before);   // past end: untouched
    assert(!rom_ips_apply(rom, ips({0,0,1, 0,2, 1,2, 0,0,15, 0,2, 1,2}), &n, &err) && rom == before); // all-or-nothing
    assert(!rom_ips_apply(rom, {'N','O','P','E','1','2','3','4'}, &n, &err));          // bad magic
    std::vector<uint8_t> trunc{'P','A','T','C','H',0,0,1,0,5,1};                       // truncated data
    assert(!rom_ips_apply(rom, trunc, &n, &err) && rom == before);
    std::vector<uint8_t> big(0x2000000, 0);                                           // 32 MiB ROM, IPS32 offset > 16 MiB
    std::vector<uint8_t> p32{'I','P','S','3','2', 0x01,0x80,0,0, 0,2, 9,8, 'E','E','O','F'};
    assert(rom_ips_apply(big, p32, &n, &err) && big[0x01800000]==9 && big[0x01800001]==8);
    std::vector<uint8_t> p32bad{'I','P','S','3','2', 0x02,0,0,0, 0,1, 1, 'E','E','O','F'};  // offset == size
    assert(!rom_ips_apply(big, p32bad, &n, &err));
    std::puts("rom_ips_test OK");
}
