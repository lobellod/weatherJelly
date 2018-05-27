#include "hlpFunc.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE;
#ifndef LEDCONTROL_INCLUDED
#define LEDCONTROL_INCLUDED

#define LED_PIN     1
#define NUM_LEDS    144
#define BRIGHTNESS  200
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND  100
#define SKYWIDTH    48
#define SKYLEDSTART (NUM_LEDS/2 - SKYWIDTH/2)
#define SKYLEDEND   (NUM_LEDS/2 + SKYWIDTH/2)


class paletteClass{
    static CRGBPalette16 temperaturePalette_static;
    static CRGBPalette16 sunColorPalette_static;
    CRGBPalette16 tempPalette;
    CRGBPalette16 sunPalette;
    uint8_t temp;
  public:
    CRGBPalette16 getTempPalette(uint8_t);
    CRGBPalette16 getSunPalette(int, int);
};

class ledEffects{
    CRGB *leds_p;
    CRGBPalette16 currentTempPalette;
    CRGBPalette16 currentSunPalette;
    mappedDataS currentData;
    int chanceOfDrops = 0;
    CRGB dropColor = CRGB::White;
  public:
    ledEffects(CRGB*);
    void setData(CRGBPalette16, mappedDataS);
    void windShiftLeds();
    void snowRainEffects();
    void sunEffects(CRGBPalette16, mappedDataS);
};

#endif
