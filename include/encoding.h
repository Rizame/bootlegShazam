//
// Created by obbte on 06/26/25.
//

#ifndef ENCODING_H
#define ENCODING_H
#include <bitset>
#include <cstdint>

#include "wavProcessing.h"

namespace encoding {
    inline uint32_t encode(int anchor_bin, int target_bin, float delta_time) {
        uint32_t fingerprint = (static_cast<uint32_t>(anchor_bin) << 23) |
                               (static_cast<uint32_t>(target_bin) << 14) |
                               static_cast<uint32_t>(delta_time);

        std::bitset<32> test = fingerprint;

        std::cout << "fingerprint: " << test << std::endl;

        return fingerprint;
    };
}


#endif //ENCODING_H
