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
#define MAX_SKY_BRIGHTNESS 35



struct skyPaletteS{
  uint8_t brightness;
  CRGBPalette16 palette;
};
extern uint8_t globalBrightness;
void setGlobalBrightness(uint8_t);

class paletteClass{
    static CRGBPalette16 temperaturePalette_static;
    static CRGBPalette16 sunColorPalette_static;
    skyPaletteS skyPalette;
    CRGBPalette16 tempPalette;
    CRGBPalette16 sunPalette;
    CRGBPalette16 sunAndSkyPalette;
    uint8_t temp;
  public:
    CRGBPalette16 getTempPalette(uint8_t);
    CRGBPalette16 getSunAndSkyPalette(int, int, uint8_t);
    skyPaletteS getSkyPalette(){return skyPalette;};
  private:
    void setSkyBrightness(uint8_t);
};

class ledEffects{
    CRGB *leds_p;
    skyPaletteS skyColorsS;
    CRGBPalette16 currentTempPalette;
    CRGBPalette16 currentSunPalette;
    mappedDataS currentData;
    int chanceOfDrops = 0;
    CRGB dropColor = CRGB::White;
    CRGB dropArray[SKYWIDTH];
  public:
    ledEffects(CRGB*);
    void setData(CRGBPalette16, mappedDataS);
    void windShiftLeds();
    void snowRainEffects();
    void SkyAndSunEffects(CRGBPalette16, mappedDataS, skyPaletteS);
  private:
    void snowRainFade();
};

#endif
