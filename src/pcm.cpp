#include <pcm.hpp>

using namespace AlsaPlusPlus;

PCMDevice::PCMDevice(HwParams params) :
  input_params(params),
  pcm_handle_open(false),
  hw_params_alloc(false)
{
}

PCMDevice::~PCMDevice()
{
  if (hw_params_alloc)
    snd_pcm_hw_params_free(hw_params);

  if (pcm_handle_open)
    snd_pcm_close(pcm_handle);
}

PCMPlayer::PCMPlayer(HwParams player_params) :
  PCMDevice(player_params)
{
  if ((err = snd_pcm_open(&pcm_handle, input_params.hw_device.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    std::cerr << "Cannot open handle to PCM audio device (" << snd_strerror(err) << ")" << std::endl;
  else
    pcm_handle_open = true;

  if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    std::cerr << "Cannot allocate hardware parameter structure for PCM object (" << snd_strerror(err) << ")" << std::endl;
  else
    hw_params_alloc = true;

  if ((err = snd_pcm_hw_params_any(pcm_handle, hw_params)) < 0)
    std::cerr << "Cannot initialize hardware parameter structure for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, input_params.access_type)) < 0)
    std::cerr << "Cannot set access type for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, input_params.format_type)) < 0)
    std::cerr <<" Cannot set sample format for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &input_params.sample_rate, 0)) < 0)
    std::cerr << "Cannot set sample rate for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, static_cast<int>(input_params.channels))) < 0)
    std::cerr << "Cannot set channel count for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params(pcm_handle, hw_params)) < 0)
    std::cerr << "Cannot apply hardware parameters to PCM device (" << snd_strerror(err) << ")" << std::endl;

  snd_pcm_hw_params_free(hw_params);
  hw_params_alloc = false;
}

void PCMPlayer::play(const std::vector<unsigned char>& audio_bytes)
{
  if ((err = snd_pcm_prepare(pcm_handle)) < 0)
    std::cerr << "Cannot prepare PCM device for use (" << snd_strerror(err) << ")" << std::endl;
}

PCMRecorder::PCMRecorder(HwParams recorder_params) :
  PCMDevice(recorder_params)
{
  if ((err = snd_pcm_open(&pcm_handle, input_params.hw_device.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0)
    std::cerr << "Cannot open handle to PCM audio device (" << snd_strerror(err) << ")" << std::endl;
  else
    pcm_handle_open = true;

  if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    std::cerr << "Cannot allocate hardware parameter structure for PCM object (" << snd_strerror(err) << ")" << std::endl;
  else
    hw_params_alloc = true;

  if ((err = snd_pcm_hw_params_any(pcm_handle, hw_params)) < 0)
    std::cerr << "Cannot initialize hardware parameter structure for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, input_params.access_type)) < 0)
    std::cerr << "Cannot set access type for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, input_params.format_type)) < 0)
    std::cerr <<" Cannot set sample format for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &input_params.sample_rate, 0)) < 0)
    std::cerr << "Cannot set sample rate for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, static_cast<int>(input_params.channels))) < 0)
    std::cerr << "Cannot set channel count for PCM object (" << snd_strerror(err) << ")" << std::endl;

  if ((err = snd_pcm_hw_params(pcm_handle, hw_params)) < 0)
    std::cerr << "Cannot apply hardware parameters to PCM device (" << snd_strerror(err) << ")" << std::endl;

  snd_pcm_hw_params_free(hw_params);
  hw_params_alloc = false;
}

void PCMRecorder::record(std::vector<unsigned char>& audio_bytes)
{
  if ((err = snd_pcm_prepare(pcm_handle)) < 0)
    std::cerr << "Cannot prepare PCM device for use (" << snd_strerror(err) << ")" << std::endl;
}
