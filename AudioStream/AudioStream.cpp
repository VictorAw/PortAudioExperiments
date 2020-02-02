#include "AudioStream.h"

#include <portaudio.h>

#define _USE_MATH_DEFINES  // Enable M_PI
#include <cmath>
#include <iostream>

namespace vaw::sine_notes {

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

bool AudioStream::SetFrequency(float frequency) {
  if (frequency == 0.0f) { return false; }
  left_scalar_ = frequency / 440.0f;
  right_scalar_ = frequency / 440.0f;
  return true;
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

  for (unsigned long i = 0; i < frames_per_buffer; ++i) {
    *(out++) = a440_[static_cast<int>(left_phase_)];
    *(out++) = a440_[static_cast<int>(right_phase_)];
    left_phase_ += left_scalar_;
    if (left_phase_ >= TABLE_SIZE) { left_phase_ -= TABLE_SIZE; }
    right_phase_ += right_scalar_;
    if (right_phase_ >= TABLE_SIZE) { right_phase_ -= TABLE_SIZE; }
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

}  // namespace vaw::sine_notes

