
// Instead of creating a static wave table as suggested in the
// lecture notes, you might just compute it at startup time.
// To do so, call init_wavetable() from main() before you
// initialize timer 6.  In order for init_wavetable() to
// modify the wavetable array, the array must not be static.

// You can also disregard this template, and build a static
// wavetable array.  Your choice.

#include <math.h>

#define N 1000
short int wavetable[N];

void init_wavetable(void)
{
  int x;
  for(x=0; x<N; x++)
    wavetable[x] = 32767 * sin(2 * M_PI * x / N);
}

