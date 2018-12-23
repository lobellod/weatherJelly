#include "LEDcontrol.h"
#include <FastLED.h>
#include "debug.h"
FASTLED_USING_NAMESPACE;

//================= Basic Palette heatmap define===============================
DEFINE_GRADIENT_PALETTE( temperature_heatmap ) {
0,     0, 0, 255,   //Blue
30,    25, 61, 255,   //Dark Slate Blue
50,    25, 110, 212,   //Midnight Blue
80,   0, 139, 180,   //Dark Cyan
150,   200, 128, 148, //light Coral
255,   255, 92, 92 }; //Indian Red

DEFINE_GRADIENT_PALETTE( sunColor_heatmap ) {
0,     220, 20, 60,   //Crimson
110,    255, 69, 0,   //Orange/red
160,    255, 140, 0,   //Dark Orange
190,   255, 165, 0,   //Orange
220,   255, 215, 0, //Gold
255,   255, 255, 0 }; //Yellow

DEFINE_GRADIENT_PALETTE( cloudAndSky_heatmap) {
  0,   0, 0, 237,
  150, 173, 216, 230,
  255,  200, 200, 200};

//======================================================================
uint8_t globalBrightness = 0;
void setGlobalBrightness(uint8_t brightness){
  globalBrightness = constrain(brightness,100,250);
}

CRGBPalette16 paletteClass::temperaturePalette_static = temperature_heatmap;
CRGBPalette16 paletteClass::sunColorPalette_static = sunColor_heatmap;

CRGBPalette16 paletteClass::getTempPalette(uint8_t temp){
  uint8_t  colorIndexLow = temp/4;
  uint8_t  colorIndexHigh = temp;
  CRGB color1 = ColorFromPalette(temperaturePalette_static, colorIndexLow, 200, NOBLEND);
  CRGB color2  = ColorFromPalette(temperaturePalette_static, colorIndexHigh,200 , NOBLEND);
  CRGB blackblue  = CRGB(0,0,50);//CRGB::Black;
  CRGB black  = CRGB::Black;
  tempPalette = CRGBPalette16(
    black, color2, color1,  blackblue,
    color2,  color2,  blackblue,  blackblue,
    color1, color1, blackblue,  blackblue,
    color1,   color1,  color2, color2);
  return tempPalette;
}

CRGBPalette16 paletteClass::getSunAndSkyPalette(int timeToRise, int timeToSet, uint8_t cloudCoverage){
  uint8_t colorIndexLow = 0;
  uint8_t colorIndexHigh = 0;
  uint8_t sunBrightness = 0;
  uint8_t skyBrightness = 5;
  if(timeToRise<1800 && timeToRise>-1800){
    colorIndexLow = map(constrain(timeToRise,-1800,600), -1800, 600, 210, 0);
    colorIndexHigh = colorIndexLow+40;
    sunBrightness = map(constrain(timeToRise,-300,600), -300, 600, SUNBRIGHTNESS, 0);
    skyBrightness = map(constrain(timeToRise,-900,0), -900, 0, MAX_SKY_BRIGHTNESS, 5);
  }
  else if(timeToSet<1800 && timeToSet>-1800){
    colorIndexLow = map(constrain(timeToSet,-600,1800), -600, 1800, 0, 210);
    colorIndexHigh = colorIndexLow+40;
    sunBrightness = map(constrain(timeToSet,-600,300), -600, 300, 0, SUNBRIGHTNESS);
    skyBrightness = map(constrain(timeToSet,0,900), 0, 900, 5, MAX_SKY_BRIGHTNESS);
  }
  else if(timeToRise<=-1800 && timeToSet>=1800){
    colorIndexHigh = 250;
    colorIndexLow = 230;
    sunBrightness = SUNBRIGHTNESS;
    skyBrightness = MAX_SKY_BRIGHTNESS;
  }
  else{
    sunBrightness = 0;
    skyBrightness = 0;
  }
  setSkyBrightness(skyBrightness);
  setGlobalBrightness(sunBrightness);

  CRGB sunEdgeColor = ColorFromPalette(sunColorPalette_static, colorIndexLow, sunBrightness/3, NOBLEND);
  CRGB sunColor  = ColorFromPalette(sunColorPalette_static, colorIndexHigh, sunBrightness , NOBLEND);
  CRGB skyColor = ColorFromPalette(skyPalette.palette,cloudCoverage, skyPalette.brightness, NOBLEND);
  sunAndSkyPalette = CRGBPalette16(
                                skyColor, skyColor, skyColor, skyColor,
                                skyColor, skyColor, skyColor, sunEdgeColor,
                                 sunColor, sunEdgeColor, skyColor, skyColor,
                                skyColor, skyColor, skyColor, skyColor);
  return sunAndSkyPalette;
}

