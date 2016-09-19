#pragma once

#include <boost/program_options.hpp>

namespace RunFragment {

enum class Format {
	ShaderToy,
	GLSLSandbox,
};

const auto defaultFormat = Format::GLSLSandbox;

void validate(boost::any& value, const std::vector<std::string>& args, Format*, int);

}
