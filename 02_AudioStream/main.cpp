#include "AudioStream.h"

#include <portaudio.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using vaw::sine_notes::AudioStream;

void print_error(PaError error) {
  std::cout << "----------------------------\n";
  std::cout << "PortAudio error " << error << ": " << Pa_GetErrorText(error) << '\n';
  std::cout << "----------------------------";
  std::cout << std::endl;
}

bool OpenStreams(std::vector<AudioStream> & streams, PaDeviceIndex device) {
  bool success = true;
  for (AudioStream & stream : streams) {
    success = stream.Open(device) && success;
  }
  return success;
}

bool StartStreams(std::vector<AudioStream> & streams) {
  bool success = true;
  for (AudioStream & stream : streams) {
    success = stream.Start() && success;
  }
  return success;
}

bool StopStreams(std::vector<AudioStream> & streams) {
  bool success = true;
  for (AudioStream & stream : streams) {
    success = stream.Stop() && success;
  }
  return success;
}

bool CloseStreams(std::vector<AudioStream> & streams) {
  bool success = true;
  for (AudioStream & stream : streams) {
    success = stream.Close() && success;
  }
  return success;
}

int main(int argc, char ** argv) {
  std::vector<AudioStream> streams(2);
  streams[1].SetFrequency(554.37);
  streams[0].SetVolume(5);
  streams[1].SetVolume(5);

  PaError error;
  error = Pa_Initialize();
  if (error != paNoError) {
    print_error(error);
    return 1;
  }

  if (OpenStreams(streams, Pa_GetDefaultOutputDevice())) {
    if (StartStreams(streams)) {
      std::cout << "----------------------------\n";
      std::cout << "Playing for 2 seconds\n";
      std::cout << "----------------------------";
      std::cout << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(2));

      StopStreams(streams);
    }
    CloseStreams(streams);
  }

  Pa_Terminate();
  std::cout << "----------------------------\n";
  std::cout << "Program finished\n";
  std::cout << "----------------------------";
  std::cout << std::endl;

  return 0;
}
