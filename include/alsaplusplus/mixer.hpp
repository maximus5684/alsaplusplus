#ifndef ALSAPLUSPLUS_MIXER_HPP
#define ALSAPLUSPLUS_MIXER_HPP

#include <alsaplusplus/common.hpp>
#include <alsa/control.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>

#include <cmath>

namespace AlsaPlusPlus
{
  class Mixer
  {
    public:
      Mixer(std::string hw_device);
      ~Mixer();

      int inc_vol_pct(float pct, float* set_to, snd_mixer_selem_channel_id_t channel = SND_MIXER_SCHN_MONO);
      int dec_vol_pct(float pct, float* set_to, snd_mixer_selem_channel_id_t channel = SND_MIXER_SCHN_MONO);
      int set_vol_pct(float pct, float* set_to);
      int mute();
      int unmute();

    private:
      int err;
      std::string device_name;
      snd_mixer_t* mixer_handle;
      snd_mixer_selem_id_t* simple_mixer_handle;
      std::string simple_elem_name = "Master";
      snd_mixer_elem_t* element_handle;
      long min_vol, max_vol, cur_vol;
      long mute_vol;

      void trim_pct(float& pct);
      int set_vol_pct(float pct);
      int get_vol_range();
      int get_cur_vol(snd_mixer_selem_channel_id_t channel);
  };
}

#endif
