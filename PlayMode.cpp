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

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00), // colour 1
        glm::u8vec4(0xff, 0xff, 0x00, 0xff), // colour 2
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

	// calc velocity from acc
	velocity_px.y += dt * acceleration_px.y;

	if (left.pressed) velocity_px.x = -speed_px;
	else if (right.pressed) velocity_px.x = speed_px;
	else velocity_px.x = 0.0f;

	// calc. position from velocity
	position_px.x += dt * velocity_px.x;
	position_px.y += dt * velocity_px.y;

	// don't let player go out of frame
	if (position_px.x <= 0.0f) position_px.x = 0.0f;
	if ((position_px.x + 8.0f >= (float)ppu.ScreenWidth)) {
		position_px.x = (float)ppu.ScreenWidth - 8.0f;
	}
	if (position_px.y < 0) position_px.y = 0.0f;
	if ((position_px.y + 8.0f >= (float)ppu.ScreenWidth)) {
		position_px.y = (float)ppu.ScreenWidth - 8.0f;
	}
	
	// delete platform
	// check if neighbouring tiles are also platform tiles
	//helper:
	uint32_t tile_x;
	uint32_t tile_y;
	uint32_t background_idx;
	std::function<void(uint32_t, uint32_t)> delete_platform = 
		[&](uint32_t tile_x, uint32_t tile_y) {
		background_idx = tile_x + ppu.BackgroundWidth * tile_y;
		if (ppu.background[background_idx] == 2) {
			ppu.background[background_idx] = 1;
			// uint can't be -ve!!!
			if (tile_x > 0) delete_platform(tile_x - 1, tile_y);
			if (tile_x < ppu.ScreenWidth) delete_platform(tile_x + 1, tile_y);
		}
	};	

	// collision check code
	// bottom-left
	tile_x = (uint32_t)(position_px.x / 8);
	tile_y = (uint32_t)(position_px.y / 8);
	background_idx = tile_x + ppu.BackgroundWidth * tile_y;
	if (ppu.background[background_idx] == 2 && velocity_px.y < 0) { 
		velocity_px.y = bounce_speed_px; // https://cplusplus.com/reference/cmath/abs/
		position_px.y = (tile_y * 8.0f) + 8.0f; // position 1 block above tile
		delete_platform(tile_x, tile_y);
	}
	
	// top-left
	tile_x = (uint32_t)(position_px.x / 8);
	tile_y = (uint32_t)((position_px.y+16.0) / 8);
	background_idx = tile_x + ppu.BackgroundWidth * tile_y;
	if (ppu.background[background_idx] == 2 && velocity_px.y > 0) { 
		velocity_px.y = 0.8f * (-bounce_speed_px); 
		// position 2 blocks below tile
		// since player is 2 blocks tall
		position_px.y = (tile_y * 8.0f) - 16.0f; 
		delete_platform(tile_x, tile_y);
	}

	// bottom-right
	tile_x = (uint32_t)((position_px.x+8) / 8);
	tile_y = (uint32_t)(position_px.y / 8);
	background_idx = tile_x + ppu.BackgroundWidth * tile_y;
	if (ppu.background[background_idx] == 2 && velocity_px.y < 0) { 
		velocity_px.y = 0.8f * std::abs(bounce_speed_px); // https://cplusplus.com/reference/cmath/abs/
		position_px.y = (tile_y * 8.0f) + 8.0f; // position 1 block above tile
		delete_platform(tile_x, tile_y);
	}
	
	// top-right
	tile_x = (uint32_t)((position_px.x+8) / 8);
	tile_y = (uint32_t)(position_px.y+16 / 8);
	background_idx = tile_x + ppu.BackgroundWidth * tile_y;
	if (ppu.background[background_idx] == 2 && velocity_px.y > 0) { 
		velocity_px.y = 0.8f * (-bounce_speed_px); 
		// position 2 blocks below tile
		// since player is 2 blocks tall
		position_px.y = (tile_y * 8.0f) - 16.0f; 
		delete_platform(tile_x, tile_y);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---
	//background scroll:
	ppu.background_position.x = int32_t(-0.5f * position_px.x);
	ppu.background_position.y = int32_t(-0.5f * position_px.y);

	//player sprite:
	// sprite 1
	ppu.sprites[0].x = int8_t(position_px.x);
	ppu.sprites[0].y = int8_t(position_px.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	// sprite 2
	ppu.sprites[2].x = int8_t(position_px.x);
	ppu.sprites[2].y = int8_t(position_px.y + 8.0f);
	ppu.sprites[2].index = 32;
	ppu.sprites[2].attributes = 7;

	//--- actually draw ---
	ppu.draw(drawable_size);

	//text
	//modified from assignment 5:
	{
		//figure out view transform to center:
		glm::mat4 world_to_clip = glm::mat4(
			2.0f/(float)ppu.ScreenWidth, 0.0f, 0.0f, 0.0f, // x coord
			0.0f, 2.0f/(float)ppu.ScreenHeight, 0.0f, 0.0f, // y coord
			0.0f, 0.0f, 1.0f, 0.0f, // z coord (ignore cus 2D)
			-1.0f, -1.0f, 0.0f, 1.0f // for 2D
		);

		DrawLines lines(world_to_clip);

		//helper:
		auto draw_text = [&](glm::vec2 const &at, std::string const &text, float H) {
			lines.draw_text(text,
				glm::vec3(at.x, at.y, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff, 0xff, 0xff, 0xff));
			// shadpw
			// float ofs = (1.0f / 2) / drawable_size.y;
			// lines.draw_text(text,
			// 	glm::vec3(at.x + ofs, at.y + ofs, 0.0),
			// 	glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			// 	glm::u8vec4(0xff, 0xff, 0xff, 0xff));
		};

		//floor-lose condition
		if (position_px.y <= 0 && velocity_px.y < 0.0f) {
			// display you lose text
			glm::vec2 textPos;
			textPos.x = (float)ppu.ScreenWidth/2.0f;
			textPos.y = (float)ppu.ScreenHeight/2.0f;
			draw_text(textPos, "You fell. I guess all shooting stars must fall in the end...", 10.0f);
			// change player sprite to dead player sprite
		}
	};
}