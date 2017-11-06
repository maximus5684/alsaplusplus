#include <cstdio>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include <alsaplusplus/pcm.hpp>

using namespace AlsaPlusPlus;

static const uint16_t BUFFER_SIZE = 4096;

struct WavHeader
{
  /* RIFF Chunk Descriptor */
	uint8_t         RIFF[4];          // RIFF Header Magic header
	uint32_t        chunk_size;       // RIFF Chunk Size
	uint8_t         WAVE[4];          // WAVE Header
	/* "fmt" sub-chunk */
	uint8_t         fmt[4];           // FMT header
	uint32_t        subchunk1size;    // Size of the fmt chunk
	uint16_t        audio_format;     // Audio format 1=PCM,6=mulaw,7=alaw,
                                    // 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
	uint16_t        num_of_chan;      // Number of channels 1=Mono 2=Sterio
	uint32_t        samples_per_sec;  // Sampling Frequency in Hz
	uint32_t        bytes_per_sec;    // bytes per second
	uint16_t        block_align;      // 2=16-bit mono, 4=16-bit stereo
	uint16_t        bits_per_sample;  // Number of bits per sample
	/* "data" sub-chunk */
	uint8_t         subchunk2id[4];   // "data"  string
	uint32_t        subchunk2size;    // Sampled data length
};

