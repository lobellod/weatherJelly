#include "hlpFunc.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE;
#ifndef LEDCONTROL_INCLUDED
#define LEDCONTROL_INCLUDED

#define LED_PIN     2
#define NUM_LEDS    144
#define BRIGHTNESS  200
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define UPDATES_PER_SECOND  100



class temperaturePalette{
    static CRGBPalette16 temperaturePalette_static;
    CRGBPalette16 basicPalette;
    uint8_t temp;
  public:
    temperaturePalette(uint8_t);
    CRGBPalette16 getPalette(){return basicPalette;}
};

class ledEffects{
    CRGB* leds_p;
    CRGBPalette16 currentPalette;
    mappedDataS currentData;
  public:
    ledEffects(CRGB*, CRGBPalette16, mappedDataS);
    void windShiftLeds();
};

#endif
