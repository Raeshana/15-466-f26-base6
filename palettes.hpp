#pragma once

#include <glm/glm.hpp>
#include <array>

// palettes
inline std::array< glm::u8vec4, 4 > player_palette = {
   glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1- transparent
    glm::u8vec4(0x29, 0x2c, 0x60, 0xff), // colour 2- 292c60
    glm::u8vec4(0x31, 0x6a, 0xcb, 0xff), // colour 3- 316acb
    glm::u8vec4(0x72, 0xf1, 0xf0, 0xff), // colour 4- 72f1f0
};

inline std::array< glm::u8vec4, 4 > default_palette = {
    glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1- transparent
    glm::u8vec4(0xff, 0xff, 0xff, 0xff), // colour 2- white
    glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 3- black
    glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 4
};

inline std::array< glm::u8vec4, 4 > background_palette = {
    glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1- transparent
    glm::u8vec4(0x29, 0x2c, 0x60, 0xff), // colour 2- 292c60
    glm::u8vec4(0x31, 0x6a, 0xcb, 0xff), // colour 3- 316acb
    glm::u8vec4(0x72, 0xf1, 0xf0, 0xff), // colour 4- 72f1f0
};

inline std::array< glm::u8vec4, 4 > cloud_palette = {
    glm::u8vec4(0xb5, 0xbd, 0xed, 0xff), // colour 1- b5bded
    glm::u8vec4(0xab, 0xc6, 0xea, 0xff), // colour 2- abc6ea
    glm::u8vec4(0xc4, 0xc5, 0xce, 0xff), // colour 3- c4c5ce
    glm::u8vec4(0xde, 0xe0, 0xeb, 0xff), // colour 4- dee0eb
};