int getFileSize(std::ifstream& in_file)
{
  int file_size = 0;

  in_file.seekg(0, in_file.end);
  file_size = in_file.tellg();
  in_file.seekg(0, in_file.beg);

  return file_size;
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Not enough arguments provided." << std::endl;
    return -1;
  }
  else if (argc > 3)
  {
    std::cerr << "Too many arguments provided." << std::endl;
    return -1;
  }

  WavHeader wav_hdr;
  int header_size = sizeof(wav_hdr);
  int file_length = 0;
  const char* file_path;

  file_path = argv[1];

  std::ifstream wav_file(file_path, std::ios::in | std::ifstream::binary);

  if (!wav_file.good())
  {
    std::cerr << "Unable to open WAV file " << file_path << "." << std::endl;
    return -2;
  }

  wav_file.read((char*)&wav_hdr, header_size);

  if (wav_file.good())
  {
    uint16_t bytes_per_sample = wav_hdr.bits_per_sample / 8;            // Number of bytes per sample
    uint64_t num_samples = wav_hdr.chunk_size / bytes_per_sample;       // Number of samples in file
    file_length = getFileSize(wav_file);                                // Size of the file

    if (wav_hdr.audio_format != 1)
    {
      std::cerr << "This application only accepts PCM-formatted WAV files." << std::endl;
      wav_file.close();
      return -3;
    }

    std::cout << "\nWe read a WAV file header! It had " << header_size << " bytes. Here's what it said:" << std::endl;
    std::cout << "\n";
    std::cout << "\tFile is:          " << file_length << " bytes." << std::endl;
    std::cout << "\tRIFF header:      " << wav_hdr.RIFF[0] << wav_hdr.RIFF[1] << wav_hdr.RIFF[2] << wav_hdr.RIFF[3] << std::endl;
    std::cout << "\tWAVE header:      " << wav_hdr.WAVE[0] << wav_hdr.WAVE[1] << wav_hdr.WAVE[2] << wav_hdr.WAVE[3] << std::endl;
    std::cout << "\tFMT:              " << wav_hdr.fmt[0] << wav_hdr.fmt[1] << wav_hdr.fmt[2] << wav_hdr.fmt[3] << std::endl;
    std::cout << "\tData size:        " << wav_hdr.chunk_size << std::endl;
    std::cout << "\n";
    std::cout << "\tSampling rate:    " << wav_hdr.samples_per_sec << std::endl;
    std::cout << "\tBits per sample:  " << wav_hdr.bits_per_sample << std::endl;
    std::cout << "\tChannels:         " << wav_hdr.num_of_chan << std::endl;
    std::cout << "\tBytes per second: " << wav_hdr.bytes_per_sec << std::endl;
    std::cout << "\tData length:      " << wav_hdr.subchunk2size << std::endl;
    std::cout << "\tAudio Format:     " << wav_hdr.audio_format << std::endl;
    std::cout << "\n";
    std::cout << "\tBlock align:      " << wav_hdr.block_align << std::endl;
    std::cout << "\tData string:      " << wav_hdr.subchunk2id[0] << wav_hdr.subchunk2id[1] << wav_hdr.subchunk2id[2];
    std::cout << wav_hdr.subchunk2id[3];
    std::cout << "\n" << std::endl;

    std::cout << "Setting up the PCM player...\n" << std::endl;

    PCMPlayer player("default");
    HwParams params;

    params.access_type = SND_PCM_ACCESS_RW_INTERLEAVED; // PCM audio is always interleaved.

    // WAV data is always stored in Little-Endian byte order so we choose
    // the format based on the bits per sample. <=8 bits/sample is unsigned,
    // >8 bits/sample is signed. See
    // http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/Docs/riffmci.pdf
    switch (wav_hdr.bits_per_sample)
    {
      case 8:
        params.format_type = SND_PCM_FORMAT_U8;
        std::cout << "Format type is SND_PCM_FORMAT_U8." << std::endl;
        break;
      case 16:
        params.format_type = SND_PCM_FORMAT_S16_LE;
        std::cout << "Format type is SND_PCM_FORMAT_S16_LE." << std::endl;
        break;
      case 24:
        params.format_type = SND_PCM_FORMAT_S24_LE;
        std::cout << "Format type is SND_PCM_FORMAT_S24_LE." << std::endl;
        break;
      case 32:
        params.format_type = SND_PCM_FORMAT_S32_LE;
        std::cout << "Format type is SND_PCM_FORMAT_S32_LE." << std::endl;
        break;
      default:
        std::cerr << "The number of bits per sample in the file is non-standard." << std::endl;
        return -4;
        break;
    }

    params.sample_rate_hz = wav_hdr.samples_per_sec;
    params.channels = static_cast<AudioChannels>(wav_hdr.num_of_chan);

    // To calculate the period time in microseconds, we take the source
    // bytes per second divided by the size of the buffer (in bytes) to get the number
    // of buffer fills (periods) required per second. We then divide 1,000,000
    // (number of microseconds in a second) by the periods per second to get the
    // number of microseconds for each period (approximate).
    float periods_per_sec = (wav_hdr.bytes_per_sec / (BUFFER_SIZE / (wav_hdr.bits_per_sample / 8)));
    params.period_time_us = 1000000.0 / periods_per_sec; // Microseconds per period (requested)

    player.set_hardware_params(params);

    std::cout << "Now we'll play the file.\n" << std::endl;

    std::vector<uint8_t> u8_buffer;
    std::vector<int16_t> s16_buffer;
    std::vector<int32_t> s32_buffer;

    switch (wav_hdr.bits_per_sample)
    {
      case 8:
      {
        u8_buffer.reserve(BUFFER_SIZE / sizeof u8_buffer[0]);

        /*
        while ((bytes_read = fread(&(u8_buffer[0]), sizeof u8_buffer[0], BUFFER_SIZE / (sizeof u8_buffer[0]), wav_file)) > 0)
        {
          std::cout << "Bytes read: " << bytes_read << std::endl;
          player.play_interleaved<uint8_t>(u8_buffer);
        }
        */
      } break;
      case 16:
      {
        s16_buffer.reserve(BUFFER_SIZE / sizeof s16_buffer[0]);

        /*
        while ((bytes_read = fread(&(s16_buffer[0]), sizeof s16_buffer[0], BUFFER_SIZE / (sizeof s16_buffer[0]), wav_file)) > 0)
        {
          std::cout << "Bytes read: " << bytes_read << std::endl;
          player.play_interleaved<int16_t>(s16_buffer);
        }
        */
      } break;
      case 24:
      case 32:
      {
        s32_buffer.reserve(BUFFER_SIZE / sizeof s32_buffer[0]);

        /*
        while ((bytes_read = fread(&(s32_buffer[0]), sizeof s32_buffer[0], BUFFER_SIZE / (sizeof s32_buffer[0]), wav_file)) > 0)
        {
          std::cout << "Bytes read: " << bytes_read << std::endl;
          player.play_interleaved<int32_t>(s32_buffer);
        }
        */
      } break;
    }
  }
  else
  {
    std::cout << "Failed to read header from file." << std::endl;
  }

  wav_file.close();

  std::cout << "Done!\n" << std::endl;

  return 0;
}
