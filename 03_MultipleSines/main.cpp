#include "AudioStream.h"
#include "Note.h"

#include <portaudio.h>

#include <chrono>
#include <iostream>
#include <thread>

using vaw::multiple_sines::AudioStream;
using vaw::multiple_sines::Note;

void print_error(PaError error) {
  std::cout << "----------------------------\n";
  std::cout << "PortAudio error " << error << ": " << Pa_GetErrorText(error) << '\n';
  std::cout << "----------------------------";
  std::cout << std::endl;
}

int main(int argc, char ** argv) {
  AudioStream stream;

  std::vector<Note> notes {
    Note(0, 0.5, "A4"),
    Note(0, 1.0, "C#5"),
    Note(0, 0.75, "G5"),
  };

  PaError error;
  error = Pa_Initialize();
  if (error != paNoError) {
    print_error(error);
    return 1;
  }

  if (stream.Open(Pa_GetDefaultOutputDevice())) {
    if (stream.PlayNotes(std::move(notes))) {
      std::cout << "----------------------------\n";
      std::cout << "Playing for 2 seconds\n";
      std::cout << "----------------------------";
      std::cout << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(2));

      stream.Stop();
    }
    stream.Close();
  }

  Pa_Terminate();
  std::cout << "----------------------------\n";
  std::cout << "Program finished\n";
  std::cout << "----------------------------";
  std::cout << std::endl;

  return 0;
}
