#include <FastLED.h>
#include <Particle.h>
#include "hlpFunc.h"

#define debug 1

FASTLED_USING_NAMESPACE;
SYSTEM_MODE(SEMI_AUTOMATIC)
SYSTEM_THREAD(ENABLED)


// Vejbystrand API ID 2663432
// API key 192e4b612a74f2afdc2d3d5e199856e9
// api call: api.openweathermap.org/data/2.5/weather?appid=192e4b612a74f2afdc2d3d5e199856e9&id=2663432&units=metric
String testdata = "800~18.97~4.1~";
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
//==================================================================================
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
    dataReceived = true;
}
//==================================================================
//==============================================================================


//=======================================================================
//===================== MAIN PROGRAM ====================================
//=======================================================================
void setup() {

//#if debug==1
Serial.begin(9600);
Serial.println("Starting....");
//#endif
//==================General setups===============
    Particle.subscribe("hook-response/weather", weatherResponse, MY_DEVICES);
//==============================================

//================= Connection setup =========================
    Particle.connect();
    if (!waitFor(Particle.connected, msRetryTime)) { WiFi.off();  }
// =============================================================
    Particle.publish("weather", PRIVATE);
    Serial.println("Running....");
}

void loop() {
//#if debug==1
  if(dataReceived){
    Serial.println(weatherData.weatherID);
    Serial.println(weatherData.temp);
    Serial.println(weatherData.wind);
  }
//#endif
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
        }
    }
//-----------------------------------------------
//=============== NOT CONNECTED =================
    if(!Particle.connected()){

    }
//-----------------------------------------------
}
