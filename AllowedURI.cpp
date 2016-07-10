#include "AllowedURI.h"

#include <regex>

namespace RunFragment {

AllowedURI* AllowedURI::fromUriRaw(std::string location) {
	const std::regex shaderToy   {"^(www\\.)?shadertoy\\.com$"};
	const std::regex glslSandbox {"^(www\\.)?glslsandbox\\.com$"};
	const auto uri = boost::network::uri::uri {location};
	const auto host = uri.host();

	std::smatch match;
	if(std::regex_match(host, match, shaderToy)) {
		return new ShadertoyURI {std::move(location)};
	}
	else if(std::regex_match(host, match, glslSandbox)) {
		return new GLSLSandboxURI {std::move(location)};
	}
	
	return nullptr;
}

void validate(boost::any& value, const std::vector<std::string>& args, AllowedURI**, int) {
	namespace po = boost::program_options;
	
	const std::string& arg = po::validators::get_single_string(args);
	
	const auto uri = AllowedURI::fromUriRaw(std::move(arg));
	
	if(uri) {
		value = boost::any {uri};
	}
	else {
		throw po::validation_error {po::validation_error::invalid_option_value};
	}
}

}
