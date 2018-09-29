#include <FastLED.h>
#include <Particle.h>
#include "hlpFunc.h"
#include "LEDcontrol.h"

#define DEBUG 2  //set to 1 for debug prints; 2 for custom string; undefine for normal
#include "debug.h"

FASTLED_USING_NAMESPACE;
SYSTEM_MODE(SEMI_AUTOMATIC)
SYSTEM_THREAD(ENABLED)

//==================== VARIABLES & DECLARATIONS =======================================
bool dataReceived = false;

//======================== Connection  ===============================
const uint32_t msRetryDelay = 60*60000; // retry every 60min
const uint32_t msRetryTime  =   30000; // stop trying after 30sec

bool   retryRunning = false;
Timer retryTimer(msRetryDelay, retryConnect);  // timer to retry connecting
Timer stopTimer(msRetryTime, stopConnect);     // timer to stop a long running try
//------------------------------------------------------------------
//======================== Data Retrieving ===================================
String weatherDataStr = String(100); //Strin for unparsed weather data

const uint32_t msGetWeather = 60000*30; //Period for retrieving weather data (30mins)
bool getData = false;
Timer getDataTimer(msGetWeather, toggleGetData);
weatherDataS weatherData;
mappedDataS LedControlData;
sunTimesS sunTime;

weatherDataParser parsedData;
dataToLedConverter mappedData;
//======================= Timekeeping =================================================
int tSec = 0;
const uint32_t msSecond = 1000;
Timer secondTimer(msSecond, timeKeeper);
//-------------------------------------------------------------------------------
//======================= LEDS =================================================
CRGB leds[NUM_LEDS];
CRGBPalette16 tempPalette;
CRGBPalette16 windPalette;
CRGBPalette16 sunPalette;
TBlendType    currentBlending;
ledEffects ledEffect(&leds[0]);
paletteClass palettes;
//-------------------------------------------------------------------------------
//===================================================================================


//====================== FUNCTION DECLARATIONS =====================================

//====================== Connection  =======================================
void retryConnect()
{
  if (!Particle.connected())   // if not connected to cloud
  {
    stopTimer.start();         // set of the timout time
    WiFi.on();
    Particle.connect();        // start a reconnectino attempt
  }
  else                         // if already connected
  {
    retryTimer.stop();         // no further attempts required
    retryRunning = false;
  }
}

void stopConnect()
{
    if (!Particle.connected()) // if after retryTime no connection
      WiFi.off();              // stop trying and swith off WiFi
    stopTimer.stop();
}
//------------------------------------------------------------------
//===================== TimeKeeping ===========================
void timeKeeper(){
  tSec = tSec+1;
}
void resetTimeKeeper(){
  tSec = 0;
  secondTimer.reset();
}
//---------------------------------------------------------------------
//===================== Data Retrieving ===========================
void toggleGetData(){         //set flag to send request for new data
    getData = true;
}
void weatherResponse(const char *event, const char *weatherDataStr){    //response handler
    weatherData = parsedData.parseData(weatherDataStr);
    mappedData.setLedConverterData(weatherData);
    LedControlData = mappedData.getData();
    tempPalette = palettes.getTempPalette(LedControlData.temp);
    ledEffect.setData(tempPalette, LedControlData);
    resetTimeKeeper();
    dataReceived = true;
}
//---------------------------------------------------------------------
//==============================================================================


//=======================================================================
//===================== MAIN PROGRAM ====================================
//=======================================================================
void setup() {

  DEBUG_BEGIN(9600);
  DEBUG_PRINTLN("Starting....");

//==================General setups===============
    Particle.subscribe("hook-response/weather", weatherResponse, MY_DEVICES);
    getDataTimer.start();
    secondTimer.start();
//==============================================

//================= Connection setup =========================
    Particle.connect();
    if (!waitFor(Particle.connected, msRetryTime)) { WiFi.off();  }
    else{
      #if debug <2
      Particle.publish("weather", PRIVATE);
      #endif
    }
// =============================================================

#if DEBUG==2
    String testDataStr = String(100);
    //"weatherID ~ temp ~ wind ~ clouds ~ currentTime ~sunrise ~sunset"
    testDataStr = "804~19.66~4.6~90~1527420000~1527388461~1527449936~";
    weatherResponse("debug_event", testDataStr);
    DEBUG_PRINTLN("mapped data:");
    DEBUG_PRINTLN(LedControlData.type);
    DEBUG_PRINTLN(LedControlData.temp);
    DEBUG_PRINTLN(LedControlData.wind);
    DEBUG_PRINTLN("Other Data:");
    DEBUG_PRINT("SKYLEDSTART: ");
    DEBUG_PRINTLN(SKYLEDSTART);
    DEBUG_PRINT("SKYLEDEND: ");
    DEBUG_PRINTLN(SKYLEDEND);

#endif
delay(3000);
//============================ LED Setup ===================================
FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
fill_solid( leds, NUM_LEDS, CRGB::Red);
FastLED.show();
delay(3000);
//=========================================================================
}

void loop() {
#if DEBUG==1
  if(dataReceived){
    DEBUG_PRINTLN("parsed data:");
    DEBUG_PRINTLN(weatherData.weatherID);
    DEBUG_PRINTLN(weatherData.temp);
    DEBUG_PRINTLN(weatherData.wind);
    DEBUG_PRINTLN("mapped data:");
    DEBUG_PRINTLN(LedControlData.type);
    DEBUG_PRINTLN(LedControlData.temp);
    DEBUG_PRINTLN(LedControlData.wind);
  }
#endif

//=========== Connection retry ====================
    if (!retryRunning && !Particle.connected())
    { // if we have not already scheduled a retry and are not connected
        stopTimer.start();         // set timeout for auto-retry by system
        retryRunning = true;
        retryTimer.start();        // schedula a retry
    }
//-------------------------------------------------

//=============== CONNECTED =====================
    if(Particle.connected()){
        if(getData){
          #if DEBUG < 2
            Particle.publish("weather", PRIVATE);
          #endif
          #if DEBUG == 2
            String testDataStr = String(100);
            //"weatherID ~ temp ~ wind ~ clouds ~ currentTime ~sunrise ~sunset"
            testDataStr = "804~19.66~4.6~90~1527420000~1527388461~1527449936~";
            weatherResponse("debug_event", testDataStr);
          #endif
            getData = false;
            getDataTimer.reset();
        }
        if(dataReceived){
          mappedData.timeForSunUpdate(tSec);

          LedControlData = mappedData.getData();
          sunPalette = palettes.getSunPalette(LedControlData.sunTime.timeToRise,LedControlData.sunTime.timeToSet);

          ledEffect.sunEffects(sunPalette, LedControlData);
          ledEffect.windShiftLeds();
          FastLED.show();
        }
    }
//-----------------------------------------------
//=============== NOT CONNECTED =================
    if(!Particle.connected()){

    }
//-----------------------------------------------
}
