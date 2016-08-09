#include "StandartConfig.h"

namespace RunFragment {
namespace StandartConfig {

const std::string shaderToy = 
R"raw(
time = 1
main = mainImage
add-uniforms = true

iResolution = iResolution
iGlobalTime = iGlobalTime
iTimeDelta = iTimeDelta
iFrame = iFrame
iFrameRate = iFrameRate
iChannelTime = iChannelTime
iChannelResolution = iChannelResolution
iMouse = iMouse
iDate = iDate
iSampleRate = iSampleRate
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
iTimeDelta = iTimeDelta
iFrame = iFrame
iFrameRate = iFrameRate
iChannelTime = iChannelTime
iChannelResolution = iChannelResolution
iMouse = mouse
iDate = iDate
iSampleRate = iSampleRate
iChannel = iChannel
iSurfacePosition = surfacePosition
)raw";

}
}
