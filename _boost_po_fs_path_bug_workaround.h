#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/program_options/value_semantic.hpp>

// See: https://svn.boost.org/trac/boost/ticket/8535

namespace boost {
namespace filesystem {

void validate(boost::any& value, const std::vector<std::string>& args, path*, int) {
	namespace po = boost::program_options;
	
	const std::string& arg = po::validators::get_single_string(args);
	
	value = boost::any {path {arg}};
}

}
}

