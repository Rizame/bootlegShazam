//
// Created by obbte on 06/27/25.
//
#include "../include/encoding.h"

#include <bitset>
#include <cmath>
#include <iostream>

uint32_t encoding::encode(int anchor_bin, int target_bin, float delta_time) {
    uint32_t anchor = static_cast<uint32_t>(anchor_bin) & ANCHOR_MASK;
    uint32_t target = static_cast<uint32_t>(target_bin) & TARGET_MASK;
    uint32_t delta = static_cast<uint32_t>(std::lround(delta_time / 0.001f));
    delta = delta & DELTA_MASK;

    uint32_t fingerprint = anchor << 23 | target << 14 | delta;

    return fingerprint;
}
