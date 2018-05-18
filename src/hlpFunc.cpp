#include <Particle.h>
#include "hlpFunc.h"

weatherDataParser::weatherDataParser(const char *data){
  receivedData=String(data);
  receivedData.toCharArray(strBuffer,50);
  weatherData.weatherID = atoi(strtok(strBuffer,"\"~"));
  weatherData.temp = atoi(strtok(NULL, "~"));
  weatherData.wind = atoi(strtok(NULL, "~"));
}
