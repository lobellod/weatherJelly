#include "hlpFunc.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE;
#ifndef LEDCONTROL_INCLUDED
#define LEDCONTROL_INCLUDED

#define LED_PIN     2
#define NUM_LEDS    144
#define BRIGHTNESS  200
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND  100



class temperaturePalette{
    static CRGBPalette16 temperaturePalette_static;
    CRGBPalette16 basicPalette;
    uint8_t temp;
  public:
    CRGBPalette16 getPalette(uint8_t);
};

class ledEffects{
    CRGB* leds_p;
    CRGBPalette16 currentPalette;
    mappedDataS currentData;
    sunTimesS sunTime;
    int mins = 35;
    int max = 107;
    int chanceOfDrops = 0;
    CRGB dropColor = CRGB::White;
  public:
    void setupLedEffects(CRGB*, CRGBPalette16, mappedDataS);
    void windShiftLeds();
    void snowRainEffects();
};

#endif
