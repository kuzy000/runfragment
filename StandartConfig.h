#pragma once

#include <string>

namespace RunFragment {
namespace StandartConfig {

const std::string shaderToy = 
R"raw(
time = 1
main = mainImage
add-uniforms = true

iResolution = iResolution
iGlobalTime = iGlobalTime
iGlobalDelta = iGlobalDelta
iGlobalFrame = iGlobalFrame
iChannelTime = iChannelTime
iMouse = iMouse
iDate = iDate
iSampleRate = iSampleRate
iChannelResolution = iChannelResolution
iChannel = iChannel
iSurfacePosition = iSurfacePosition
)raw";

const std::string glslSandbox = 
R"raw(
time = 1
main = none
add-uniforms = false

iResolution = resolution
iGlobalTime = time
iGlobalDelta = iGlobalDelta
iGlobalFrame = iGlobalFrame
iChannelTime = iChannelTime
iMouse = mouse
iDate = iDate
iSampleRate = iSampleRate
iChannelResolution = iChannelResolution
iChannel = iChannel
iSurfacePosition = surfacePosition
)raw";

const std::string& defaultConfig = shaderToy;

}
}

