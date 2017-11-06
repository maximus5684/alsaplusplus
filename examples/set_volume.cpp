#include <cstdio>

#include <cxxopts.hpp>
#include <alsaplusplus/mixer.hpp>

using namespace AlsaPlusPlus;

int main(int argc, char** argv)
{
  try
  {
    cxxopts::Options options("set_volume", "Sets master volume using the Mixer interface of AlsaPlusPlus.");
    options.positional_help("[volume pct]");
    
    options.add_options()
      ("d,decrease", "Decrease volume by given percent.", cxxopts::value<float>())
      ("h,help", "Print help.")
      ("i,increase", "Increase volume by given percent.", cxxopts::value<float>())
      ("v,volume", "Set volume to given percent", cxxopts::value<float>());

    options.parse_positional("volume");
    options.parse(argc, argv);

    if (options.count("help"))
    {
      std::cout << "\n" << options.help({"", "Group"}) << std::endl;
      return 0;
    }

    Mixer m("default", "Master");
    float set_as;

    if (options.count("decrease"))
    {
      set_as = m.dec_vol_pct(options["decrease"].as<float>());
      std::cout << "Volume was set to " << set_as * 100.0 << "%." << std::endl;
    }
    else if (options.count("increase"))
    {
      set_as = m.inc_vol_pct(options["increase"].as<float>());
      std::cout << "Volume was set to " << set_as * 100.0 << "%." << std::endl;
    }
    else if (options.count("volume"))
    {
      set_as = m.set_vol_pct(options["volume"].as<float>());
      std::cout << "Volume was set to " << set_as * 100.0 << "%." << std::endl;
    }
    else
    {
      std::cout << "\n" << options.help({"", "Group"}) << std::endl;
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
