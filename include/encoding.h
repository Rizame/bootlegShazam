//
// Created by obbte on 06/26/25.
//

#ifndef ENCODING_H
#define ENCODING_H
#include <cstdint>
constexpr uint32_t ANCHOR_MASK = 0x1FF; // 9 bits
constexpr uint32_t TARGET_MASK = 0x1FF; // 9 bits
constexpr uint32_t DELTA_MASK = 0x3FFF; // 14 bits

namespace encoding {
    uint32_t encode(int anchor_bin, int target_bin, float delta_time);
}


#endif //ENCODING_H
