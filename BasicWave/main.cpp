#include <chrono>
#define _USE_MATH_DEFINES  // Enable M_PI
#include <cmath>
#include <iostream>
#include <string>
#include <thread>

#include <portaudio.h>

constexpr int SAMPLE_RATE = 44000;

struct TestData {
  float left_phase = 0.0f;
  float right_phase = 0.0f;
  std::string waveform;

  // Each index of the buffer in a frame is one sample. The table size is how many frames
  // are needed to complete one period of the sine wave. Therefore, if the sample rate is
  // 44000 and the table size is 100, the note frequency is 440 Hz. If the table size is
  // 200, the note frequency is 220 Hz.
  constexpr static int TABLE_SIZE = 100;
  float sine[TABLE_SIZE];

  TestData(std::string const & waveform_setting) {
    waveform = waveform_setting;

    for (int i = 0; i < TABLE_SIZE; ++i) {
      double angle = static_cast<double>(i) / static_cast<double>(TABLE_SIZE);
      sine[i] = static_cast<float>(sin(angle * M_PI * 2));
    }
  }
};

int AudioStreamCallback(void const * input_buffer,
                        void * output_buffer,
                        unsigned long frames_per_buffer,
                        PaStreamCallbackTimeInfo const * time_info,
                        PaStreamCallbackFlags status_flags,
                        void * user_data) {
  TestData * data = reinterpret_cast<TestData *>(user_data);
  float * out = reinterpret_cast<float *>(output_buffer);

  for (unsigned int i = 0; i < frames_per_buffer; ++i) {
    if (data->waveform == "sawtooth") {
      *(out++) = data->left_phase;
      *(out++) = data->right_phase;

      // Generate simple sawtooth phaser that ranges between -1.0 and 1.0
      data->left_phase += 0.01f;
      if (data->left_phase >= 1.0f) { data->left_phase -= 2.0f; }
      // Right will have a higher pitch so we can distinguish left and right
      data->right_phase += 0.03;
      if (data->right_phase >= 1.0f) { data->right_phase -= 2.0f; }
    } else if (data->waveform == "sine") {
      // For now, we'll just cast float to int here, but realistically we would have some sort
      // of dynamically typed phase members, either through templating or through inheritance
      *(out++) = data->sine[static_cast<int>(data->left_phase)];
      *(out++) = data->sine[static_cast<int>(data->right_phase)];
      data->left_phase += 1;
      if (data->left_phase >= data->TABLE_SIZE) { data->left_phase -= data->TABLE_SIZE; }
      data->right_phase += 1.18;
      if (data->right_phase >= data->TABLE_SIZE) { data->right_phase -= data->TABLE_SIZE; }
    }
  }

  return paContinue;
}

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cout << "Usage: BasicPlayback waveform" << std::endl;
    return 1;
  }

  std::string waveform = argv[1];

  TestData data(waveform);

  PaError init_error = Pa_Initialize();
  if (init_error != paNoError) { goto handle_error; }

  PaStream * stream;
  PaError error;

  error = Pa_OpenDefaultStream(
    &stream,
    0, // No input channels
    2, // Stero output
    paFloat32, // 32-bit floating point output
    SAMPLE_RATE,
    256, // Frames per buffer
    AudioStreamCallback,
    &data
  );
  if (error != paNoError) { goto handle_error; }
  
  error = Pa_StartStream(stream);
  if (error != paNoError) { goto handle_error; }

  std::this_thread::sleep_for(std::chrono::seconds(2));

  error = Pa_StopStream(stream);
  if (error != paNoError) { goto handle_error; }

  error = Pa_CloseStream(stream);
  if (error != paNoError) { goto handle_error; }

  error = Pa_Terminate();
  if (error != paNoError) { goto handle_error; }

  return 0;

  // Convenience location for handling C errors since this is only an example tutorial with just a main function
  handle_error:
    std::cout << "PortAudio error " << error << ": " << Pa_GetErrorText(error) << std::endl;
}

