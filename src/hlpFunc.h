#include <Particle.h>

#ifndef HLPFUNC_INCLUDED
#define HLPFUNC_INCLUDED

struct weatherDataS{
  int weatherID;
  int temp;
  int wind;
  int clouds;
  int currentTime;
  int sunrise;
  int sunset;
};

struct sunTimesS{
  int timeToRise;
  int timeToSet;
  int timeNow;
};

struct mappedDataS{
  uint8_t type;
  uint8_t temp;
  uint8_t wind;
  uint8_t clouds;
  struct sunTimesS sunTime;
};

class weatherDataParser{
    String receivedData;
    weatherDataS weatherData;
    char strBuffer[100] = "";
  public:
    weatherDataS parseData(const char *data);
};

class dataToLedConverter{
    weatherDataS unMappedData;
    mappedDataS mappedData;
    int thunderStormIDs[10]={200,201,202,210,211,212,221,230,231,232};
    int lightRainIDs[9]={300,301,302,310,311,500,501,511,520};
    int heavyRainIDs[10]={312,323,314,321,502,503,504,521,522,531};
    int rainSnowIDs[4]={611,612,615,616};
    int lightSnowIDs[2]={600,620};
    int heavySnowIDs[4]={601,602,621,622};
    int otherIDs[10]={701,711,721,731,741,751,761,762,771,781};
    int clearishIDs[5]={800,801,802,803,804};
  public:
    void timeForSunUpdate(int);
    void setLedConverterData(weatherDataS);
    mappedDataS getData() {return mappedData;}
  private:
    int weatherIDconverter(int);
    uint8_t cloudConverter(int);
    uint8_t tempConverter(int);
    uint8_t windConverter(int);
};

#endif
