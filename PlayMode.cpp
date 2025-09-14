#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint elements_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > elements_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("elements.pnct"));
	elements_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > elements_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("elements.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = elements_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = elements_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

//Load Samples
Load< Sound::Sample > sample_1_2(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("1-2.wav"));
});


Load< Sound::Sample > sample_1_3(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("1-3.wav"));
});

Load< Sound::Sample > sample_1_5(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("1-5.wav"));
});


Load< Sound::Sample > sample_1_6(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("1-6.wav"));
});

Load< Sound::Sample > sample_1_8(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("1-8.wav"));
});


Load< Sound::Sample > sample_1_11(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("1-11.wav"));
});

void PlayMode::spawn(const std::string& mesh_name, const glm::vec3& pos) { //@GPT referenced
    
    const Mesh& mesh = elements_meshes->lookup(mesh_name);
    scene.transforms.emplace_back();
    Scene::Transform& tr = scene.transforms.back();
    tr.name     = mesh_name;              
    tr.position = pos;
    tr.rotation = glm::quat(1,0,0,0);
    tr.scale    = glm::vec3(1.0f);

    scene.drawables.emplace_back(&tr);
    Scene::Drawable& dr = scene.drawables.back();
    dr.pipeline        = lit_color_texture_program_pipeline;
    dr.pipeline.vao    = elements_meshes_for_lit_color_texture_program;
    dr.pipeline.type   = mesh.type;
    dr.pipeline.start  = mesh.start;
    dr.pipeline.count  = mesh.count;
}

void PlayMode::level_init(int current_level) {
	scene.drawables.clear();
	if(current_level == 1) {
		spawn("Unit1", glm::vec3(0,-5,7));
		spawn("Unit2", glm::vec3(0,0,7));
		spawn("Combo1", glm::vec3(0,5,7));

		spawn("Unit1", glm::vec3(0,-5,3));
		spawn("Unit3", glm::vec3(0,0,3));
		spawn("Combo2", glm::vec3(0,5,3));
		
	}
}

PlayMode::PlayMode() : scene(*elements_scene) {
	//get pointers to leg for convenience:
	//std::cout << "Units size = " << Units.size() << std::endl;
	//std::cout << "Combos size = " << Combos.size() << std::endl;

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	//current_music = Sound::loop(*sample_1_2, 1.0f, 10.0f);
	level_init(1);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (level_cs) {
		if (evt.type == SDL_EVENT_KEY_DOWN) {
			if (evt.key.key == SDLK_RETURN) {
				level_cs = false;
				return true;
			}
		}
		return false;
	}
	if (evt.type == SDL_EVENT_KEY_DOWN) {
		if (evt.key.key == SDLK_ESCAPE) {
			SDL_SetWindowRelativeMouseMode(Mode::window, false);
			return true;
		} else if (evt.key.key == SDLK_A) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_D) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_W) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_S) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_SPACE) {
			//if (honk_oneshot) honk_oneshot->stop();
			//std::cout<< "honk" << std::endl;
			//honk_oneshot = Sound::play(*sample_1_3, 3.0f, 0.0f); //hardcoded position of front of car, from blender
		}
	} else if (evt.type == SDL_EVENT_KEY_UP) {
		if (evt.key.key == SDLK_A) {
			left.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_D) {
			right.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_W) {
			up.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_S) {
			down.pressed = false;
			return true;
		}
	} //else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
	// 	if (SDL_GetWindowRelativeMouseMode(Mode::window) == false) {
	// 		SDL_SetWindowRelativeMouseMode(Mode::window, true);
	// 		return true;
	// 	}
	// } else if (evt.type == SDL_EVENT_MOUSE_MOTION) {
	// 	if (SDL_GetWindowRelativeMouseMode(Mode::window) == true) {
	// 		//@camera control referenced from Matteo
	// 		glm::vec2 motion = glm::vec2(
	// 			evt.motion.xrel / float(window_size.y),
	// 			evt.motion.yrel / float(window_size.y));
	// 		// Make sure the rotation does not roll the character by multiplying yaw on one side
	// 		// and pitch on the other (source: https://stackoverflow.com/questions/46738139/prevent-rotation-around-certain-axis-with-quaternion)
	// 		camera->transform->rotation = glm::normalize(
	// 			glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 0.0f, 1.0f)) 
	// 			* camera->transform->rotation 
	// 			* glm::angleAxis(-motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f)));
			
	// 		return true;
	// 	}
	//}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// wobble += elapsed / 10.0f;
	// wobble -= std::floor(wobble);

	// hip->rotation = hip_base_rotation * glm::angleAxis(
	// 	glm::radians(5.0f * std::sin(wobble * 2.0f * float(M_PI))),
	// 	glm::vec3(0.0f, 1.0f, 0.0f)
	// );
	// upper_leg->rotation = upper_leg_base_rotation * glm::angleAxis(
	// 	glm::radians(7.0f * std::sin(wobble * 2.0f * 2.0f * float(M_PI))),
	// 	glm::vec3(0.0f, 0.0f, 1.0f)
	// );
	// lower_leg->rotation = lower_leg_base_rotation * glm::angleAxis(
	// 	glm::radians(10.0f * std::sin(wobble * 3.0f * 2.0f * float(M_PI))),
	// 	glm::vec3(0.0f, 0.0f, 1.0f)
	// );

	// //move sound to follow leg tip position:
	// leg_tip_loop->set_position(get_leg_tip_position(), 1.0f / 60.0f);

	//move camera:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 30.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		glm::mat4x3 frame = camera->transform->make_parent_from_local();
		glm::vec3 frame_right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 frame_forward = -frame[2];

		camera->transform->position += move.x * frame_right + move.y * frame_forward;
	}

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_parent_from_local();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	float aspect = float(drawable_size.x) / float(drawable_size.y);
	constexpr float H = 0.09f;
	float ofs = 2.0f / drawable_size.y;

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this

	if(level_cs) { //cut scene
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));
		
		if(level == 1) {
			lines.draw_text("Believe it or not, music is math. Are you ready for level 1?",
				glm::vec3(-aspect + 0.1f * H, 0.0f + 0.1f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			
			lines.draw_text("Believe it or not, music is math. Are you ready for level 1?",
				glm::vec3(-aspect + 0.1f * H + ofs, 0.0f + + 0.1f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		}
		return;
	}

	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.851f, 0.867f, 0.753f, 0.5f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	GL_ERRORS();
}

// glm::vec3 PlayMode::get_leg_tip_position() {
// 	//the vertex position here was read from the model in blender:
// 	return lower_leg->make_world_from_local() * glm::vec4(-1.26137f, -11.861f, 0.0f, 1.0f);
// }
