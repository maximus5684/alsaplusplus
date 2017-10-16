#include <pcm.hpp>

using namespace AlsaPlusPlus;

PCMDevice::PCMDevice(std::string hw_device, snd_pcm_stream_t stream_type) :
  err(0),
  device_name(hw_device),
  hw_params_alloc(false)
{
  if ((err = snd_pcm_open(&pcm_handle, device_name.c_str(), stream_type, 0)) < 0)
    handle_error_code(err, true, "Cannot open handle to PCM audio device.");
}

PCMDevice::~PCMDevice()
{
  if (hw_params_alloc)
    snd_pcm_hw_params_free(hw_params);

  snd_pcm_close(pcm_handle);
}

int PCMDevice::set_hardware_params(HwParams params)
{
  snd_pcm_state_t hw_state = snd_pcm_state(pcm_handle);

  if (hw_state == SND_PCM_STATE_OPEN)
  {
    input_params = params;
    frame_size = snd_pcm_format_physical_width(input_params.format_type) * static_cast<int>(input_params.channels);

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    {
      handle_error_code(err, false, "Cannot allocate hardware parameter structure for PCM object.");
      return err;
    }
    else
    {
      hw_params_alloc = true;
    }

    if ((err = snd_pcm_hw_params_any(pcm_handle, hw_params)) < 0)
    {
      handle_error_code(err, false, "Cannot initialize hardware parameter structure for PCM object.");
      return err;
    }

    if ((err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, input_params.access_type)) < 0)
    {
      handle_error_code(err, false, "Cannot set access type for PCM object.");
      return err;
    }

    if ((err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, input_params.format_type)) < 0)
    {
      handle_error_code(err, false, "Cannot set sample format for PCM object.");
      return err;
    }

    if ((err = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, static_cast<int>(input_params.channels))) < 0)
    {
      handle_error_code(err, false, "Cannot set channel count for PCM object.");
      return err;
    }

    unsigned int req_rate = input_params.sample_rate_hz;

    if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &req_rate, 0)) < 0)
    {
      handle_error_code(err, false, "Cannot set sample rate for PCM object.");
      return err;
    }
    else
    {
      if (req_rate != input_params.sample_rate_hz)
      {
        std::cout << "WARNING: Selected sample rate does not match requested. Requested: " << req_rate << "Hz, got ";
        std::cout << input_params.sample_rate_hz << "Hz." << std::endl;
      }

      input_params.sample_rate_hz = req_rate;
    }

    if ((err = snd_pcm_hw_params_set_period_time_near(pcm_handle, hw_params, &input_params.period_time_us, 0)) < 0)
    {
      handle_error_code(err, false, "Cannot set period time for PCM object.");
      return err;
    }

    snd_pcm_uframes_t psize;

    if ((err = snd_pcm_hw_params_get_period_size(hw_params, &psize, 0)) < 0)
    {
      handle_error_code(err, false, "Could not get period size for PCM object.");
      return err;
    }
    else
    {
      period_size = psize;
    }

    if ((err = snd_pcm_hw_params(pcm_handle, hw_params)) < 0)
    {
      handle_error_code(err, false, "Cannot apply hardware parameters to PCM device.");
      return err;
    }

    snd_pcm_hw_params_free(hw_params);
    hw_params_alloc = false;
  }
  else
  {
    std::ostringstream oss;
    oss << "Could not configure PCM device - device in state " << snd_pcm_state_name(hw_state) << " instead of SND_PCM_STATE_OPEN.";
    handle_error_code(static_cast<int>(std::errc::bad_file_descriptor), false, oss.str());
  }

  return 0;
}

int PCMDevice::xrun_recovery()
{
  if (err == -EPIPE)
  {
    err = snd_pcm_prepare(pcm_handle);

    if (err < 0)
    {
      handle_error_code(err, false, "Attempt to recover from underrun failed.");
      return 0;
    }
  }
  else if (err == -ESTRPIPE)
  {
    while ((err = snd_pcm_resume(pcm_handle)) == -EAGAIN)
      sleep(1); //Try once a second until suspend is released.

    if (err < 0)
    {
      err = snd_pcm_prepare(pcm_handle);

      if (err < 0)
      {
        handle_error_code(err, false, "Cannot recover from suspend during read.");
      }
    }

    return 0;
  }

  return err;
}

