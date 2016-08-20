#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>

#include "AppConfig.h"

namespace RunFragment {
namespace ArgumentsParser {

namespace po = boost::program_options;

po::variables_map argsToVm(int argc, char* argv[], po::options_description desc);

RunFragment::AppConfig vmToAppConfig(const po::variables_map& vm);

}
}

