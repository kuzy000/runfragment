#include "Renderer.h"

#include "OnScopeEnd.h"

#include <stdexcept>
#include <regex>
#include <fstream>
#include <iostream>

namespace RunFragment {

Renderer::SharedData Renderer::sharedData;

Renderer::Renderer(const Configuration& config, Target target, GLFWwindow* window)
	: config {config}
	, target {target}
	, window {window}
	, path {target == Target::Image ? *config.image.filename
	      : *config.bufs[static_cast<std::size_t>(target)].filename} {
	const GLfloat vertices[] = {
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f,  1.0f,
	};
	const GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	
	glGenVertexArrays(1, &vao);
	onDestruction.push([this] {
		glDeleteVertexArrays(1, &vao);
	});
	
	glGenBuffers(1, &vbo);
	onDestruction.push([this] {
		glDeleteBuffers(1, &vao);
	});

	glGenBuffers(1, &ebo);
	onDestruction.push([this] {
		glDeleteBuffers(1, &ebo);
	});
	
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
	
	const std::string vertexSource = R"glsl(
		#version 330 core
		layout(location = 0) in vec2 position;
		varying vec2 )glsl" + config.iSurfacePosition + R"glsl(;
		void main() {
			)glsl" + config.iSurfacePosition + R"glsl( = position;
			gl_Position = vec4(position, 0, 1);
		}
	)glsl";
	
	vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
	onDestruction.push([this] {
		glDeleteShader(vertex);
	});
	
	reloadFile();
	
	onDestruction.push([this] {
		glDeleteShader(fragment);
		glDeleteProgram(program);
	});
	
	glGenFramebuffers(1, &fbo);
	onDestruction.push([this] {
		glDeleteFramebuffers(1, &fbo);
	});
	
	glGenTextures(1, &tex);
	onDestruction.push([this] {
		glDeleteTextures(1, &tex);
	});

	glBindTexture(GL_TEXTURE_2D, tex);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
}

void Renderer::start() {
	startTime = std::chrono::high_resolution_clock::now();
}

