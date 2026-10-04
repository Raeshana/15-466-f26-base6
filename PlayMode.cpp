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
	// ppu.tile_table[32] = PLAYER_TILE;

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
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
	// float Tick = 1.0f / 60.0f;
	
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

	// floor
	if (position.y < 0.0f && velocity.y < 0.0f) {
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

	//some other misc sprites:
	// for (uint32_t i = 1; i < 63; ++i) {
	// 	float amt = (i + 2.0f * background_fade) / 62.0f;
	// 	ppu.sprites[i].x = int8_t(0.5f * float(PPU466::ScreenWidth) + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * float(PPU466::ScreenWidth));
	// 	ppu.sprites[i].y = int8_t(0.5f * float(PPU466::ScreenHeight) + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * float(PPU466::ScreenWidth));
	// 	ppu.sprites[i].index = 32;
	// 	ppu.sprites[i].attributes = 6;
	// 	if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	// }

	//--- actually draw ---
	ppu.draw(drawable_size);
}