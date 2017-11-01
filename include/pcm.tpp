//audio_samples.size() cannot exceed 4294967295
//With 1-byte samples, that's ~4GB of audio.
//With 2-byte samples, ~2GB.
//With 4-byte samples, ~1GB.
template <typename SAMPLE_TYPE>
  int PCMPlayer::play_interleaved(const std::vector<SAMPLE_TYPE>& audio_samples)
{
  if (sizeof(SAMPLE_TYPE) == (snd_pcm_format_physical_width(input_params.format_type) / 8))
  {
    if (audio_samples.size() == 0)
    {
      handle_error_code(static_cast<int>(std::errc::bad_file_descriptor), false, "Provided audio sample vector was empty.");
      return static_cast<int>(std::errc::bad_file_descriptor);
    }

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
            err = snd_pcm_writei(pcm_handle, (void*)&(audio_samples[i * period_size]), (frames_remaining - current_frames_written));

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
            err = snd_pcm_writei(pcm_handle, (void*)&(audio_samples[i * period_size]), (period_size - current_frames_written));

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

template <typename SAMPLE_TYPE>
  int PCMRecorder::record_interleaved(const std::vector<SAMPLE_TYPE>& audio_samples)
{
  return 0;
}

template <typename SAMPLE_TYPE>
  int PCMRecorder::record_noninterleaved(const std::vector<std::vector<SAMPLE_TYPE>>& audio_samples)
{
  return 0;
}
