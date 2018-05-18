#include <Particle.h>

#ifndef HLPFUNC_INCLUDED
#define HLPFUNC_INCLUDED

struct weatherDataS{
  int weatherID;
  int temp;
  int wind;
};

class weatherDataParser{
    String receivedData;
    weatherDataS weatherData;
    char strBuffer[50] = "";
  public:
    weatherDataParser(const char*);
    weatherDataS Data(){return weatherData;}
};

#endif
