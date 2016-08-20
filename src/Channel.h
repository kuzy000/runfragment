#pragma once

#include <memory>
#include <string>
#include <boost/program_options.hpp>

#include <boost/filesystem/path.hpp>

namespace RunFragment {

namespace fs = boost::filesystem;

class Channel {
public:
	virtual ~Channel() = default;
	
	static std::unique_ptr<Channel> fromString(std::string str);
	static Channel* fromStringRaw(std::string str);
};

void validate(boost::any& value, const std::vector<std::string>& args, Channel**, int);


class ChannelBuf : public Channel {
public:
	enum class Kind { BufA = 0, BufB, BufC, BufD };
	
	ChannelBuf(Kind kind)
		: kind {kind} {}
	
	const Kind kind;
};

class ChannelImage : public Channel {
public:
	ChannelImage(fs::path path, bool flipped)
		: path {std::move(path)}, flipped {flipped} {}
	
	const fs::path path;
	const bool flipped;
};

}

