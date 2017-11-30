// for NT october 1992 beta version:
// sndPlaySound will not be found in winmm.lib if in a C++-function

#include <windows.h>
#include <mmsystem.h>

void DoSound(void)
{
   if (!sndPlaySound("TURTLE.WAV", SND_ASYNC | SND_NODEFAULT))
      MessageBeep(0);
}