void Renderer::render() {
	reloadShader();
	
	if(target != Target::Image) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program);
	glBindVertexArray(vao);
	
	auto uniformSamplerIfDefined = [this] (int num) {
		if(sharedData.texs[num] != 0) {
			glActiveTexture(GL_TEXTURE0 + num);
			glBindTexture(GL_TEXTURE_2D, sharedData.texs[num]);

			glUniform1i(iChannels[num], num);
		}
	};
	
	uniformSamplerIfDefined(0);
	uniformSamplerIfDefined(1);
	uniformSamplerIfDefined(2);
	uniformSamplerIfDefined(3);
	
	GLfloat globalTime = std::chrono::duration_cast<std::chrono::duration<GLfloat>>(std::chrono::high_resolution_clock::now() - startTime).count();
	glUniform1f(iGlobalTime, globalTime * config.time);
	
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	glUniform2f(iResolution, static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	glUniform2f(iMouse, static_cast<GLfloat>(1. - x / width), static_cast<GLfloat>(y / height));
	
	time_t stamp = time(nullptr);
	tm* now = localtime(&stamp);
	GLfloat year = now->tm_year + 1900;
	GLfloat month = now->tm_mon;
	GLfloat day = now->tm_mday;
	GLfloat sec = [] {
		time_t t1, t2;
		struct tm tms;
		time(&t1);
		localtime_r(&t1, &tms);
		tms.tm_hour = 0;
		tms.tm_min = 0;
		tms.tm_sec = 0;
		t2 = mktime(&tms);
		return t1 - t2;
	} ();
	glUniform4f(iDate, year, month, day, sec);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Renderer::reloadFile() {
	std::lock_guard<std::mutex> guard {sourceChanging};
	
	std::ifstream file {path};
	
	if(file.is_open()) {
		std::stringstream ss;
		
		if(config.addUniforms) {
			ss << "uniform vec2      " + config.iResolution + ";" << std::endl
			   << "uniform float     " + config.iGlobalTime + ";" << std::endl
			   << "uniform float     " + config.iGlobalDelta + ";" << std::endl
			   << "uniform float     " + config.iGlobalFrame + ";" << std::endl
			   << "uniform float     " + config.iChannelTime + "[4];" << std::endl
			   << "uniform vec4      " + config.iMouse + ";" << std::endl
			   << "uniform vec4      " + config.iDate + ";" << std::endl
			   << "uniform float     " + config.iSampleRate + ";" << std::endl
			   << "uniform vec3      " + config.iChannelResolution + "[4];" << std::endl
			   << "uniform sampler2D " + config.iChannel + "0;" << std::endl
			   << "uniform sampler2D " + config.iChannel + "1;" << std::endl
			   << "uniform sampler2D " + config.iChannel + "2;" << std::endl
			   << "uniform sampler2D " + config.iChannel + "3;" << std::endl;
		}
		
		ss << file.rdbuf();
		
		if(config.main != boost::none) {
			ss << 
				"void main() {\n"
					<< *config.main + "(gl_FragColor, gl_FragCoord);\n"
				"}\n";
		}
		
		fragmentSource = ss.str();
		changed = true;
	}
	else {
		std::cerr << "Error: can't open file '" << path << "'" << std::endl;
	}
}

void Renderer::reloadShader() {
	std::lock_guard<std::mutex> guard {sourceChanging};

	if(changed) {
		GLuint newFragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
		if(newFragment != 0) {
			std::cout << path << ": OK" << std::endl;
			if(fragment != 0) {
				glDeleteShader(fragment);
			}
			fragment = newFragment;

			if(program != 0) {
				glDeleteProgram(program);
			}
			program = glCreateProgram();
			glAttachShader(program, vertex);
			glAttachShader(program, fragment);
			glLinkProgram(program);

			iResolution = glGetUniformLocation(program, config.iResolution.c_str());
			iGlobalTime = glGetUniformLocation(program, config.iGlobalTime.c_str());
			iGlobalDelta = glGetUniformLocation(program, config.iGlobalDelta.c_str());
			iGlobalFrame = glGetUniformLocation(program, config.iGlobalFrame.c_str());
			iChannelTime = glGetUniformLocation(program, config.iChannelTime.c_str());
			iMouse = glGetUniformLocation(program, config.iMouse.c_str());
			iDate = glGetUniformLocation(program, config.iDate.c_str());
			iSampleRate = glGetUniformLocation(program, config.iSampleRate.c_str());
			iChannelResolution = glGetUniformLocation(program, config.iChannelResolution.c_str());
			iChannels[0] = glGetUniformLocation(program, (config.iChannel + "0").c_str());
			iChannels[1] = glGetUniformLocation(program, (config.iChannel + "1").c_str());
			iChannels[2] = glGetUniformLocation(program, (config.iChannel + "2").c_str());
			iChannels[3] = glGetUniformLocation(program, (config.iChannel + "3").c_str());
		}
	}

	changed = false;
}

void Renderer::onWindowResize(int width, int height) {
	for(GLuint& tex : sharedData.texs) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}
}

GLuint Renderer::compileShader(GLenum type, const std::string& source) {
	GLuint id = glCreateShader(type);

	const char* cStr = source.c_str();
	glShaderSource(id, 1, &cStr, nullptr);
	glCompileShader(id);

	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		GLint length = 1024;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		if(length == 0) { 
			length = 1024;
		}

		std::unique_ptr<GLchar[]> errorLog = std::unique_ptr<GLchar[]>(new GLchar[length]);
		glGetShaderInfoLog(id, sizeof(GLchar) * length, nullptr, errorLog.get());

		std::cerr << path << ":" << std::endl;
		std::cerr << errorLog.get() << std::endl;

		id = 0;
	}

	return id;
}

}
