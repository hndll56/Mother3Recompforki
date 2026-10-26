# Modding MOTHER 3 Recomp (Linux build)

Two mechanisms, both verified on the Linux build (2026-10-26). Neither needs a rebuild once the
executable exists, except adding *new* code hooks.

## 1. Data mods — `GBARECOMP_ROM_PATCH=<file.ips>`
Applies a classic **IPS** or **IPS32** patch to the in-memory ROM **after** the stock ROM hash passed
(the base ROM is still verified; the file on disk is never modified). Use IPS32 for offsets >16 MiB —
MOTHER 3's ROM is 32 MiB. A malformed patch or any write past the ROM aborts startup (all-or-nothing).
BPS is not supported.

    GBARECOMP_ROM_PATCH=my_text.ips ./build/Mother3RecompEN --bios ... --rom ... variants/mother3en/game.toml

**Limit:** statically recompiled code is not regenerated. Patch *data* (text, tables, stats, graphics).
Patched code bytes are ignored by native functions → use a code hook (below).
Requires the engine patch `docs/gbarecomp-rom-ips.patch` (`git -C gbarecomp apply ../docs/gbarecomp-rom-ips.patch`);
`gbarecomp` is a submodule, so this change is not part of the superproject commit.

## 2. Code mods — native function hooks
1. Declare the entry in `variants/mother3en/game.toml`:
       [[mod_function_hook]]
       addr = 0x080816B8
       mode = "thumb"
2. Re-run `gba_recompile` (2 s) and rebuild (only shards containing hooked functions recompile).
3. Register a callback in `src/mods/*.cpp` (`gba_mod_register_function_entry_plugin`). It must set the
   full CPU state including R15 (see `mother3_rates_plugin.cpp: guest_return`). Return 0 to decline.

Included example `src/mods/mother3_rates_plugin.cpp` (hooks off by default → stock behavior):

    M3_EXP_MULT=2 M3_DP_MULT=3 ./build/Mother3RecompEN ...   # x2 EXP, x3 DP (0..1000, saturating)
    M3_HOOK_TRACE=1 ...                                      # prints how many times seed_rng ran

## Verified / not verified
- Verified: hook fires in the real game (seed_rng seen once at boot); with mods off, frame 600 is
  byte-identical to the pre-mod build; patched ROM bytes are visible to the guest through the bus
  (0x08800000 reads e43fbc21 stock, efbeadde patched); unit tests `tests/run_mod_rates_test.sh`.
- **Not verified in-game:** the EXP/DP multiplier inside a real battle (only unit-tested with a fake CPU/bus);
  a visible graphics/text IPS (no text offsets located yet); multiple patches stacked.
