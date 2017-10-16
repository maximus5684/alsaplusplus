#include <error.hpp>

void AlsaPlusPlus::handle_error_code(int err_code, bool throws, std::string error_desc)
{
  std::cerr << error_desc << " (ALSA Description: " << snd_strerror(err_code) << ")" << std::endl;

  if (throws)
  {
    std::error_code ec(err_code, std::generic_category());
    std::system_error se(ec, error_desc);

    throw se;
  }
}
