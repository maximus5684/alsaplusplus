#ifndef ALSAPLUSPLUS_COMMON_HPP
#define ALSAPLUSPLUS_COMMON_HPP

//C
extern "C"
{
#include <unistd.h>
#include <cstdio>
#include <cstring>
}

//C++
#include <memory>
#include <iostream>
#include <vector>
#include <sstream>

//ALSA
extern "C"
{
#include <alsa/global.h>
#include <alsa/asoundef.h>
#include <alsa/error.h>
#include <alsa/input.h>
#include <alsa/output.h>
#include <alsa/conf.h>
}

#include <alsaplusplus/error.hpp>

namespace AlsaPlusPlus
{
  enum class AudioChannels :
    int
  {
    MONO = 1,
    STEREO = 2,
    STEREO_PLUS_SUB = 3,
    STEREO_SURROUND = 4,
    FULL_SURROUND = 5,
    FULL_SURROUND_PLUS_SUB = 6
  };
}

#endif
