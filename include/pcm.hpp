#ifndef ALSAPLUSPLUS_PCM_HPP
#define ALSAPLUSPLUS_PCM_HPP

#include <common.hpp>
#include <alsa/pcm.h>

namespace AlsaPlusPlus
{
  struct HwParams
  {
    snd_pcm_access_t access_type;
    snd_pcm_format_t format_type;
    unsigned int sample_rate_hz;
    AudioChannels channels;
    unsigned int period_time_us;
  };

  class PCMDevice
  { 
    public:
      PCMDevice(std::string hw_device, snd_pcm_stream_t stream_type);
      ~PCMDevice();
      int set_hardware_params(HwParams params);

    protected:
      int xrun_recovery();

      int err;
      std::string device_name;
      HwParams input_params;
      snd_pcm_t* pcm_handle;
      snd_pcm_hw_params_t* hw_params;
      bool hw_params_alloc;
      unsigned long frame_size; //bytes = channels * size(audio_data_struct)
      snd_pcm_uframes_t period_size; //number of frames between interrupts
  };

  class PCMPlayer :
    public PCMDevice
  {
    public:
      PCMPlayer(std::string hw_device);

      template <typename SAMPLE_TYPE>
        int play_interleaved(const std::vector<SAMPLE_TYPE>& audio_samples);
      template <typename SAMPLE_TYPE>
        int play_noninterleaved(const std::vector<std::vector<SAMPLE_TYPE>>& audio_streams);
  };

  class PCMRecorder :
    public PCMDevice
  {
    public:
      PCMRecorder(std::string hw_device);

      template <typename SAMPLE_TYPE>
        int record_interleaved(const std::vector<SAMPLE_TYPE>& audio_samples);
      template <typename SAMPLE_TYPE>
        int record_noninterleaved(const std::vector<std::vector<SAMPLE_TYPE>>& audio_streams);
  };

  //Definitions of templated functions
  #include <pcm.tpp>

} 


#endif
