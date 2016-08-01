#include "Download.h"

#include <regex>
#include <fstream>

#define BOOST_NETWORK_ENABLE_HTTPS
#include <boost/network/include/http/client.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace RunFragment {
namespace Download {

void fromURI(const RunFragment::AllowedURI* location) {
	using namespace boost::network;
	using namespace rapidjson;
	
	if(dynamic_cast<const GLSLSandboxURI*>(location)) {
		auto fragment = location->fragment();
		uri::uri url {"http://glslsandbox.com/item/" + fragment};
		
		http::client client;
		http::client::request request {url};
		
		std::cout << "Downloading using the GLSLSandbox API..." << std::endl;
		http::client::response response = client.get(request);

		
		Document d;
		if(!d.Parse(response.body().c_str()).HasParseError()) {
			const auto code = d["code"].GetString();
			
			std::string filename = fragment + ".glsl";
			std::cout << "Saving Image to: " << filename << std::endl;
			std::ofstream ofs {filename.c_str()};
			ofs << "// Downloaded from " << location->string() << std::endl << std::endl;
			ofs << code << std::endl;
		}
	}
	if(dynamic_cast<const ShadertoyURI*>(location)) {
		const std::string key {"Nd8KwM"};
		const auto path = location->path();
		
		std::regex shaderIdRegex {"/view/(.*)"};
		std::smatch shaderIdSmatch;
		
		std::regex_search(path.begin(), path.end(), shaderIdSmatch, shaderIdRegex);
		
		const std::string shaderId = shaderIdSmatch[1];
		
		uri::uri url {"https://www.shadertoy.com/api/v1/shaders/" + shaderId + "?key=" + key};
		
		http::client client;
		http::client::request request {url};
		
		std::cout << "Downloading using the Shadertoy.com API..." << std::endl;
		http::client::response response = client.get(request);
		
		std::cout << response.body().c_str() << std::endl;
	}
}

}
}
