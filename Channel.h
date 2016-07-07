#pragma once

#include <memory>
#include <boost/program_options.hpp>

namespace RunFragment {

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

}

