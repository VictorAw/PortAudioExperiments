cl.exe -EHsc -MT -std:c++17 -Fo.\obj\^
  main.cpp AudioStream.cpp^
  portaudio_x64.lib Advapi32.lib^
  -I . -I D:\Programming\C++\Libraries\portaudio\include^
  /link -LIBPATH:D:\Programming\C++\Libraries\portaudio\lib^
  -OUT:bin\AudioStream.exe