PCMPlayer::PCMPlayer(std::string hw_device) :
  PCMDevice(hw_device, SND_PCM_STREAM_PLAYBACK)
{
}

//audio_samples.size() cannot exceed 4294967295
//With 1-byte samples, that's ~4GB of audio.
//With 2-byte samples, ~2GB.
//With 4-byte samples, ~1GB.
template <typename SAMPLE_TYPE>
  int PCMPlayer::play_interleaved(const std::vector<SAMPLE_TYPE>& audio_samples)
{
  if (sizeof(SAMPLE_TYPE) == snd_pcm_format_physical_width(input_params.format_type))
  {
    snd_pcm_state_t hw_state = snd_pcm_state(pcm_handle);

    if (hw_state == SND_PCM_STATE_PREPARED || hw_state == SND_PCM_STATE_RUNNING)
    {
      //Because frame size should equal sizeof(SAMPLE_TYPE),
      //we should be able to send a number of SAMPLE_TYPE objects = period_size
      //to fill the PCM buffer exactly.
      unsigned int periods = static_cast<unsigned int>((float)audio_samples.size() / (float)period_size);

      if (audio_samples.size() % periods > 0)
        periods += 1;

      size_t frames_remaining = audio_samples.size();
      size_t current_frames_written;

      for (auto i = 0; i < periods; i++)
      {
        current_frames_written = 0;

        //TODO: Support MMAP access.
        if (frames_remaining < period_size)
        {
          while (current_frames_written < frames_remaining)
          {
            err = snd_pcm_writei(pcm_handle, audio_samples[i * period_size], (frames_remaining - current_frames_written));

            if (err == -EAGAIN)
              continue; //Try again.

            if (err < 0)
            {
              //Try to recover
              int xrun_err;

              if ((xrun_err = xrun_recovery()) < 0)
              {
                handle_error_code(xrun_err, false, "Read error.");
                return err;
              }
              else
              {
                break; //Recovered - skip period.
              }
            }

            current_frames_written -= err;
            frames_remaining -= err;
          }
        }
        else
        {
          while (current_frames_written < period_size)
          {
            err = snd_pcm_writei(pcm_handle, audio_samples[i * period_size], (period_size - current_frames_written));

            if (err == -EAGAIN)
              continue; //Try again.

            if (err < 0)
            {
              //Try to recover
              int xrun_err;

              if ((xrun_err = xrun_recovery()) < 0)
              {
                handle_error_code(xrun_err, false, "Read error.");
                return err;
              }
              else
              {
                break; //Recovered - skip period.
              }
            }

            current_frames_written -= err;
            frames_remaining -= err;
          }
        }
      }
    }
    else
    {
      std::ostringstream oss;
      oss << "Could not start playback - device in state " << snd_pcm_state_name(hw_state) << " instead of SND_PCM_STATE_PREPARED or SND_PCM_STATE_RUNNING.";
      handle_error_code(static_cast<int>(std::errc::bad_file_descriptor), false, oss.str());
      return static_cast<int>(std::errc::bad_file_descriptor);
    }
  }
  else
  {
    handle_error_code(static_cast<int>(std::errc::invalid_argument), false, "The datatype of the provided audio vector did not match the configured stream format.");
    return static_cast<int>(std::errc::invalid_argument);
  }

  return 0;
}

template <typename SAMPLE_TYPE>
  int PCMPlayer::play_noninterleaved(const std::vector<std::vector<SAMPLE_TYPE>>& audio_streams)
{
  return 0;
}

PCMRecorder::PCMRecorder(std::string hw_device) :
  PCMDevice(hw_device, SND_PCM_STREAM_CAPTURE)
{
}

template <typename SAMPLE_TYPE>
  int PCMRecorder::record_interleaved(std::vector<SAMPLE_TYPE>& audio_samples)
{
  return 0;
}

template <typename SAMPLE_TYPE>
  int PCMRecorder::record_noninterleaved(std::vector<std::vector<SAMPLE_TYPE>>& audio_samples)
{
  return 0;
}
