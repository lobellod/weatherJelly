#include <Particle.h>
#include "hlpFunc.h"

weatherDataParser::weatherDataParser(const char *data){
  receivedData=String(data);
  receivedData.toCharArray(strBuffer,50);
  weatherData.weatherID = atoi(strtok(strBuffer,"\"~"));
  weatherData.temp = atoi(strtok(NULL, "~"));
  weatherData.wind = atoi(strtok(NULL, "~"));
}

dataToLedConverter::dataToLedConverter(weatherDataS dataStruct){
  mappedData.type = weatherIDconverter(dataStruct.weatherID);
  int loc_temp = dataStruct.temp;
  int loc_wind = dataStruct.wind;
  if(dataStruct.temp<-5){
    loc_temp=-5;
  }
  else if(dataStruct.temp>30){
    loc_temp = 30;
  }
  if(dataStruct.wind>15){
    loc_wind = 15;
  }
  mappedData.temp=map(loc_temp, -5, 30, 0, 255);
  mappedData.wind=map(loc_wind, 0, 15, 0, 255);
}

int dataToLedConverter::weatherIDconverter(int weatherID){
  int type;
  for(int i = 0; i<(sizeof(thunderStormIDs)/sizeof(thunderStormIDs[0]));i++){
    if(weatherID == thunderStormIDs[i]){
      type = 1; //Thunderstorm
      return type;
    }
  }
  for(int i = 0; i<(sizeof(lightRainIDs)/sizeof(lightRainIDs[0]));i++){
    if(weatherID ==lightRainIDs[i]){
      type = 2; //Light rain
      return type;
    }
  }
  for(int i = 0; i<(sizeof(heavyRainIDs)/sizeof(heavyRainIDs[0]));i++){
    if(weatherID ==heavyRainIDs[i]){
      type = 3; //Heavy rain
      return type;
    }
  }
  for(int i = 0; i<(sizeof(rainSnowIDs)/sizeof(rainSnowIDs[0]));i++){
    if(weatherID ==rainSnowIDs[i]){
      type = 4; //Rain & Snow
      return type;
    }
  }
  for(int i = 0; i<(sizeof(lightSnowIDs)/sizeof(lightSnowIDs[0]));i++){
    if(weatherID ==lightSnowIDs[i]){
      type = 5; //Light Snow
      return type;
    }
  }
  for(int i = 0; i<(sizeof(heavySnowIDs)/sizeof(heavySnowIDs[0]));i++){
    if(weatherID ==heavySnowIDs[i]){
      type = 6; //Heavy Snow
      return type;
    }
  }
  for(int i = 0; i<(sizeof(otherIDs)/sizeof(otherIDs[0]));i++){
    if(weatherID ==otherIDs[i]){
      type = 7; //Other weather
      return type;
    }
  }
  for(int i = 0; i<(sizeof(clearishIDs)/sizeof(clearishIDs[0]));i++){
    if(weatherID ==clearishIDs[i]){
      type = 8; //Clear/cloudy weather
      return type;
    }
  }
  type = 9; //Unknown weatherID
  return type;
}
