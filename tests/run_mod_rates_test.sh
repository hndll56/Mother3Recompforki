#!/bin/bash
set -e; cd "$(dirname "$0")/.."; G=gbarecomp
g++ -std=c++20 -I$G/src/armv4t -I$G/src/runtime -I$G/external/arm-recomp-core/profiles/armv4t_gba \
  tests/mod_rates_test.cpp $G/src/runtime/mod_function_hooks.cpp -o /tmp/mod_rates_test
M3_EXP_MULT=3 M3_DP_MULT=1000 /tmp/mod_rates_test
g++ -std=c++20 -Wall tests/rom_ips_test.cpp -o /tmp/rom_ips_test && /tmp/rom_ips_test
