#include "LEDcontrol.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE;

DEFINE_GRADIENT_PALETTE( temperature_heatmap ) {
0,     0,  0,  255,   //Blue
50,     25,  61,  255,   //Dark Slate Blue
90,   25,  110,  212,   //Midnight Blue
120,   0,139,  180,   //Dark Cyan
180,   200, 128,  148, //light Coral
255,   255,92,92 }; //Indian Red

CRGBPalette16 temperaturePalette::temperaturePalette_static = temperature_heatmap;

CRGBPalette16 temperaturePalette::getPalette(uint8_t temp){
  uint8_t  colorIndexLow = temp/4;
  uint8_t  colorIndexHigh = temp;
  CRGB color1 = ColorFromPalette(temperaturePalette_static, colorIndexLow, 200, NOBLEND);
  CRGB color2  = ColorFromPalette(temperaturePalette_static, colorIndexHigh,200 , NOBLEND);
  CRGB black  = CRGB(0,0,80);//CRGB::Black;
  basicPalette = CRGBPalette16(
                                color1,   color1,  color2, color2,
                                color1, color2, color1,  black,
                                color2,  color2,  color1,  color1,
                                color2, color1, color2,  black );
  return basicPalette;
  }

void ledEffects::setupLedEffects(CRGB* ledArray, CRGBPalette16 palette, mappedDataS data){
  leds_p = ledArray;
  currentPalette = palette;
  currentData = data;
}

void ledEffects::windShiftLeds(){
  int colorIndex = beatsin16((currentData.wind/14),0,120);
  int tempIndex = colorIndex;
  for( int i = (NUM_LEDS/2-1); i >= 0; i--) {
    leds_p[i] = ColorFromPalette( currentPalette, tempIndex, 200, LINEARBLEND);
    tempIndex++;
  }
  tempIndex = colorIndex;
  for( int i = NUM_LEDS/2; i < NUM_LEDS; i++) {
    leds_p[i] = ColorFromPalette( currentPalette, tempIndex, 200, LINEARBLEND);
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
    if(random16(0,2)==0){dropColor = CRGB::LightBlue;}
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
    leds_p[ random16(mins,max) ] += dropColor;
  }
}
