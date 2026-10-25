#!/bin/bash
# usage: run.sh trace frames out.png  — headless run with open-source BIOS HLE + input trace
cd "$(cd "$(dirname "$0")/../.." && pwd)
export LD_LIBRARY_PATH=/home/claude/sdl2inst/lib SDL_AUDIODRIVER=dummy GBARECOMP_INPUT_REPLAY=$1
timeout 900 xvfb-run -a ./build/Mother3RecompEN --bios /home/claude/cogbios/bios.bin --bios-sha1 2e41f9d30ecdf1c4540edf89ac16f49bf327edb7 --bios-hle --rom variants/mother3en/roms/mother3_en.gba --frames $2 --dump-png $3 variants/mother3en/game.toml 2>&1 | grep -E "final_pc|fatal|unmapped=[1-9]" | cut -c1-120
