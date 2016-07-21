#pragma once

#include <string>
#include <boost/network/uri.hpp>
#include <boost/program_options.hpp>

namespace RunFragment {

class AllowedURI : public boost::network::uri::uri {
public:
	using uri::uri;
	virtual ~AllowedURI() = default;
	
	static AllowedURI* fromUriRaw(std::string location);
};

class ShadertoyURI : public AllowedURI {
public:
	using AllowedURI::AllowedURI;
};

class GLSLSandboxURI : public AllowedURI {
	using AllowedURI::AllowedURI;
};

void validate(boost::any& value, const std::vector<std::string>& args, AllowedURI**, int);

}

