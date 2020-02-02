#ifndef VAW_MULTIPLE_SINES_AUDIO_STREAM_H
#define VAW_MULTIPLE_SINES_AUDIO_STREAM_H

#include "Note.h"

#include <portaudio.h>

#include <chrono>

namespace vaw::multiple_sines {

constexpr int SAMPLE_RATE = 44000;

class AudioStream {
public:
  AudioStream();

  bool Open(PaDeviceIndex device);
  bool Close();

  bool Start();
  bool Stop();

  bool SetVolume(float percent);
  bool PlayNotes(std::vector<Note> && notes);

private:
  struct NotePlaybackState {
    float phase = 0.0f;
    float phase_step = 0.0f;
  };

  struct NotePlayback {
    std::vector<Note> notes;
    std::vector<NotePlaybackState> playback;

    std::size_t size;
  };

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

  NotePlayback active_notes_;
  std::chrono::system_clock::time_point playback_start_;
};

}  // namespace vaw::multiple_sines

#endif  // VAW_MULTIPLE_SINES_AUDIO_STREAM_H

