#include "Renderer.h"

#include <stdexcept>
#include <regex>
#include <fstream>
#include <iostream>

#include <FreeImagePlus.h>

#include "OnScopeEnd.h"

namespace RunFragment {

Renderer::SharedData Renderer::sharedData;

Renderer::Renderer(const AppConfig& config, Target target, GLFWwindow* window)
	: config {config}
	, renderConfig {target == Target::Image ? *config.image
	                                        : *config.bufs[static_cast<std::size_t>(target)]}
	, target {target}
	, window {window} {
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
	
	if(target != Target::Image) {
		glGenFramebuffers(1, &fbo);
		onDestruction.push([this] {
			glDeleteFramebuffers(1, &fbo);
		});
		
		glGenTextures(1, &buf);
		onDestruction.push([this] {
			glDeleteTextures(1, &buf);
		});
	
		glBindTexture(GL_TEXTURE_2D, buf);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buf, 0);
	}
	
	for(std::size_t i = 0; i < renderConfig.channels.size(); i++) {
		const Channel* channel = renderConfig.channels[i].get();
		
		if(const auto channelImage = dynamic_cast<const ChannelImage*>(channel)) {
			const auto path = channelImage->path;
			
			if(sharedData.texs.find(path) != sharedData.texs.end()) {
				break;
			}
			
			GLuint tex;
			glGenTextures(1, &tex); // TODO destruction
			sharedData.texs.emplace(path, tex);
			
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, tex);
			
			fipImage textureFile;
			if(!textureFile.load(path.c_str())) {
				std::cerr << "Error: can't open file '" << path << "'" << std::endl;
			}

			if(!textureFile.convertTo24Bits()) {
				std::cerr << path << ": texture converting error" << std::endl;
			}
			
			if(channelImage->flipped) {
				textureFile.flipVertical();
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureFile.getWidth(), textureFile.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, textureFile.accessPixels());
		}
	}
}

