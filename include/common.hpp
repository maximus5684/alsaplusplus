#ifndef ALSAPLUSPLUS_COMMON_HPP
#define ALSAPLUSPLUS_COMMON_HPP

//C
extern "C"
{
#include <unistd.h>
#include <cstdio>
}

//C++
#include <memory>
#include <iostream>
#include <stdexcept>
#include <vector>

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

#endif
