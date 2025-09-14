#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

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

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	//Scene::Transform *hip = nullptr;
	//Scene::Transform *upper_leg = nullptr;
	//Scene::Transform *lower_leg = nullptr;

	// std::vector< Mesh * > Units;
	// std::vector< Mesh * > Combos;

	//glm::quat hip_base_rotation;
	//glm::quat upper_leg_base_rotation;
	//glm::quat lower_leg_base_rotation;
	//float wobble = 0.0f;

	glm::vec3 get_leg_tip_position();

	//music coming from the tip of the leg (as a demonstration):
	//std::shared_ptr< Sound::PlayingSample > current_music;

	//car honk sound:
	//std::shared_ptr< Sound::PlayingSample > honk_oneshot;
	
	//camera:
	Scene::Camera *camera = nullptr;

	void level_init(int current_level);
	void spawn(const std::string& mesh_name, const glm::vec3& pos);

};