void Renderer::start() {
	startTime = std::chrono::high_resolution_clock::now();
	prevFrameTime = std::chrono::high_resolution_clock::now();
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
	
	for(std::size_t i = 0; i < renderConfig.channels.size(); i++) {
		const Channel* channel = renderConfig.channels[i].get();
		
		if(const auto channelBuf = dynamic_cast<const ChannelBuf*>(channel)) {
			const auto num = static_cast<std::size_t>(channelBuf->kind);
			
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, sharedData.bufs[num]);
	
			glUniform1i(iChannels[i], i);
		}
		else if(const auto channelImage = dynamic_cast<const ChannelImage*>(channel)) {
			const auto path = channelImage->path;
			
			const auto it = sharedData.texs.find(path);
			if(it == sharedData.texs.end()) {
				continue;
			}
			
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, it->second);
			
			glUniform1i(iChannels[i], i);
		}
	}
	
	const auto currentTime = std::chrono::high_resolution_clock::now();

	const GLfloat globalTime = std::chrono::duration_cast<std::chrono::duration<GLfloat>>(currentTime - startTime).count();
	glUniform1f(iGlobalTime, globalTime * config.time);
	
	const GLfloat timeDelta = std::chrono::duration_cast<std::chrono::duration<GLfloat>>(currentTime - prevFrameTime).count();
	glUniform1f(iTimeDelta, timeDelta);
	
	prevFrameTime = currentTime;
	
	glUniform1i(iFrame, frame);
	frame++;
	
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	glUniform2f(iResolution, static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	glUniform2f(iMouse, static_cast<GLfloat>(1. - x / width), static_cast<GLfloat>(y / height));
	
	const time_t stamp = time(nullptr);
	const tm* const now = localtime(&stamp);
	
	const GLfloat year = now->tm_year + 1900;
	const GLfloat month = now->tm_mon;
	const GLfloat day = now->tm_mday;
	const GLfloat sec = [] {
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
	
	std::ifstream file {renderConfig.path.string()};
	
	if(file.is_open()) {
		std::stringstream ss;
		
		if(config.addUniforms) {
			ss << "uniform vec2      " << config.iResolution << ";" << std::endl
			   << "uniform float     " << config.iGlobalTime << ";" << std::endl
			   << "uniform float     " << config.iTimeDelta << ";" << std::endl
			   << "uniform int       " << config.iFrame << ";" << std::endl
			   << "uniform float     " << config.iFrameRate << ";" << std::endl
			   << "uniform float     " << config.iChannelTime << "[4];" << std::endl
			   << "uniform vec3      " << config.iChannelResolution << "[4];" << std::endl
			   << "uniform vec4      " << config.iMouse << ";" << std::endl
			   << "uniform vec4      " << config.iDate << ";" << std::endl
			   << "uniform float     " << config.iSampleRate << ";" << std::endl
			   << "uniform sampler2D " << config.iChannel << "0;" << std::endl
			   << "uniform sampler2D " << config.iChannel << "1;" << std::endl
			   << "uniform sampler2D " << config.iChannel << "2;" << std::endl
			   << "uniform sampler2D " << config.iChannel << "3;" << std::endl;
		}
		
		ss << file.rdbuf();
		
		if(config.main != boost::none) {
			ss << 
				"void main() {\n"
					<< *config.main << "(gl_FragColor, gl_FragCoord);\n"
				"}\n";
		}
		
		fragmentSource = ss.str();
		changed = true;
	}
	else {
		std::cerr << "Error: can't open file '" << renderConfig.path << "'" << std::endl;
	}
}

void Renderer::reloadShader() {
	std::lock_guard<std::mutex> guard {sourceChanging};

	if(changed) {
		const GLuint newFragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
		if(newFragment != 0) {
			std::cout << target << " [ " << renderConfig.path << " ]: OK" << std::endl;
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
			iTimeDelta = glGetUniformLocation(program, config.iTimeDelta.c_str());
			iFrame = glGetUniformLocation(program, config.iFrame.c_str());
			iFrameRate = glGetUniformLocation(program, config.iFrameRate.c_str());
			iChannelTime = glGetUniformLocation(program, config.iChannelTime.c_str());
			iChannelResolution = glGetUniformLocation(program, config.iChannelResolution.c_str());
			iMouse = glGetUniformLocation(program, config.iMouse.c_str());
			iDate = glGetUniformLocation(program, config.iDate.c_str());
			iSampleRate = glGetUniformLocation(program, config.iSampleRate.c_str());
			iChannels[0] = glGetUniformLocation(program, (config.iChannel + "0").c_str());
			iChannels[1] = glGetUniformLocation(program, (config.iChannel + "1").c_str());
			iChannels[2] = glGetUniformLocation(program, (config.iChannel + "2").c_str());
			iChannels[3] = glGetUniformLocation(program, (config.iChannel + "3").c_str());
		}
	}

	changed = false;
}

void Renderer::onWindowResize(int width, int height) {
}

GLuint Renderer::compileShader(GLenum type, const std::string& source) {
	const GLuint id = glCreateShader(type);

	const auto cStr = source.c_str();
	glShaderSource(id, 1, &cStr, nullptr);
	glCompileShader(id);

	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		GLint length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		if(length == 0) { 
			length = 1024;
		}

		std::unique_ptr<GLchar[]> errorLog = std::unique_ptr<GLchar[]>(new GLchar[length]);
		glGetShaderInfoLog(id, sizeof(GLchar) * length, nullptr, errorLog.get());

		std::cerr << target << " [ " << renderConfig.path << " ]:" << std::endl;
		std::cerr << errorLog.get() << std::endl;

		return 0;
	}

	return id;
}

std::ostream& operator <<(std::ostream& os, Renderer::Target target) {
	const std::array<std::string, 5> targets {{ "Image",  "BufA",  "BufB",  "BufC", "BufD" }};
	os << targets.at(static_cast<std::size_t>(target) + 1);
	
	return os;
}

}
