#pragma once

#include <array>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "Channel.h"

namespace RunFragment {

namespace fs = boost::filesystem;

struct RenderConfig {
	RenderConfig(fs::path path)
		: path {std::move(path)} {}
	
	RenderConfig(RenderConfig&&) = default;
	RenderConfig& operator =(RenderConfig&&) = default;
	
	RenderConfig(const RenderConfig& other)
		: path {other.path}
		, channels {[&other] {
			std::array<std::unique_ptr<Channel>, 4> result;
			
			for(std::size_t i = 0; i < other.channels.size(); i++) {
				result[i] = std::unique_ptr<Channel> {new Channel {*other.channels[i]}};
			}
			
			return result;
		} ()} {}
		
	RenderConfig& operator =(const RenderConfig& rhs) {
		path = rhs.path;
		
		for(std::size_t i = 0; i < channels.size(); i++) {
			channels[i] = std::unique_ptr<Channel> {new Channel {*rhs.channels[i]}};
		}
		
		return *this;
	}
	
	fs::path path;
	std::array<std::unique_ptr<Channel>, 4> channels {{nullptr}};
};

}

