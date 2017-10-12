#ifndef ALSAPLUSPLUS_MIXER_HPP
#define ALSAPLUSPLUS_MIXER_HPP

#include <common.hpp>
#include <alsa/control.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>

namespace AlsaPlusPlus
{
  class Mixer
  {
    public:
      Mixer();
      virtual ~Mixer();

    private:
      snd_mixer_t* sound_mixer_handle;
      snd_mixer_selem_id_t* simple_mixer_handle;
  };
}

#endif
