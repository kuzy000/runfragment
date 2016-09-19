#include "CreateProject.h"

#include "Option.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>

namespace RunFragment {
namespace CreateProject {

void glslsandbox(fs::path path) {
	std::cout << "Image: " << path << std::endl;
	
	fs::ofstream ofs {path};
	ofs << R"glsl(
#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void main() {
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)glsl";

}

void shadertoy(fs::path path) {
	const auto dir = path;
	const auto config = dir / "config.ini";
	const auto image = dir / "Image.glsl";
	if(fs::create_directory(dir)) {
		std::cout << "Saving project to: " << dir << std::endl;
		
		std::cout << "    config: " << config << std::endl;
		fs::ofstream ofsConfig {config};
		ofsConfig << Option::format << " = shadertoy" << std::endl;
		ofsConfig << Option::image << " = Image.glsl" << std::endl;
		
		std::cout << "    Image: " << image << std::endl;
		fs::ofstream ofsImage {image};
		ofsImage << R"glsl(
void mainImage(out vec4 fragColor, in vec2 fragCoord) {
	fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)glsl";
	
	}
	else {
		std::cerr << "Error: can't create the project directory" << std::endl;
	}

}

}
}
