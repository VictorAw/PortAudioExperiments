#ifndef VAW_SINE_NOTES_AUDIO_STREAM_H
#define VAW_SINE_NOTES_AUDIO_STREAM_H

//#include "Note.h"

#include <portaudio.h>

namespace vaw::sine_notes {

constexpr int SAMPLE_RATE = 44000;

class AudioStream {
public:
  AudioStream();

  bool Open(PaDeviceIndex device);
  bool Close();

  bool Start();
  bool Stop();

  bool SetVolume(float percent);
  bool SetFrequency(float frequency);
  //bool PlayNote(Note const & note);

private:
  typedef int PortAudioCallback(void const * input_buffer, void * output_buffer,
                                unsigned long frames_per_buffer,
                                PaStreamCallbackTimeInfo const * time_info,
                                PaStreamCallbackFlags status_flags,
                                void * user_data);
  PortAudioCallback AudioCallback;
  static PortAudioCallback AudioCallbackEntry;

  void FinishedCallback();
  static void FinishedCallbackEntry(void * user_data);

  static constexpr int TABLE_SIZE = (44000 / SAMPLE_RATE) * 100;
  static float a440_[TABLE_SIZE];
  static bool a440_generated_;
  PaStream * stream_ = nullptr;
  // Channel phases
  float left_phase_ = 0.0f;
  float right_phase_ = 0.0f;

  // Scalars to convert A440 to the requested note frequency
  float left_scalar_ = 1.0f;
  float right_scalar_ = 1.0f;

  float volume_scalar_ = 1.0f;
};

}  // namespace vaw::sine_notes

#endif  // VAW_SINE_NOTES_AUDIO_STREAM_H

