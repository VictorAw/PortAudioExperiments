cl.exe -EHsc -MT -std:c++17 -Fo.\obj\^
  main.cpp^
  portaudio_x64.lib Advapi32.lib^
  -I D:\Programming\C++\Libraries\portaudio\include^
  /link -LIBPATH:D:\Programming\C++\Libraries\portaudio\lib^
  -OUT:bin\BasicWave.exe

