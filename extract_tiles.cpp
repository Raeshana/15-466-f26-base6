#include <iostream>
#include <cassert>
#include <fstream>
#include "load_save_png.hpp"
#include <string>
#include "palettes.hpp"

std::ofstream assets_hpp("assets.hpp");
glm::uvec2 size;
std::vector< glm::u8vec4 > png;


bool tile_from_png(std::string tile_name, uint32_t x_offset, uint32_t y_offset, std::array< glm::u8vec4, 4 > palette){
    // clip out bottom left 8x8 tile (player sprite – smiley face from class)
    assets_hpp <<"const PPU466::Tile " << tile_name << "= PPU466::Tile{\n";
    assets_hpp << "\t.bit0 = {\n";

    // does not need to be efficient; source code run only once on programmer end
    // when sprites print to assets.hpp, they will look “upside down”
    
    for (uint32_t y=y_offset; y<y_offset+8; y++) { // 8x8 sprite
        assets_hpp << "\t\t0b"; 
        for (int32_t x = int32_t(x_offset) + 7; x >= int32_t(x_offset); x--) {
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
            else { // yes: write bit0 of found
                assets_hpp << (found & 1); // bit0 of found
            }
        } 
        assets_hpp << ",\n"; // close line x of bit0
    }
    assets_hpp << "},\n"; // close bit0

    assets_hpp << "\t.bit1 = {\n";

    for (uint32_t y=y_offset; y<y_offset+8; y++) { // 8x8 sprite
        assets_hpp << "\t\t0b"; 
        for (int32_t x = int32_t(x_offset) + 7; x >= int32_t(x_offset); x--)  {
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
            else { // yes: write  bit1 of found
                uint32_t bit1_found = (found >> 1) & 1; // bit1 of found
                assets_hpp << bit1_found; // pack bit1 with bit1_found
            }
        } 
        assets_hpp << ",\n"; // close line x of bit1
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
    // open assets.hpp as a write file
    if (!assets_hpp) {
        std::cerr << "Failed to open assets.hpp\n";
        return 1;
    }
	
    load_png("sprites/game-1_sprites.png");
    // tile_from_png("PLAYER_TILE", 0, 0, player_palette);

    load_png("sprites/sky_tileset.png");
    // prio 0 stars
    tile_from_png("STARS_TILE_1", 0*8, 0, default_palette);
    tile_from_png("STARS_TILE_2", 1*8, 0, default_palette);
    tile_from_png("STARS_TILE_3", 2*8, 0, default_palette);
    // background stars
    tile_from_png("STARS_TILE_4", 3*8, 0, background_palette);
    // player star
    tile_from_png("PLAYER_TILE_1", 0, 8, player_palette);
    tile_from_png("PLAYER_TILE_2", 8, 8, player_palette);
    tile_from_png("PLAYER_TILE_3", 0, 16, player_palette);
    tile_from_png("PLAYER_TILE_4", 8, 16, player_palette);
    // cloud tile
    tile_from_png("CLOUD", 16, 8, cloud_palette);
}
