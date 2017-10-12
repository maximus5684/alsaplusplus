#ifndef ALSAPLUSPLUS_PCM_HPP
#define ALSAPLUSPLUS_PCM_HPP

#include <common.hpp>
#include <alsa/pcm.h>

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

  struct HwParams
  {
    std::string hw_device;
    snd_pcm_access_t access_type;
    snd_pcm_format_t format_type;
    unsigned int sample_rate;
    AudioChannels channels;
  };

  class PCMDevice
  { 
    public:
      PCMDevice(HwParams params);
      ~PCMDevice();

      int err;

    protected:
      HwParams input_params;

      snd_pcm_t* pcm_handle;
      bool pcm_handle_open;

      snd_pcm_hw_params_t* hw_params;
      bool hw_params_alloc;
  };

  class PCMRecorder :
    public PCMDevice
  {
    public:
      PCMRecorder(HwParams recorder_params);
      virtual ~PCMRecorder();

      void record(std::vector<unsigned char>& audio_bytes);
  };

  class PCMPlayer :
    public PCMDevice
  {
    public:
      PCMPlayer(HwParams player_params);
      virtual ~PCMPlayer();

      void play(const std::vector<unsigned char>& audio_bytes);
  };
} 

#endif
