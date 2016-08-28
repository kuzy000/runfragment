#include "Format.h"

namespace RunFragment {

void validate(boost::any& value, const std::vector<std::string>& args, Format*, int) {
	namespace po = boost::program_options;
	
	const std::string& arg = po::validators::get_single_string(args);
	
	if(arg == "s" || arg == "shadertoy") {
		value = boost::any {Format::ShaderToy};
	}
	else if(arg == "g" || arg == "glslsandbox") {
		value = boost::any {Format::GLSLSandbox};
	}
	else {
		throw po::validation_error {po::validation_error::invalid_option_value};
	}
}

}
