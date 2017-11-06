#include <cstdio>

#include <cxxopts.hpp>
#include <mixer.hpp>

using namespace AlsaPlusPlus;

int main(int argc, char** argv)
{
  try
  {
    cxxopts::Options options("Example Application for Setting Master Volume", "Sets master volume using the Mixer interface of AlsaPlusPlus.");
    options.add_options()
      ("d,decrease", "Decrease volume by given percent.", cxxopts::value<float>())
      ("i,increase", "Increase volume by given percent.", cxxopts::value<float>())
      ("v,volume", "Set volume to given percent", cxxopts::value<float>());
    options.parse_positional("volume");
    options.parse(argc, argv);

    if (options.count("help"))
    {
      std::cout << options.help({"", "Group"}) << std::endl;
      return 0;
    }

    Mixer m("default");
    float set_as;

    if (options.count("decrease"))
    {
      if (m.dec_vol_pct(options["decrease"].as<float>(), &set_as) < 0)
        std::cerr << "Unable to set volume." << std::endl;
      else
        std::cout << "Volume was set to " << set_as * 100.0 << "%." << std::endl;
    }
    else if (options.count("increase"))
    {
      if (m.inc_vol_pct(options["increase"].as<float>(), &set_as) < 0)
        std::cerr << "Unable to set volume." << std::endl;
      else
        std::cout << "Volume was set to " << set_as * 100.0 << "%." << std::endl;
    }
    else if (options.count("volume"))
    {
      if (m.set_vol_pct(options["volume"].as<float>(), &set_as) < 0)
        std::cerr << "Unable to set volume." << std::endl;
      else
        std::cout << "Volume was set to " << set_as * 100.0 << "%." << std::endl;
    }
    else
    {
      std::cout << options.help({"", "Group"}) << std::endl;
      return 0;
    }
  }
  catch (const cxxopts::OptionException& e)
  {
    std::cerr << "Error parsing options: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
