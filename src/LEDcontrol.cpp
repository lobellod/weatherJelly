#include "LEDcontrol.h"
#include <FastLED.h>
#include "debug.h"
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
  CRGB blackblue  = CRGB(0,0,80);//CRGB::Black;
  tempPalette = CRGBPalette16(
                                color1,   color1,  color2, color2,
                                color1, color2, color1,  blackblue,
                                color2,  color2,  color1,  color1,
                                color2, color1, color2,  blackblue );
  return tempPalette;
}

CRGBPalette16 paletteClass::getSunAndSkyPalette(int timeToRise, int timeToSet, uint8_t cloudCoverage){
  uint8_t colorIndexLow = 0;
  uint8_t colorIndexHigh = 0;
  uint8_t sunBrightness = 0;
  uint8_t skyBrightness = 5;
  if(timeToRise<1800 && timeToRise>-1800){
    colorIndexLow = map(timeToRise, -1800, 1800, 180, 0);
    colorIndexHigh = colorIndexLow+40;
    sunBrightness = map(constrain(timeToRise,0,1800), 0, 1800, 200, 0);
    skyBrightness = map(constrain(timeToRise,-1800,0), -1800, 0, MAX_SKY_BRIGHTNESS, 5);
  }
  else if(timeToSet<1800 && timeToSet>-1800){
    colorIndexLow = map(timeToSet, -1800, 1800, 0, 180);
    colorIndexHigh = colorIndexLow+40;
    sunBrightness = map(constrain(timeToSet,-1800,0), -1800, 0, 0, 200);
    skyBrightness = map(constrain(timeToSet,0,1800), 0, 1800, 5, MAX_SKY_BRIGHTNESS);
  }
  else if(timeToRise<-1800 && timeToSet>1800){
    colorIndexHigh = 250;
    colorIndexLow = 230;
    sunBrightness = 200;
    skyBrightness = MAX_SKY_BRIGHTNESS;
  }
  setSkyBrightness(skyBrightness);
  setGlobalBrightness(sunBrightness);

  CRGB sunEdgeColor = ColorFromPalette(sunColorPalette_static, colorIndexLow, sunBrightness/4, NOBLEND);
  CRGB sunColor  = ColorFromPalette(sunColorPalette_static, colorIndexHigh, sunBrightness , NOBLEND);
  CRGB skyColor = ColorFromPalette(skyPalette.palette,cloudCoverage, skyPalette.brightness, NOBLEND);
  /*if(!sunEdgeColor){
    sunEdgeColor = skyColor;
    sunColor = skyColor;
  }*/
  sunAndSkyPalette = CRGBPalette16(
                                skyColor, skyColor, skyColor, skyColor,
                                skyColor, skyColor, skyColor, skyColor,
                                sunEdgeColor, sunColor, sunEdgeColor, skyColor,
                                skyColor, skyColor, skyColor, skyColor);

  return sunAndSkyPalette;
}

void paletteClass::setSkyBrightness(uint8_t brightness){
  skyPalette.palette = cloudAndSky_heatmap;
  skyPalette.brightness = brightness;
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
    leds_p[i] = ColorFromPalette( currentTempPalette, tempIndex, globalBrightness, LINEARBLEND);
    tempIndex++;
  }
  tempIndex = colorIndex;
  for(int j = SKYLEDEND; j < NUM_LEDS; j++) {
    leds_p[j] = ColorFromPalette( currentTempPalette, tempIndex, globalBrightness, LINEARBLEND);
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

  if( random8(200) < chanceOfDrops) {
    leds_p[ random16(SKYLEDSTART,SKYLEDEND) ] += dropColor;
  }
}

void ledEffects::SkyAndSunEffects(CRGBPalette16 palette, mappedDataS updatedSunData, skyPaletteS skyData){
  currentData = updatedSunData;
  skyColorsS = skyData;
  currentSunPalette = palette;
  uint8_t index;
  int tempTime = updatedSunData.sunTime.timeNow;
  int sunriseTime = updatedSunData.sunTime.timeToRise+tempTime;
  int sunsetTime = updatedSunData.sunTime.timeToSet+tempTime;
  tempTime = constrain(tempTime, sunriseTime, sunsetTime);
  index = map(tempTime,sunriseTime,sunsetTime,160,48);

  for (int i=SKYLEDSTART; i<SKYLEDEND; i++){
    leds_p[i] = ColorFromPalette( currentSunPalette, index, 255, LINEARBLEND);
    index=index+2;
  }

}