void paletteClass::setSkyBrightness(uint8_t brightness){
  skyPalette.palette = cloudAndSky_heatmap;
  skyPalette.brightness = brightness;
}

ledEffects::ledEffects(CRGB* ledArray){
    leds_p = ledArray;
    for(int i=0;i<SKYWIDTH;i++){
      dropArray[i]=CRGB::Black;
    }
    for(int i=0; i<NUM_LEDS; i++){
      thunderArray[i]=CRGB::Black;
    }
    errorPalette = CRGBPalette16(
                                  CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Red,
                                  CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Red,
                                  CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Red,
                                  CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Red);
}

void ledEffects::setData(CRGBPalette16 tempPalette, mappedDataS data){
  currentTempPalette = tempPalette;
  currentData = data;
}

void ledEffects::windShiftLeds(){
  uint8_t colorIndex = beatsin16((currentData.wind/11),0,255);
  uint8_t colorIndex2 = beatsin16((currentData.wind/15),0, 412);
  for( int i = (SKYLEDSTART-1); i >= 0; i--) {
    leds_p[i] = ColorFromPalette( currentTempPalette, colorIndex, globalBrightness, LINEARBLEND);
    colorIndex++;
  }
  for(int j = SKYLEDEND; j < NUM_LEDS; j++) {
    leds_p[j] = ColorFromPalette( currentTempPalette, colorIndex2, globalBrightness, LINEARBLEND);
    colorIndex2--;
  }
}


void ledEffects::snowRainEffects(){
  if(currentData.type==2){
    //Light Rain
    chanceOfDrops = 10;
    dropColor = CRGB::Blue;
  }
  else if(currentData.type==3){
    //Heavy Rain
    chanceOfDrops = 30;
    dropColor = CRGB::Blue;
  }
  else if(currentData.type==4){
    //Rain & Snow
    chanceOfDrops = 20;
    if(random8(0,2)==0){dropColor = CRGB::Blue;}
    else{dropColor = CRGB::White;}
  }
  else if(currentData.type==5){
    //light Snow
    chanceOfDrops = 10;
    dropColor = CRGB::White;
  }
  else if(currentData.type==6){
    //Heavy Snow
    chanceOfDrops = 30;
    dropColor = CRGB::White;
  }
  else if(currentData.type==1){
    //Thunder & rain
    chanceOfDrops = 30;
    dropColor = CRGB::Blue;
    thunderEffect();
  }
  else{
    chanceOfDrops = 0;
  }

  if( random8(1000) < chanceOfDrops) {
    dropArray[random16(0,SKYWIDTH) ] += dropColor;
  }
  snowRainFade();
}

void ledEffects::snowRainFade(){
  EVERY_N_MILLISECONDS(10){
    for(int i=0; i<SKYWIDTH;i++){
      dropArray[i].fadeToBlackBy(15);
    }
  }
  for (int i=SKYLEDSTART; i<SKYLEDEND; i++){
    leds_p[i] += dropArray[i-SKYLEDSTART];
  }
}

