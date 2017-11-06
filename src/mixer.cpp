#include <alsaplusplus/mixer.hpp>

using namespace AlsaPlusPlus;

Mixer::Mixer(std::string hw_device) :
  err(0),
  device_name(hw_device)
{
  if ((err = snd_mixer_open(&mixer_handle, 0)) < 0)
    handle_error_code(err, true, "Cannot open handle to mixer device.");

  if ((err = snd_mixer_attach(mixer_handle, device_name.c_str())) < 0)
    handle_error_code(err, true, "Cannot attach mixer to device.");

  if ((err = snd_mixer_selem_register(mixer_handle, NULL, NULL)) < 0)
    handle_error_code(err, true, "Cannot register simple mixer object.");

  if ((err = snd_mixer_load(mixer_handle)) < 0)
    handle_error_code(err, true, "Cannot load sound mixer.");

  snd_mixer_selem_id_alloca(&simple_mixer_handle);
  snd_mixer_selem_id_set_index(simple_mixer_handle, 0);
  snd_mixer_selem_id_set_name(simple_mixer_handle, simple_elem_name.c_str());
  element_handle = snd_mixer_find_selem(mixer_handle, simple_mixer_handle);

  if (element_handle == NULL)
  {
    std::ostringstream oss;
    oss << "Could not find simple mixer element named " << simple_elem_name << ".";
    handle_error_code(static_cast<int>(std::errc::argument_out_of_domain), true, oss.str());
  }
}

Mixer::~Mixer()
{
  snd_mixer_close(mixer_handle);
}

int Mixer::dec_vol_pct(float pct, float* set_to, snd_mixer_selem_channel_id_t channel)
{
  trim_pct(pct);

  if ((err = get_vol_range()) < 0)
    return err;

  if ((err = get_cur_vol(channel)) < 0)
    return err;

  float new_vol = ((float)cur_vol / (max_vol - min_vol)) - pct;
  trim_pct(new_vol);
  new_vol = round(new_vol * 100.0) / 100.0;

  if ((err = set_vol_pct(new_vol)) < 0)
    return err;

  if ((err = get_cur_vol(channel)) < 0)
    return err;

  *set_to = round(((float)cur_vol / (max_vol - min_vol)) * 100.0) / 100.0;

  return 0;
}

int Mixer::inc_vol_pct(float pct, float* set_to, snd_mixer_selem_channel_id_t channel)
{
  trim_pct(pct);

  if ((err = get_vol_range()) < 0)
    return err;

  if ((err = get_cur_vol(channel)) < 0)
    return err;

  float new_vol = ((float)cur_vol / (max_vol - min_vol)) + pct;
  trim_pct(new_vol);
  new_vol = round(new_vol * 100.0) / 100.0;

  if ((err = set_vol_pct(new_vol)) < 0)
    return err;

  if ((err = get_cur_vol(channel)) < 0)
    return err;

  *set_to = round(((float)cur_vol / (max_vol - min_vol)) * 100.0) / 100.0;

  return 0;
}

int Mixer::set_vol_pct(float pct, float* set_to)
{
  if ((err = set_vol_pct(pct)) < 0)
    return err;

  if ((err = get_cur_vol(SND_MIXER_SCHN_MONO)) < 0)
    return err;

  *set_to = round(((float)cur_vol / (max_vol - min_vol)) * 100.0) / 100.0;

  return 0;
}

int Mixer::mute()
{
  if ((err = get_cur_vol(SND_MIXER_SCHN_MONO)) < 0)
    return err;

  if ((err = set_vol_pct(0)) < 0)
    return err;

  return 0;
}

void Mixer::trim_pct(float& pct)
{
  pct = (pct < 0) ? 0 : pct;
  pct = (pct > 1) ? 1 : pct;
}

int Mixer::set_vol_pct(float pct)
{
  trim_pct(pct);

  if ((err = get_vol_range()) < 0)
    return err;

  if ((err = snd_mixer_selem_set_playback_volume_all(element_handle, pct * max_vol)) < 0)
  {
    handle_error_code(err, false, "Cannot set volume to requested value.");
    return err;
  }

  return 0;
}

int Mixer::get_vol_range()
{
  if ((err = snd_mixer_selem_get_playback_volume_range(element_handle, &min_vol, &max_vol)) < 0)
  {
    handle_error_code(err, false, "Cannot get min/max volume range.");
    return err;
  }
  else
  {
    return 0;
  }
}

int Mixer::get_cur_vol(snd_mixer_selem_channel_id_t channel)
{
  if ((err = snd_mixer_selem_get_playback_volume(element_handle, channel, &cur_vol)) < 0)
  {
    handle_error_code(err, false, "Could not get volume for provided channel.");
    return err;
  }

  return 0;
}
