#include "Download.h"

#include <regex>
#include <fstream>
#include <vector>
#include <unordered_map>

#define BOOST_NETWORK_ENABLE_HTTPS
#include <boost/network/include/http/client.hpp>

#include <boost/filesystem.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include "ShadertoyJSON.h"

namespace RunFragment {
namespace Download {

void fromURI(const RunFragment::AllowedURI* location) {
	using namespace boost::network;
	using namespace rapidjson;
	
	if(dynamic_cast<const GLSLSandboxURI*>(location)) {
		const auto fragment = location->fragment();
		uri::uri url {"http://glslsandbox.com/item/" + fragment};
		
		http::client client;
		http::client::request request {url};
		
		std::cout << "Downloading using the GLSLSandbox API..." << std::endl;
		const http::client::response response = client.get(request);

		
		Document document;
		if(!document.Parse(response.body().c_str()).HasParseError()) {
			const auto code = document["code"].GetString();
			
			const std::string filename = fragment + ".glsl";
			std::cout << "Saving Image to: " << filename << std::endl;
			std::ofstream ofs {filename.c_str()};
			ofs << "// Downloaded from " << location->string() << std::endl << std::endl;
			ofs << code << std::endl;
		}
	}
	else if(dynamic_cast<const ShadertoyURI*>(location)) {
		const std::string key {"Nd8KwM"};
		const auto path = location->path();
		
		const std::regex shaderIdRegex {"/view/(.*)"};
		std::smatch shaderIdSmatch;
		
		std::regex_search(path.begin(), path.end(), shaderIdSmatch, shaderIdRegex);
		
		const std::string shaderId = shaderIdSmatch[1];
		
		const uri::uri url {"https://www.shadertoy.com/api/v1/shaders/" + shaderId + "?key=" + key};
		
		http::client client;
		http::client::request request {url};
		
		std::cout << "Downloading using the Shadertoy.com API..." << std::endl;
		const http::client::response response = client.get(request);
		
		
		Document document;
		if(!document.Parse(response.body().c_str()).HasParseError()) {
			namespace fs = boost::filesystem;
			
			if(document["Error"].IsString()) {
				std::cout << "Shadertoy.com API error: " << document["Error"].GetString() << std::endl;
				return;
			}
		
			const ShadertoyJSON data {document};
			
			const fs::path dir {data.info.name + " " + shaderId};
			const fs::path config {dir / "config.ini"};
			
			if(fs::create_directory(dir)) {
				std::cout << "Saving project to: " << dir << std::endl;
				std::cout << "    config: " << config << std::endl;
				
				fs::ofstream ofsConfig {config};
				ofsConfig << "# " << data.info.name << std::endl;
				ofsConfig << "# Created by " << data.info.username << std::endl;
				ofsConfig << "# Downloaded from " << location->string() << std::endl;
				ofsConfig << std::endl;
				
				ofsConfig << "format = s" << std::endl;
				ofsConfig << std::endl;
				
				const std::unordered_map<std::string, std::pair<std::string, fs::path>> nameBufPathMap {
					{"Image", {"Image", fs::path {"Image.glsl"}}},
					{"Buf A", {"BufA",  fs::path {"BufA.glsl"}}},
					{"Buf B", {"BufB",  fs::path {"BufB.glsl"}}},
					{"Buf C", {"BufC",  fs::path {"BufC.glsl"}}},
					{"Buf D", {"BufD",  fs::path {"BufD.glsl"}}},
				};
				std::unordered_map<std::size_t, std::string> idBufferMap;
				
				for(const auto& pass : data.renderpass) {
					const std::string name = name == "" ? "Image" : pass.name;
					
					const auto it = nameBufPathMap.find(name);
					if(it != nameBufPathMap.end()) {
						const auto& buf  = it->second.first;
						const auto& path = it->second.second;
						
						std::cout << "    " << buf << ": " << dir / path << std::endl;
						ofsConfig << buf << " = " << path.filename().string() << std::endl;
						
						fs::ofstream code {dir / path};
						code << pass.code << std::endl;
						
						for(const auto& output : pass.outputs) {
							idBufferMap.emplace(output.id, buf);
						}
					}
					else {
						std::cerr << "Error: unsupported buffer type '" << pass.name << "'" << std::endl;
					}
				}
				ofsConfig << std::endl;
				
				std::size_t textureCounter = 0;
				std::unordered_map<std::string, fs::path> downloadPathDirPath;
				
				for(const auto& pass : data.renderpass) {
					const std::string name = name == "" ? "Image" : pass.name;
					
					const auto it = nameBufPathMap.find(name);
					if(it != nameBufPathMap.end()) {
						const auto& buf  = it->second.first;
						
						for(const auto& input : pass.inputs) {
							if(input.ctype == "buffer") {
								// FIXME: remove at
								ofsConfig << buf << "-c" << input.channel << " = " << idBufferMap.at(input.id) << std::endl;
							}
							else if(input.ctype == "texture") {
								const std::string src = input.src;
								const fs::path path = src.substr(src.find_last_of('/') + 1);
								
								ofsConfig << buf << "-c" << input.channel << " = " << path.filename().string();
								
								downloadPathDirPath.emplace(std::move(src), std::move(path));
								if(input.sampler.vflip == "true") {
									ofsConfig << ":flip";
								}
								ofsConfig << std::endl;
							}
							else {
								std::cerr << "Error: unsupported channel type: '" << input.ctype << "'" << std::endl;
							}
						}
					}
				}
				const auto wget = [&dir] (std::string src, fs::path path) {
					http::client client;
					http::client::request request {"https://www.shadertoy.com" + src};
					const http::client::response response = client.get(request);

					std::cout << "    " << src << ": " << path << std::endl;
					fs::ofstream ofs {dir / path, std::ios::out | std::ios::binary};
					ofs << body(response) << std::endl;
				};
				
				for(const auto& p : downloadPathDirPath) {
					wget(p.first, p.second);
				}
				
			}
			else {
				std::cerr << "Error: can't create the project directory" << std::endl;
			}
		}
	}
}

}
}
