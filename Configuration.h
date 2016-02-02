#pragma once

#include <utility>

#include <boost/optional.hpp>
#include <boost/program_options/variables_map.hpp>

namespace RunFragment {

namespace po = boost::program_options;

struct Configuration {
	enum class Format { GlslSandbox, ShaderToy };

	boost::optional<std::string> filename;
	float time;

	Format format;
	struct Uniform {
		std::string time;

		static getDefault(std::string name, Format format) {
		}
	} uniform;

	static Configuration fromVariablesMap(const po::variables_map& vm) {
		auto lookupStr = [&vm] (std::string name, std::string defvalue) {
			if(vm.count(name)) {
				return vm[name].as<std::string>();
			}
			else {
				return defvalue;
			}
		};

		Configuration result;
		result.filename = vm.count("file") ? boost::optional<std::string> {vm["file"].as<std::string>()}
		                                   : boost::none;
		result.time = vm.count("time") ? vm["time"].as<float>() : 1.0f;
		result.format = lookupStr("format", "s") == "s" ? Format::ShaderToy
		              : lookupStr("format", "s") == "g" ? Format::GlslSandbox
		              : throw std::runtime_error("unvalid --format argument");

		result.uniform.time = lookupStr("utime", Format::ShaderToy)

		return result;
	}
};

}

