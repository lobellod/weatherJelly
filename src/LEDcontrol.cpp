#include "LEDcontrol.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE;

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     25,  25,  112,   //Midnight blue
120,   0,  0,  255,   //Blue
190,   0,139,  139,   //Dark Cyan
255,   205,92,92 }; //Indian Red

CRGBPalette16 temperaturePalette::temperaturePalette_static = heatmap_gp;

temperaturePalette::temperaturePalette(uint8_t temp){
  uint8_t  colorIndexLow = temp/4;
  uint8_t  colorIndexHigh = colorIndexLow + temp/2;
  CRGB color1 = ColorFromPalette(temperaturePalette_static, colorIndexLow, 200, LINEARBLEND);
  CRGB color2  = ColorFromPalette(temperaturePalette_static, colorIndexHigh,200 , LINEARBLEND);
  CRGB black  = CRGB::Black;
  basicPalette = CRGBPalette16(
                                color1,   color1,  color2, color2,
                                color1, color2, color1,  black,
                                color2,  color2,  color1,  color1,
                                color2, color1, color2,  black );
  }

ledEffects::ledEffects(CRGB* leds_p, CRGBPalette16 currentPalette, mappedDataS currentData){
}

void ledEffects::windShiftLeds(){
  int colorIndex = beatsin16((currentData.wind/4),0,100);
  for( int i = (NUM_LEDS/2-1); i >= 0; i--) {
    leds_p[i] = ColorFromPalette( currentPalette, colorIndex, 200, LINEARBLEND);
  }
  for( int i = NUM_LEDS/2; i < NUM_LEDS; i++) {
    leds_p[i] = ColorFromPalette( currentPalette, colorIndex, 200, LINEARBLEND);
  }
}