void ledEffects::thunderEffect(){
  EVERY_N_SECONDS(2){
    if(random8(200) < 10){
      for (int i=0; i<NUM_LEDS; i++){
        thunderArray[i] += CRGB::White;
      }
      if(random8(50) < 35){
        thunderCrack = random8(100);
      }
    }
  }
  EVERY_N_MILLISECONDS(30){
    if(thunderCrack>10){
      thunderCrack = thunderCrack - 1;
    }
    else if(thunderCrack==10){
      for (int i=SKYLEDSTART; i<SKYLEDEND; i++){
        thunderArray[i] += CRGB::White;
        thunderCrack = 0;
      }
    }
    else{
      thunderCrack = 0;
    }
    for(int i=0; i<NUM_LEDS;i++){
      thunderArray[i].fadeToBlackBy(15);
    }
  }
  for(int i=0; i<NUM_LEDS;i++){
    leds_p[i] += thunderArray[i];
  }
}
void ledEffects::errorMode(){
  uint8_t colorIndex = beatsin8(2,0,64);
  uint8_t localBrightness = beatsin8(6,0,255);
  for(int i=0;i<NUM_LEDS;i++){
    leds_p[i]=ColorFromPalette(errorPalette,colorIndex,localBrightness,LINEARBLEND);
    colorIndex++;
  }
}

void ledEffects::SkyAndSunEffects(CRGBPalette16 palette, mappedDataS updatedSunData, skyPaletteS skyData){
  currentData = updatedSunData;
  skyColorsS = skyData;
  currentSunPalette = palette;
  uint8_t index;
  unsigned long tempTime = updatedSunData.sunTime.timeNow;
  unsigned long sunriseTime = updatedSunData.sunTime.timeToRise+tempTime;
  unsigned long sunsetTime = updatedSunData.sunTime.timeToSet+tempTime;
  tempTime = constrain(tempTime, sunriseTime, sunsetTime);
  index = map(tempTime,sunriseTime,sunsetTime,138,24);
  //  index = map(tempTime,sunriseTime,sunsetTime,144,16);
  for (int i=SKYLEDSTART; i<SKYLEDEND; i++){
    leds_p[i] = ColorFromPalette( currentSunPalette, index, 255, LINEARBLEND);
    index=index+2;
  }
}
/*
CRGBPalette16 testPalette = CRGBPalette16(
                              CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black,
                              CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Red,
                              CRGB::White, CRGB::Red, CRGB::Black, CRGB::Black,
                              CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black);

void ledEffects::SkyAndSunEffects(CRGBPalette16 palette, mappedDataS updatedSunData, skyPaletteS skyData){
  currentData = updatedSunData;
  skyColorsS = skyData;
  currentSunPalette = palette;
  int timeTo;
  uint8_t index;
  int tempTime = updatedSunData.sunTime.timeNow;
  int sunriseTime = updatedSunData.sunTime.timeToRise+tempTime;
  int sunsetTime = updatedSunData.sunTime.timeToSet+tempTime;
  if(updatedSunData.sunTime.timeToRise>0){
    timeTo = constrain(updatedSunData.sunTime.timeToRise, 0, 600);
    index = map(timeTo,0,600,138,152);
//    index = 144;
  }
  else if(updatedSunData.sunTime.timeToSet<0){
    timeTo = constrain(updatedSunData.sunTime.timeToSet, -600, 0);
    index = map(timeTo,-600,0,24,0);
    //index = 16;
  }
  else{
      tempTime = constrain(tempTime, sunriseTime, sunsetTime);
      //index = map(tempTime,sunriseTime,sunsetTime,142,18);
      index = map(tempTime,sunriseTime,sunsetTime,138,24);
  }
  for (int i=SKYLEDSTART; i<SKYLEDEND; i++){
    leds_p[i] = ColorFromPalette( currentSunPalette, index, 255, LINEARBLEND);
    //leds_p[i] = ColorFromPalette( testPalette, index, 255, LINEARBLEND);
    index=index+2;
  }
}*/
