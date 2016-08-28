#include "Channel.h"

#include <array>
#include <algorithm>
#include <regex>
#include <iostream>

#include <boost/filesystem/operations.hpp>

#include "Option.h"

namespace RunFragment {

std::unique_ptr<Channel> Channel::fromString(std::string str) {
	return std::unique_ptr<Channel> {Channel::fromStringRaw(std::move(str))};
}

Channel* Channel::fromStringRaw(std::string str) {
	const auto& bufs = Option::bufs;
	const auto it = std::find(bufs.begin(), bufs.end(), std::move(str));

	if(it != bufs.end()) {
		const auto kind = static_cast<ChannelBuf::Kind>(std::distance(bufs.begin(), it));
		
		return new ChannelBuf {kind};
	}
	
	const std::regex re {"^(.*\\.)(png|jpg|jpeg|tiff|gif|tga)(:flip)?$"};
	
	std::smatch match;
	if(std::regex_match(str, match, re)) {
		const bool flipped = match.str(3) != "";
		const auto path = match.str(1) + match.str(2);
		
		return new ChannelImage {fs::absolute(fs::path{path}), flipped};
	}

	return nullptr;
}

void validate(boost::any& value, const std::vector<std::string>& args, Channel**, int) {
	namespace po = boost::program_options;
	
	const std::string& arg = po::validators::get_single_string(args);
	
	if(arg == Option::none) {
		value = boost::any {static_cast<Channel*>(nullptr)};
		return;
	}
	
	const auto channel = Channel::fromStringRaw(arg);
	
	if(channel) {
		value = boost::any {std::move(channel)};
	}
	else {
		throw po::validation_error {po::validation_error::invalid_option_value};
	}
}

}
