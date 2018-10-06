#include <Particle.h>
#include "hlpFunc.h"

weatherDataS weatherDataParser::parseData(const char *data){
  receivedData=String(data);
  receivedData.toCharArray(strBuffer,100);
  weatherData.weatherID = atoi(strtok(strBuffer,"\"~"));
  weatherData.temp = atoi(strtok(NULL, "~"));
  weatherData.wind = atoi(strtok(NULL, "~"));
  weatherData.clouds = atoi(strtok(NULL, "~"));
  weatherData.currentTime = atoi(strtok(NULL, "~"));
  weatherData.sunrise = atoi(strtok(NULL, "~"));
  weatherData.sunset = atoi(strtok(NULL, "~"));
  return weatherData;
}

void dataToLedConverter::setLedConverterData(weatherDataS dataStruct){
  unMappedData=dataStruct;
  mappedData.type = weatherIDconverter(dataStruct.weatherID);
  mappedData.clouds = cloudConverter(dataStruct.clouds);
  mappedData.temp = tempConverter(dataStruct.temp);
  mappedData.wind = windConverter(dataStruct.wind);
  timeForSunUpdate(dataStruct.currentTime);
}

uint8_t dataToLedConverter::windConverter(int windspeed){
  uint8_t mappedWind;
  windspeed = constrain(windspeed, 0, 15);
  mappedWind=map(windspeed, 0, 15, 0, 255);
  return mappedWind;
}
uint8_t dataToLedConverter::tempConverter(int temperature){
  uint8_t mappedTemperature;
  temperature = constrain(temperature, -10, 30);
  mappedTemperature=map(temperature, -10, 30, 0, 255);
  return mappedTemperature;
}

int dataToLedConverter::weatherIDconverter(int weatherID){
  int type;
  int i;
  for(i = 0; i<(sizeof(thunderStormIDs)/sizeof(thunderStormIDs[0]));i++){
    if(weatherID == thunderStormIDs[i]){
      type = 1; //Thunderstorm
      return type;
    }
  }
  for(i = 0; i<(sizeof(lightRainIDs)/sizeof(lightRainIDs[0]));i++){
    if(weatherID ==lightRainIDs[i]){
      type = 2; //Light rain
      return type;
    }
  }
  for(i = 0; i<(sizeof(heavyRainIDs)/sizeof(heavyRainIDs[0]));i++){
    if(weatherID ==heavyRainIDs[i]){
      type = 3; //Heavy rain
      return type;
    }
  }
  for(i = 0; i<(sizeof(rainSnowIDs)/sizeof(rainSnowIDs[0]));i++){
    if(weatherID ==rainSnowIDs[i]){
      type = 4; //Rain & Snow
      return type;
    }
  }
  for(i = 0; i<(sizeof(lightSnowIDs)/sizeof(lightSnowIDs[0]));i++){
    if(weatherID ==lightSnowIDs[i]){
      type = 5; //Light Snow
      return type;
    }
  }
  for(i = 0; i<(sizeof(heavySnowIDs)/sizeof(heavySnowIDs[0]));i++){
    if(weatherID ==heavySnowIDs[i]){
      type = 6; //Heavy Snow
      return type;
    }
  }
  for(i = 0; i<(sizeof(otherIDs)/sizeof(otherIDs[0]));i++){
    if(weatherID ==otherIDs[i]){
      type = 7; //Other weather
      return type;
    }
  }
  for(i = 0; i<(sizeof(clearishIDs)/sizeof(clearishIDs[0]));i++){
    if(weatherID ==clearishIDs[i]){
      type = 8; //Clear/cloudy weather
      return type;
    }
  }
  type = 9; //Unknown weatherID
  return type;
}

void dataToLedConverter::timeForSunUpdate(int timeAge){
  mappedData.sunTime.timeNow = (unMappedData.currentTime + timeAge);
  mappedData.sunTime.timeToRise = unMappedData.sunrise - mappedData.sunTime.timeNow;
  mappedData.sunTime.timeToSet = unMappedData.sunset - mappedData.sunTime.timeNow;
}

uint8_t dataToLedConverter::cloudConverter(int cloudpercentage){
  cloudpercentage = constrain(cloudpercentage, 0, 100);
  uint8_t cloudConverted = map(cloudpercentage, 0, 100, 0 ,255);
  return cloudConverted;
}
