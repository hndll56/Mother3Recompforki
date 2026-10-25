# Linux x86-64 bring-up notes (MOTHER 3 EN, verified 2026-10-25)

Environment: Ubuntu 24.04 container, gcc 13.3, cmake 3.28, ninja, 2 cores / 8 GB.

## Deviations from upstream (all required, all small — see linux-bringup.patch)
1. ROM hash: repo pins SHA-1 306fb887… (a different EN patch build). The supplied
   "Mother 3 (Eng. Translation 1.1)" is b15c0c74…; the hash was swapped in
   game.toml, config/mother3en_en.toml, symbols/mother3_jpn.toml and CMakeLists.txt
   (+ CRC32 0xEDE46930). Symbols come from the JP decomp; this build relies on
   self-heal for any moved code.
2. src/main.cpp: Emerald-only hooks (gf_ReadFlash*, emerald::*_extended_view) are
   under `#ifdef GBARECOMP_EMERALD`; they do not link for MOTHER 3.
3. `gbarecomp/external/arm-recomp-core` could not be cloned (submodule URL resolves to
   agray0425/arm-recomp-core, not accessible); the pinned commit 763b922 was fetched
   from mstan/arm-recomp-core instead.
4. BIOS: the runtime needs a 16 KB BIOS file even in HLE mode. Nintendo's BIOS was not
   available; the MIT-licensed Cult-of-GBA/BIOS replacement was used with
   `--bios-hle --bios-sha1 <its sha1>`. Use your own dump (sha1 300c20df…) for accuracy.
5. Build host lacked libsdl2-dev / GL dev files: SDL2 2.30.9 built from source into a
   prefix; a stub GL dev dir (libGL.so symlink + one-line GL/gl.h) satisfied FindOpenGL.
   On a normal Arch install: `pacman -S sdl2 libglvnd cmake ninja` replaces all of this.

## Build
    git clone --recurse-submodules <repo>   # then fix item 3 if it fails
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
    cd variants/mother3en && ../../build/gbarecomp_build/gba_recompile --rom roms/mother3_en.gba \
      --config game.toml --config symbols/mother3_jpn.toml --symbols symbols/imported_symbols.tsv \
      --data-symbols symbols/imported_data_symbols.tsv --out generated --max-functions 65536
    cmake --build build --target Mother3RecompEN -j2     # ~10 min on 2 cores

## Run
    ./build/Mother3RecompEN --bios <bios.bin> --rom variants/mother3en/roms/mother3_en.gba variants/mother3en/game.toml
Headless test: tests/linux_smoke/run.sh <trace> <frames> <out.png> (needs xvfb).
