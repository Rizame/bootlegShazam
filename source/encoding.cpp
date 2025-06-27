//
// Created by obbte on 06/27/25.
//
#include "../include/encoding.h"

#include <bitset>
#include <cmath>
#include <iostream>

uint32_t encoding::encode(int anchor_bin, int target_bin, float delta_time) {
    auto masked_time = static_cast<uint32_t>(std::lround(delta_time / 0.001f));

    if (masked_time > DELTA_MASK) masked_time = DELTA_MASK;

    uint32_t fingerprint = (static_cast<uint32_t>(anchor_bin) & ANCHOR_MASK) << 23 |
                           (static_cast<uint32_t>(target_bin) & TARGET_MASK) << 14 |
                           masked_time & DELTA_MASK;

    return fingerprint;
}
