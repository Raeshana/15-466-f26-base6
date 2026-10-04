#include <iostream>
#include <cassert>
#include <fstream>
#include "load_save_png.hpp"

int main(int argc, char **argv) {
	// load the png
    glm::uvec2 size;
    std::vector< glm::u8vec4 > png;
    load_png("sprites/game-1_sprites.png", &size, &png, LowerLeftOrigin);
    std::cout << "png loaded (it's " << size.x << " by " << size.y << ")!" << std::endl;

    // clip out bottom left 8x8 tile (player sprite – smiley face from class)
    std::vector< glm::u8vec4 > palette{
        glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1
        glm::u8vec4(0xff, 0x00, 0x00, 0xff), // colour 2
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 3
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 4
    };

    // for every pixel of the sprite, look it up in the palette
    // and write out into “it’s a tile” format

    // tile format:
    /* PLAYER_TILE = PPU466::Tile{
        .bit0 = { … },
        .bit1 = { … },
    } */

    // open assets.hpp as a write file
    std::ofstream assets_hpp("assets.hpp");
    if (!assets_hpp) {
        std::cerr << "Failed to open assets.hpp\n";
        return 1;
    }

    assets_hpp <<"const PPU466::Tile PLAYER_TILE = PPU466::Tile{\n";
    assets_hpp << "\t.bit0 = {\n";

    // does not need to be efficient; source code run only once on programmer end
    // when sprites print to assets.hpp, they will look “upside down”
    
    uint64_t bit1 = 0x00000000; // store bit1
    for (uint32_t y=0; y<8; y++) { // 8x8 sprite
        assets_hpp << "\t\t0b"; 
        for (uint32_t x=0; x<8; x++) {
            glm::u8vec4 col = png[y * size.x + x]; // get pixel from png

            // see if col exists in the palette
            size_t found = palette.size();
            
            for (size_t i=0; i < palette.size(); i++) {
                if (col == palette[i]) {
                    found = i; // found  = the index of col in palette
                    break;
                }
            }
            if (found == palette.size()) { // no: write 0
                assets_hpp << "0";
            }
            else { // yes: write bit0 of found, store bit1 of found
                assets_hpp << (found & 1); // bit0 of found
                uint32_t bit1_found = (found >> 1) & 1; // bit1 of found
                bit1 = (bit1 << 1) | bit1_found; // pack bit1 with bit1_found
            }
        } 
        assets_hpp << ",\n"; // close line x of bit0
        // bit1 = bit1
    }
    assets_hpp << "},\n"; // close bit0
    
    // write bit1
    assets_hpp << "\t.bit1 = {\n";
    for (uint64_t i=0; i < 8; i++){
        assets_hpp << "\t\t0b";
        for (uint32_t j=0; j < 8; j++){
            assets_hpp << ((bit1 >> ((8-i)*8 + j)) & 1);
        }
        assets_hpp << ",\n"; // close line i of bit1
    }

    assets_hpp << "},\n"; // close bit1
    assets_hpp << "};\n"; // close tile
}
