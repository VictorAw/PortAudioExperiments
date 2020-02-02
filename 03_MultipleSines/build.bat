cl.exe -EHsc -MT -std:c++17 -Fo.\obj\^
  main.cpp AudioStream.cpp Note.cpp^
  portaudio_x64.lib Advapi32.lib^
  -D_USE_MATH_DEFINES^
  -I . -I D:\Programming\C++\Libraries\portaudio\include^
  /link -LIBPATH:D:\Programming\C++\Libraries\portaudio\lib^
  -OUT:bin\MultipleSines.exe

