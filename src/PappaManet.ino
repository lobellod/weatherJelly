#include <FastLED.h>
#include <Particle.h>
#include "hlpFunc.h"
#include "LEDcontrol.h"
#define debug 1  //set to 1 for debug prints

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
String weatherDataStr = String(20); //Strin for unparsed weather data

const uint32_t msGetWeather = 60000*30; //Period for retrieving weather data (30mins)
bool getData = true;
Timer getDataTimer(msGetWeather, toggleGetData);
weatherDataS weatherData;
mappedDataS LedControlData;
//------------------------------------------------------------------------------
//======================= LEDS =================================================
CRGB leds[NUM_LEDS];
CRGBPalette16 tempPalette;
CRGBPalette16 windPalette;
TBlendType    currentBlending;
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
//===================== Data Retrieving ===========================
void toggleGetData(){         //set flag to send request for new data
    getData = true;
}
void weatherResponse(const char *event, const char *weatherDataStr){    //response handler
    weatherDataParser parsedData(weatherDataStr);
    weatherData = parsedData.Data();
    dataToLedConverter mappedData(weatherData);
    LedControlData = mappedData.Data();
    dataReceived = true;
}
//---------------------------------------------------------------------
//==============================================================================


//=======================================================================
//===================== MAIN PROGRAM ====================================
//=======================================================================
void setup() {

#if debug==1
  Serial.begin(9600);
  Serial.println("Starting....");
#endif
//==================General setups===============
    Particle.subscribe("hook-response/weather", weatherResponse, MY_DEVICES);
    getDataTimer.start();
//==============================================

//================= Connection setup =========================
    Particle.connect();
    if (!waitFor(Particle.connected, msRetryTime)) { WiFi.off();  }
    else{
      Particle.publish("weather", PRIVATE);
    }
// =============================================================

#if debug==1
  Serial.println("Running....");
#endif

FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

void loop() {
#if debug==1
  if(dataReceived){
    Serial.println("parsed data:");
    Serial.println(weatherData.weatherID);
    Serial.println(weatherData.temp);
    Serial.println(weatherData.wind);
    Serial.println("mapped data:");
    Serial.println(LedControlData.type);
    Serial.println(LedControlData.temp);
    Serial.println(LedControlData.wind);
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
            Particle.publish("weather", PRIVATE);
            getData = false;
            getDataTimer.reset();
        }
        if(dataReceived){
          temperaturePalette Palette(LedControlData.temp);
          tempPalette = Palette.getPalette();
          ledEffects setupLedEffects(&leds[0],tempPalette, LedControlData);
          setupLedEffects.windShiftLeds();
          FastLED.show();
        }
    }
//-----------------------------------------------
//=============== NOT CONNECTED =================
    if(!Particle.connected()){

    }
//-----------------------------------------------
}
