#include "AudioStream.h"

#include <portaudio.h>

#include <cmath>
#include <iostream>

namespace vaw::add_notes {

float AudioStream::a440_[AudioStream::TABLE_SIZE];
bool AudioStream::a440_generated_ = false; 

AudioStream::AudioStream() {
  if (!a440_generated_) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
      double angle = (static_cast<double>(i) / static_cast<double>(TABLE_SIZE)) * (M_PI * 2);
      a440_[i] = static_cast<float>(sin(angle));
    }
  }
  a440_generated_ = true;
}

bool AudioStream::Open(PaDeviceIndex device) {
  if (device == paNoDevice) {
    return false;
  }

  PaStreamParameters parameters;
  parameters.device = device;
  parameters.channelCount = 2;  // Stereo
  parameters.sampleFormat = paFloat32;
  parameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowOutputLatency;
  parameters.hostApiSpecificStreamInfo = NULL;

  PaError error = Pa_OpenStream(
    &stream_,
    NULL,  // No input
    &parameters,
    SAMPLE_RATE,
    256,  // Frames per buffer
    paNoFlag,  // Stream flags
    &AudioStream::AudioCallbackEntry,
    this  // Pass this in so that it can be sent into the member function call
  );
  if (error != paNoError) { return false; }

  error = Pa_SetStreamFinishedCallback(stream_, &AudioStream::FinishedCallbackEntry);
  if (error != paNoError) {
    Pa_CloseStream(stream_);
    stream_ = nullptr;
    return false;
  }

  return true;
}

bool AudioStream::Close() {
  if (!stream_) { return false; }
  PaError error = Pa_CloseStream(stream_);
  stream_ = nullptr;
  return error == paNoError;
}

bool AudioStream::Start() {
  if (!stream_) { return false; }
  PaError error = Pa_StartStream(stream_);
  return error == paNoError;
}

bool AudioStream::Stop() {
  if (!stream_) { return false; }
  PaError error = Pa_StopStream(stream_);
  return error == paNoError;
}

bool AudioStream::SetVolume(float percent) {
  if (percent > 100.0f || percent < 0.0f) { return false; }
  volume_ = percent / 100.0f;
  return true;
}

// TODO: Rework this into AddNotes
bool AudioStream::PlayNotes(std::vector<Note> && notes) {
  std::size_t note_count = notes.size();
  active_notes_.notes = std::move(notes);
  active_notes_.playback.resize(note_count);
  active_notes_.size = note_count;
  for (std::size_t i = 0; i < note_count; ++i) {
    Note & note = active_notes_.notes[i];
    NotePlaybackState & playback = active_notes_.playback[i];
    playback.phase = 0.0f;
    playback.phase_step = note.frequency / 440.0f;

    // Add a volume point matching the start and end of the note so that interpolation can be
    // performed
    
    // Beginning
    auto first_time_volume_pair = note.time_volume_map.begin();
    if (first_time_volume_pair->first != note.start_millis) {
      note.time_volume_map[note.start_millis] = first_time_volume_pair->second;
    } else if (first_time_volume_pair == note.time_volume_map.end()) {
      note.time_volume_map[note.start_millis] = 1.0f;
    }

    // End
    auto last_time_volume_pair = note.time_volume_map.rbegin();
    if (last_time_volume_pair->first != note.end_millis) {
      note.time_volume_map[note.end_millis] = last_time_volume_pair->second;
    } else if (last_time_volume_pair == note.time_volume_map.rend()) {
      note.time_volume_map[note.end_millis] = 1.0f;
    }
  }

  return Start();
}

int AudioStream::AudioCallback(void const * input_buffer, void * output_buffer,
                               unsigned long frames_per_buffer,
                               PaStreamCallbackTimeInfo  const * time_info,
                               PaStreamCallbackFlags status_flags,
                               void * user_data) {
  float * out = reinterpret_cast<float *>(output_buffer);

  // Prevent unused variable warnings
  (void) time_info;
  (void) status_flags;
  (void) input_buffer;
  (void) user_data;

  double current_time_milli = time_info->currentTime * 1e3;
  playback_start_ = playback_start_ == 0.0 ? current_time_milli : playback_start_;
  float clip_scalar = 1.0f / static_cast<float>(active_notes_.size);
  for (unsigned long _ = 0; _ < frames_per_buffer; ++_) {
    float amplitude = 0.0f;
    for (std::size_t i = 0; i < active_notes_.size; ++i) {
      Note const & note = active_notes_.notes[i];
      NotePlaybackState & playback = active_notes_.playback[i];
      std::int64_t elapsed_time_millis = static_cast<int64_t>(current_time_milli - playback_start_);

      if (elapsed_time_millis >= note.start_millis && elapsed_time_millis < note.end_millis) {
        float volume_scalar = volume_ * note.GetVolume(elapsed_time_millis);
        amplitude += volume_scalar * clip_scalar * a440_[static_cast<int>(playback.phase)];
        playback.phase += playback.phase_step;
        if (playback.phase >= TABLE_SIZE) { playback.phase -= TABLE_SIZE; }
      }
    }

    // Left channel
    *(out++) = amplitude;
    // Right channel
    *(out++) = amplitude;
  }

  return paContinue;
}

int AudioStream::AudioCallbackEntry(void const * input_buffer, void * output_buffer,
                                    unsigned long frames_per_buffer,
                                    PaStreamCallbackTimeInfo  const * time_info,
                                    PaStreamCallbackFlags status_flags,
                                    void * user_data) {
  return reinterpret_cast<AudioStream *>(user_data)->AudioCallback(
      input_buffer, output_buffer, frames_per_buffer, time_info, status_flags, user_data);
}

void AudioStream::FinishedCallback() {
  std::cout << "Stream completed\n";
}

void AudioStream::FinishedCallbackEntry(void * user_data) {
  return reinterpret_cast<AudioStream *>(user_data)->FinishedCallback();
}

}  // namespace vaw::add_notes

