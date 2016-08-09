#pragma once

#include "AppConfig.h"
#include "OnScopeEnd.h"

#include <mutex>
#include <chrono>
#include <array>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace RunFragment {

class Renderer {
public:
	enum class Target { 
		Image = -1, 
		BufA = 0, 
		BufB = 1, 
		BufC = 2,
		BufD = 3,
	};
	
	Renderer(const AppConfig& config, Target target, GLFWwindow* window);
	
	void start();
	void render();
	void reloadFile();
	
	static void onWindowResize(int width, int height);
private:
	void reloadShader();
	
	GLuint compileShader(GLenum type, const std::string& source);
	
	const AppConfig& config;
	const RenderConfig& renderConfig;
	const Target target;
	
	GLFWwindow* window;
	
	struct SharedData {
		std::array<GLuint, 4> bufs {{0}};
		std::map<std::string, GLuint> texs;
	};
	static SharedData sharedData;
	
	GLuint img = 0;
	
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	
	GLuint& buf = sharedData.bufs[static_cast<std::size_t>(target)];
	GLuint fbo = 0;
	
	GLuint vertex = 0;
	GLuint fragment = 0;
	GLuint program = 0;
	
	GLint iResolution = -1; // done
	GLint iGlobalTime = -1; // done
	GLint iTimeDelta = -1; // done
	GLint iFrame = -1; // done
	GLint iFrameRate = -1;
	GLint iChannelTime = -1;
	GLint iChannelResolution = -1;
	GLint iMouse = -1; // done
	GLint iDate = -1; // done
	GLint iSampleRate = -1;
	std::array<GLint, 4> iChannels {{-1}}; // done
	
	std::mutex sourceChanging;
	
	std::string fragmentSource;
	bool changed = false;
	
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point prevFrameTime;
	GLint frame = 0;
	
	OnScopeEnd onDestruction;
};

std::ostream& operator <<(std::ostream& os, Renderer::Target target);

}

