#ifndef VAW_ADD_NOTES_AUDIO_STREAM_H
#define VAW_ADD_NOTES_AUDIO_STREAM_H

#include "Note.h"

#include <portaudio.h>

namespace vaw::add_notes {

constexpr int SAMPLE_RATE = 44000;

class AudioStream {
public:
  AudioStream();

  bool Open(PaDeviceIndex device);
  bool Close();

  bool Start();
  bool Stop();

  bool SetVolume(float percent);
  bool AddNotes(std::vector<Note> && notes);

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
  float volume_ = 1.0f;

  double playback_start_ = 0.0;
};

}  // namespace vaw::add_notes

#endif  // VAW_ADD_NOTES_AUDIO_STREAM_H

