#include <cmath>
#include <array>
#include <cstdint>

#include "DataClasses.h"

namespace COLORS {
    uint32_t toInt(const Color& c, uint8_t led)
    {
        return (led << 24) | (c.r << 16) | (c.g << 8) | c.b;
    }

    Color gamma(Color c) {
        return {GAMMA_TABLE[c.r], GAMMA_TABLE[c.g], GAMMA_TABLE[c.b]};
    }

    uint8_t gamma8(uint8_t n) {
        return GAMMA_TABLE[n];
    }

    Color gamma_scale(Color c, double scale) {
        return {
            GAMMA_TABLE[(uint8_t)ceil(c.r * scale)], 
            GAMMA_TABLE[(uint8_t)ceil(c.g * scale)], 
            GAMMA_TABLE[(uint8_t)ceil(c.b * scale)]
        };
    }
}