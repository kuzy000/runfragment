#pragma once

#include "Parameter.h"

#include <array>

#include <boost/program_options/options_description.hpp>

namespace RunFragment {
namespace Option {

extern const Parameter image;
extern const std::array<Parameter, 4> imageChannels;

extern const std::array<Parameter, 4> bufs;
extern const std::array<std::array<Parameter, 4>, 4> bufChannels;

extern const Parameter config;
extern const Parameter download;
extern const Parameter time;
extern const Parameter help;
extern const Parameter format;
extern const Parameter main;
extern const Parameter addUniform;

extern const Parameter iResolution;
extern const Parameter iGlobalTime;
extern const Parameter iTimeDelta;
extern const Parameter iFrame;
extern const Parameter iFrameRate;
extern const Parameter iChannelTime;
extern const Parameter iChannelResolution;
extern const Parameter iMouse;
extern const Parameter iDate;
extern const Parameter iSampleRate;
extern const Parameter iChannel;
extern const Parameter iSurfacePosition;

extern const boost::program_options::options_description helpOptions;
extern const boost::program_options::options_description parsingOptions;

}
}
