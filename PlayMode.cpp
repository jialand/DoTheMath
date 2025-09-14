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
Load< Sound::Sample > sample_honk(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("honk.wav"));
});

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

void PlayMode::level_init() {
	level++;
	scene.drawables.clear();
	if(level == 1) {
		spawn("Unit1", glm::vec3(0,-5,7));
		spawn("Unit2", glm::vec3(0,0,7));
		spawn("Combo1", glm::vec3(0,5,7));

		spawn("Unit1", glm::vec3(0,-5,3));
		spawn("Unit3", glm::vec3(0,0,3));
		spawn("Combo2", glm::vec3(0,5,3));
		
	}
}

PlayMode::PlayMode() : scene(*elements_scene) {
	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	//current_music = Sound::loop(*sample_1_2, 1.0f, 10.0f);
	level_init();
}

//@GPT referenced  & https://learnopengl.com/Getting-started/Coordinate-Systems?utm_source=chatgpt.com
static bool world_to_screen_px (const Scene::Camera* cam, glm::uvec2 window_size, const glm::vec3& world, glm::vec2* out_px) {
	glm::mat4x3 F = cam->transform->make_parent_from_local(); // [right, up, -fwd, eye]
    glm::vec3 eye   = F[3];
    glm::vec3 right = F[0];
    glm::vec3 up    = F[1];
    glm::vec3 fwd   = -F[2];

    // rebase the transform to camera_centered
    glm::vec3 p = world - eye;
    float x_cam = glm::dot(p, right);
    float y_cam = glm::dot(p, up);
    float z_cam = glm::dot(p, fwd); // look at -Z

    if (z_cam <= 0.0001f) return false; // begind camera. No projection

    // camera view range
    float tan_half_fovy = std::tan(0.5f * cam->fovy);
    float tan_half_fovx = tan_half_fovy * cam->aspect;

	//position in camera view
    float ndc_x = (x_cam / z_cam) / tan_half_fovx;
    float ndc_y = (y_cam / z_cam) / tan_half_fovy;

    // out of camera viewport
    if (ndc_x < -1.f || ndc_x > 1.f || ndc_y < -1.f || ndc_y > 1.f) return false;

    //ndc transform to pixel transform
    (*out_px).x = (ndc_x * 0.5f + 0.5f) * float(window_size.x);
    (*out_px).y = (1.0f - (ndc_y * 0.5f + 0.5f)) * float(window_size.y);
    return true;
}

Sound::Sample const& PlayMode::pick_sample_for_name(const std::string& name) const{
	std::cout << "Trying to play sample for" << name << std::endl;
	if (level == 1) {
		if (name.find("Unit1")  != std::string::npos) return *sample_1_2;
		if (name.find("Unit2")  != std::string::npos) return *sample_1_3;
		if (name.find("Unit3")  != std::string::npos) return *sample_1_6;
		if (name.find("Combo1") != std::string::npos) return *sample_1_5;
    	if (name.find("Combo2") != std::string::npos) return *sample_1_8;

	}
    // if (name.find("Unit1")  != std::string::npos) return *sample_1_2;
    // if (name.find("Unit2")  != std::string::npos) return *sample_1_3;
    // if (name.find("Unit3")  != std::string::npos) return *sample_1_5;
    // if (name.find("Unit4")  != std::string::npos) return *sample_1_6;
    // if (name.find("Unit5")  != std::string::npos) return *sample_1_8;
    // if (name.find("Unit6")  != std::string::npos) return *sample_1_11;

    // if (name.find("Combo1") != std::string::npos) return *sample_1_2;
    // if (name.find("Combo2") != std::string::npos) return *sample_1_3;
    // if (name.find("Combo3") != std::string::npos) return *sample_1_5;
    // if (name.find("Combo4") != std::string::npos) return *sample_1_6;
    // if (name.find("Combo5") != std::string::npos) return *sample_1_8;

    // 默认兜底
    return *sample_honk;
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
	} else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) { //click mouse to interact @GPT referenced
		//get mouse position on screen (pixel)
		std::cout << "Mouse clicked" << std::endl;
		const float mx = evt.button.x;
		const float my = evt.button.y;
		const glm::vec2 mouse_px(mx,my);

		//selection radius
		const float pick_radius_px = 100.0f;

		float target_dis = std::numeric_limits<float>::max();
		Scene::Drawable* target = nullptr;

		//find the closest drawable
		for(auto &dr : scene.drawables) {
			if(!dr.transform) continue;
			glm::vec2 px;
			if(!world_to_screen_px(camera, window_size, dr.transform->position, &px)) continue;

			float dis = glm::dot(px-mouse_px, px-mouse_px);
			if(dis < pick_radius_px * pick_radius_px && dis < target_dis) { //within radius and better than current target
				target_dis = dis;
				target = &dr;
			}
		}
		
		if(target && target->transform) {
			std::cout << "Target is" << target->transform->name << std::endl;
			Sound::stop_all_samples();
			Sound::Sample const& current_music = pick_sample_for_name(target->transform->name);
			Sound::play(current_music, 3.0f, 0.0f);
		}
		return true;
	}

	return false;
}

void PlayMode::update(float elapsed) {
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
