#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

// header file for asset pipeline
// see google doc: https://docs.google.com/document/d/1TxCWwI5_XaqrjB0YpM-9gQqsMgI7udDwoV-GPZzUoGU/edit?usp=sharing
#include "assets.hpp" 

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

// #include <random>

PlayMode::PlayMode() {
	//use sprite 32 as a "player":
	ppu.tile_table[32] = PLAYER_TILE;

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff), // black
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0x00, 0x00, 0xff), // red
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	const PPU466::Tile RED_TILE = PPU466::Tile{
		.bit0 = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
		.bit1 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};

	const PPU466::Tile BLACK_TILE = PPU466::Tile{
		.bit0 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		.bit1 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1
        glm::u8vec4(0xff, 0x00, 0x00, 0xff), // colour 2
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 3
        glm::u8vec4(0x00, 0x00, 0x00, 0xff), // colour 4
	};
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_EVENT_KEY_DOWN) {
		if (evt.key.key == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_EVENT_KEY_UP) {
		if (evt.key.key == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float dt) {

	// from in-class notes:
	// Refernced https://gafferongames.com/post/integration_basics/
	// And also https://lazyfoo.net/tutorials/SDL/44_frame_independent_movement/index.php
	
	// get acc. from inputs
	if (left.pressed) acceleration.x = -speed;
	else if (right.pressed) acceleration.x = speed;
	else acceleration.x = 0.0f;

	// calc velocity from acc
	velocity.x += dt * acceleration.x;
	velocity.y += dt * acceleration.y;

	// calc. position from velocity
	position.x += dt * velocity.x;
	position.y += dt * velocity.y;

	// check player collision with actual floor
	uint32_t background_idx = (uint32_t)position.x + ppu.BackgroundWidth * (uint32_t)position.y;
	if (ppu.background[background_idx] == int16_t(0b11 << 8)) { 
		std::cout << "on red tile";
	}
	// (info >> 8) & 0x07 //extract palette index bits

	// floor
	if (position.y < 0 && velocity.y < 0.0f) {
		velocity.y = 0.8f * std::abs(velocity.y); // https://cplusplus.com/reference/cmath/abs/
		position.y = 0.0f;
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//tilemap gets recomputed every frame as some weird plasma thing:
	//NOTE: don't do this in your game! actually make a map or something :-)
	// for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
	// 	for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
	// 		//TODO: make weird plasma thing
	// 		ppu.background[x+PPU466::BackgroundWidth*y] = ((x+y)%16);
	// 	}
	// }

	//background scroll:
	ppu.background_position.x = int32_t(-0.5f * position.x);
	ppu.background_position.y = int32_t(-0.5f * position.y);

	//player sprite:
	ppu.sprites[0].x = int8_t(position.x);
	ppu.sprites[0].y = int8_t(position.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	//--- actually draw ---
	ppu.draw(drawable_size);
}