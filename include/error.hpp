#ifndef ALSAPLUSPLUS_ERROR_HPP
#define ALSAPLUSPLUS_ERROR_HPP

#include <iostream>
#include <system_error>

extern "C"
{
#include <alsa/error.h>
}

namespace AlsaPlusPlus
{
  void handle_error_code(int err_code, bool throws, std::string error_desc);
}

#endif
