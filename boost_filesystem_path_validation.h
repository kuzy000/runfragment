#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options/value_semantic.hpp>

namespace boost {
namespace filesystem {

void validate(boost::any& value, const std::vector<std::string>& args, path*, int) {
	namespace po = boost::program_options;
	
	const std::string& arg = po::validators::get_single_string(args);
	value = boost::any {absolute(path {arg})};
}

}
}

