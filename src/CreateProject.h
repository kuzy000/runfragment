#pragma once

#include <boost/filesystem/path.hpp>

namespace RunFragment {
namespace CreateProject {

namespace fs = boost::filesystem;

void glslsandbox(fs::path path);
void shadertoy(fs::path path);

}
}

