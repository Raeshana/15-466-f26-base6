#include <iostream>
#include <cassert>
#include <fstream>
#include "load_save_png.hpp"
#include <string>

std::ofstream assets_hpp("assets.hpp");
glm::uvec2 size;
std::vector< glm::u8vec4 > png;


bool tile_from_png(std::string tile_name, uint32_t x_offset, uint32_t y_offset, std::vector< glm::u8vec4 > palette){
    // clip out bottom left 8x8 tile (player sprite – smiley face from class)
    assets_hpp <<"const PPU466::Tile " << tile_name << "= PPU466::Tile{\n";
    assets_hpp << "\t.bit0 = {\n";

    // does not need to be efficient; source code run only once on programmer end
    // when sprites print to assets.hpp, they will look “upside down”
    
    uint64_t bit1 = 0x00000000; // store bit1
    for (uint32_t y=y_offset; y<y_offset+8; y++) { // 8x8 sprite
        assets_hpp << "\t\t0b"; 
        for (uint32_t x=x_offset; x<x_offset+8; x++) {
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
    assets_hpp << "\n"; // skip line for clarity
    return 0;
}

bool load_png(std::string file_name){
    // load the png
    load_png(file_name, &size, &png, LowerLeftOrigin);
    std::cout << "png loaded (it's " << size.x << " by " << size.y << ")!" << std::endl;
    return 0;
}

int main(int argc, char **argv) {

    // palettes
    std::vector< glm::u8vec4 > player_palette{
        glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1
        glm::u8vec4(0xff, 0x00, 0x00, 0xff), // colour 2
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 3
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 4
    };

    std::vector< glm::u8vec4 > background_palette{
        glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1- transparent
        glm::u8vec4(0xff, 0xff, 0xff, 0xff), // colour 2- white
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 3
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 4
    };

    // open assets.hpp as a write file
    if (!assets_hpp) {
        std::cerr << "Failed to open assets.hpp\n";
        return 1;
    }
	
    load_png("sprites/game-1_sprites.png");
    tile_from_png("PLAYER_TILE", 0, 0, player_palette);

    load_png("sprites/sky_tileset.png");
    tile_from_png("STARS_TILE", 0, 0, background_palette);
    tile_from_png("STARS_TILE_2", 8, 0, background_palette);
}
