#include <iostream>
#include <memory>

#include <boost/program_options/variables_map.hpp>

#include "Application.h"
#include "ArgumentsParser.h"
#include "Option.h"
#include "AllowedURI.h"
#include "Download.h"
#include "Format.h"
#include "CreateProject.h"

int main(int argc, char* argv[]) {
	using namespace RunFragment;
	namespace po = boost::program_options;
	
	po::variables_map vm;
	try {
		vm = ArgumentsParser::argsToVm(argc, argv, Option::parsingOptions);
	}
	catch(std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Try '--" << Option::help << "'" << std::endl;

		return EXIT_FAILURE;
	}
	
	if(vm.count(Option::help)) {
		std::cout << "Usage:" << std::endl;
		std::cout << "  " << argv[0] << "[options] <Image_shader.glsl>" << std::endl;
		std::cout << "  " << argv[0] << "[options] --" << Option::image << "=<Image_shader.glsl>" << std::endl;
		std::cout << "  " << argv[0] << "[options] --" << Option::config << "=<config_file.ini>" << std::endl;
		std::cout << "  " << argv[0] << "[options] --" << Option::download << "=<url>" << std::endl;
		std::cout << "  " << argv[0] << "[options] --" << Option::create << "=<path>" << std::endl;
		std::cout << Option::helpOptions << std::endl;

		return EXIT_SUCCESS;
	}
	
	if(vm.count(Option::download)) {
		auto uri = std::unique_ptr<AllowedURI> {vm[Option::download].as<AllowedURI*>()};
		
		Download::fromURI(uri.get());
		
		return EXIT_SUCCESS;
	}
	
	if(vm.count(Option::create)) {
		const auto format = vm.count(Option::format) ? vm[Option::format].as<Format>() : defaultFormat;
		const auto path =   vm[Option::create].as<fs::path>();
		
		try {
			switch(format) {
			case Format::ShaderToy: {
				CreateProject::shadertoy(path);
				break;
			}
			case Format::GLSLSandbox: {
				CreateProject::glslsandbox(path);
				break;
			}
			}

			return EXIT_SUCCESS;
		}
		catch(std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;

			return EXIT_FAILURE;
		}
	}
	
	try {
		Application application {ArgumentsParser::vmToAppConfig(vm)};
		application.run();
	}
	catch(std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
