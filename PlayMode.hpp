#include "PPU466.hpp"
#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	// player stuff:
	// Referenced https://gafferongames.com/post/integration_basics/
	float mass = 10.0f;
	glm::vec2 position = glm::vec2(100.0f, 1.0f); 
	glm::vec2 velocity = glm::vec2(100.0f, 0.0f);     
	glm::vec2 acceleration = glm::vec2(0.0f, -10.0f); // gravity

	// tilemap:


	//----- drawing handled by PPU466 -----
	PPU466 ppu;
};
