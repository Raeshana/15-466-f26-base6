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

#include "palettes.hpp"

// #include <random>

PlayMode::PlayMode() {
	// put palettes into palette table
	ppu.palette_table[0] = player_palette;
	ppu.palette_table[1] = default_palette;
	ppu.palette_table[2] = background_palette;
	ppu.palette_table[3] = cloud_palette;

	// put tiles into tile table
	ppu.tile_table[1] = STARS_TILE_1;
	ppu.tile_table[2] = STARS_TILE_2;
	ppu.tile_table[3] = STARS_TILE_3;
	ppu.tile_table[4] = STARS_TILE_4;
	ppu.tile_table[30] = CLOUD;
	ppu.tile_table[32] = PLAYER_TILE_1;
	ppu.tile_table[33] = PLAYER_TILE_2;
	ppu.tile_table[34] = PLAYER_TILE_3;
	ppu.tile_table[35] = PLAYER_TILE_4;

	// generate platforms as sprites
	uint32_t sprite_idx = 10;
	for (uint32_t y = 24; y < ppu.ScreenHeight && sprite_idx + 4 < 64; y += 24) {
	// get a random x value on the screen
	// we dont have to worry about drawing 'off the screen'
	// since background = 2*width
    uint32_t platform_x = rand() % (ppu.ScreenWidth - 40);

	//draw sprites (5 wide)
    for (uint32_t i = 0; i < 5; ++i) {
        ppu.sprites[sprite_idx].x = (uint8_t)(platform_x + i * 8.0f); // tile is 8 px
        ppu.sprites[sprite_idx].y = (uint8_t)y;
        ppu.sprites[sprite_idx].index = 30;
        ppu.sprites[sprite_idx].attributes = 3;

        sprite_idx++;
    }
}
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

	// collision check code
	// as sprites:
	for (uint32_t i = 5; i < 64; i++) {

		// skip unused sprites
		if (ppu.sprites[i].index != 30) continue;

		float sprite_x = ppu.sprites[i].x;
		float sprite_y = ppu.sprites[i].y;

		// falling 
		if (velocity_px.y < 0.0f &&
			position_px.x + 16.0f > sprite_x &&
			position_px.x < sprite_x + 8.0f &&
			position_px.y <= sprite_y + 8.0f &&
			position_px.y + 16.0f >= sprite_y) {

			velocity_px.y = bounce_speed_px;
			position_px.y = sprite_y + 8.0f;

			// ignore platform-- transparent
			ppu.sprites[i] = ppu.sprites[6];
		}

		// hit head
		else if (velocity_px.y > 0.0f &&
				position_px.x + 16.0f > sprite_x &&
				position_px.x < sprite_x + 8.0f &&
				position_px.y + 16.0f >= sprite_y &&
				position_px.y <= sprite_y + 8.0f) {

			velocity_px.y = -0.8f * bounce_speed_px;
			position_px.y = sprite_y - 16.0f;

			// ignore platform-- transparent
			ppu.sprites[i] = ppu.sprites[6];
		}
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---
	//background scroll (vertical only):
	ppu.background_position.y = int32_t(-0.5f * position_px.y);

	//player sprite:
	// sprite 1
	ppu.sprites[0].x = int8_t(position_px.x);
	ppu.sprites[0].y = int8_t(position_px.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 0;
	// sprite 2
	ppu.sprites[1].x = int8_t(position_px.x + 8.0f);
	ppu.sprites[1].y = int8_t(position_px.y);
	ppu.sprites[1].index = 33;
	ppu.sprites[1].attributes = 0;
	// sprite 3
	ppu.sprites[2].x = int8_t(position_px.x);
	ppu.sprites[2].y = int8_t(position_px.y + 8.0f);
	ppu.sprites[2].index = 34;
	ppu.sprites[2].attributes = 0;
	// sprite 4
	ppu.sprites[3].x = int8_t(position_px.x + 8.0f);
	ppu.sprites[3].y = int8_t(position_px.y + 8.0f);
	ppu.sprites[3].index = 35;
	ppu.sprites[3].attributes = 0;

	// win sprite
	ppu.sprites[4].x = 100;
	ppu.sprites[4].y = 100;
	ppu.sprites[4].index = 31;
	ppu.sprites[4].attributes = 0;

	// cloud sprite
	ppu.sprites[5].x;
	ppu.sprites[5].y;
	ppu.sprites[5].index = 30;
	ppu.sprites[5].attributes = 3;

	// nothing
	ppu.sprites[6].x;
	ppu.sprites[6].y;
	ppu.sprites[6].index = 3;
	ppu.sprites[6].attributes = 1;

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

		//middle of screen
		glm::vec2 textPos;
		textPos.x = (float)ppu.ScreenWidth/2.0f;
		textPos.y = (float)ppu.ScreenHeight/2.0f;

		//floor-lose condition
		if (position_px.y <= 0 && velocity_px.y < 0.0f) {
			// display you lose text
			draw_text(textPos, "You fell. I guess all shooting stars must fall in the end...", 10.0f);
			// change player sprite to dead player sprite
		}

		// win condition
		// if the player has removed all the blocks:
		bool can_win = true;
		for (int i = 10; i < 64; i++) {
			if (ppu.sprites[i].index == 30) { // cloud idx 30
			can_win = can_win & false;
			break; // we just need 1 false
			}
		}
		if (can_win) {
			draw_text(textPos, "For the first time, it is you who gets to make a wish. You win.", 10.0f);
			velocity_px.x = 0.0f;
			velocity_px.y = 0.0f;
			acceleration_px.y = 0.0f;
		}
	};
}