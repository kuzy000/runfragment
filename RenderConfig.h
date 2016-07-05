#pragma once

#include <string>
#include <array>
#include <ostream>

#include "Channel.h"

namespace RunFragment {

struct RenderConfig {
	RenderConfig(std::string path)
		: path {std::move(path)} {}
	
	std::string path;
	std::array<std::unique_ptr<Channel>, 4> channels;
};

}

