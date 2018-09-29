#include "LEDcontrol.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE;

//================= Basic Palette heatmap define===============================
DEFINE_GRADIENT_PALETTE( temperature_heatmap ) {
0,     0, 0, 255,   //Blue
50,    25, 61, 255,   //Dark Slate Blue
90,    25, 110, 212,   //Midnight Blue
120,   0, 139, 180,   //Dark Cyan
180,   200, 128, 148, //light Coral
255,   255, 92, 92 }; //Indian Red

DEFINE_GRADIENT_PALETTE( sunColor_heatmap ) {
0,     220, 20, 60,   //Crimson
110,    255, 69, 0,   //Orange/red
160,    255, 140, 0,   //Dark Orange
190,   255, 165, 0,   //Orange
220,   255, 215, 0, //Gold
255,   255, 255, 0 }; //Yellow
//======================================================================

CRGBPalette16 paletteClass::temperaturePalette_static = temperature_heatmap;
CRGBPalette16 paletteClass::sunColorPalette_static = sunColor_heatmap;

CRGBPalette16 paletteClass::getTempPalette(uint8_t temp){
  uint8_t  colorIndexLow = temp/4;
  uint8_t  colorIndexHigh = temp;
  CRGB color1 = ColorFromPalette(temperaturePalette_static, colorIndexLow, 200, NOBLEND);
  CRGB color2  = ColorFromPalette(temperaturePalette_static, colorIndexHigh,200 , NOBLEND);
  CRGB blackblue  = CRGB(0,0,80);//CRGB::Black;
  tempPalette = CRGBPalette16(
                                color1,   color1,  color2, color2,
                                color1, color2, color1,  blackblue,
                                color2,  color2,  color1,  color1,
                                color2, color1, color2,  blackblue );
  return tempPalette;
}

CRGBPalette16 paletteClass::getSunPalette(int timeToRise, int timeToSet){
  uint8_t colorIndexLow;
  uint8_t colorIndexHigh;
  uint8_t sunBrightness;
  if(timeToRise<3600 && timeToRise>-3600){
    colorIndexLow = map(timeToRise, 3600, -3600, 0, 210);
    colorIndexHigh = colorIndexLow+40;
    sunBrightness = map(timeToRise, 3600, 0, 0, 200);
  }
  else if(timeToSet<3600 && timeToSet>-3600){
    colorIndexLow = map(timeToSet, 3600, -3600, 210, 0);
    colorIndexHigh = colorIndexLow+40;
    sunBrightness = map(timeToSet, 0, -3600, 200, 0);
  }
  else if(timeToRise<-3600 && timeToSet>3600){
    colorIndexHigh = 250;
    colorIndexLow = 230;
    sunBrightness = 200;
  }
  else{
    sunBrightness = 0;
  }
  CRGB color1 = ColorFromPalette(sunColorPalette_static, colorIndexLow, sunBrightness/2, NOBLEND);
  CRGB color2  = ColorFromPalette(sunColorPalette_static, colorIndexHigh, sunBrightness , NOBLEND);
  CRGB black  = CRGB::Black;
  sunPalette = CRGBPalette16(
                                black,   black,  black, black,
                                black, black, black,  color1,
                                color2,  color1,  black,  black,
                                black, black, black,  black );
  return sunPalette;
}

ledEffects::ledEffects(CRGB* ledArray){
    leds_p = ledArray;
}

void ledEffects::setData(CRGBPalette16 tempPalette, mappedDataS data){
  currentTempPalette = tempPalette;
  currentData = data;
}

void ledEffects::windShiftLeds(){
  uint8_t colorIndex = beatsin8((currentData.wind/14),0,120);
  uint8_t tempIndex = colorIndex;
  for( int i = (SKYLEDSTART-1); i >= 0; i--) {
    leds_p[i] = ColorFromPalette( currentTempPalette, tempIndex, 200, LINEARBLEND);
    tempIndex++;
  }
  tempIndex = colorIndex;
  for(int j = SKYLEDEND; j < NUM_LEDS; j++) {
    leds_p[j] = ColorFromPalette( currentTempPalette, tempIndex, 200, LINEARBLEND);
    tempIndex++;
  }
}

void ledEffects::snowRainEffects(){
  if(currentData.type==2){
    //Light Rain
    chanceOfDrops = 20;
    dropColor = CRGB::LightBlue;
  }
  else if(currentData.type==3){
    //Heavy Rain
    chanceOfDrops = 50;
    dropColor = CRGB::LightBlue;
  }
  else if(currentData.type==4){
    //Rain & Snow
    chanceOfDrops = 40;
    if(random8(0,2)==0){dropColor = CRGB::LightBlue;}
    else{dropColor = CRGB::White;}
  }
  else if(currentData.type==5){
    //light Snow
    chanceOfDrops = 20;
    dropColor = CRGB::White;
  }
  else if(currentData.type==6){
    //Heavy Snow
    chanceOfDrops = 50;
    dropColor = CRGB::White;
  }

  if( random8() < chanceOfDrops) {
    leds_p[ random16(SKYLEDSTART,SKYLEDEND) ] += dropColor;
  }
}

void ledEffects::sunEffects(CRGBPalette16 palette, mappedDataS updatedSunData){
  currentData = updatedSunData;
  currentSunPalette = palette;
  uint8_t index;
  int tempTime = updatedSunData.sunTime.timeNow;
  if(tempTime<updatedSunData.sunTime.timeToRise){
    tempTime = updatedSunData.sunTime.timeToRise;
  }
  if(tempTime>updatedSunData.sunTime.timeToSet){
    tempTime = updatedSunData.sunTime.timeToSet;
  }
  index = map(tempTime,updatedSunData.sunTime.timeToRise,updatedSunData.sunTime.timeToSet,80,120);
  for (int i=SKYLEDSTART; i<SKYLEDEND;i++){
    leds_p[i] = ColorFromPalette( currentSunPalette, index, 200, LINEARBLEND);
    index=index+2;
  }

}
