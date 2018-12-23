#include <Particle.h>
#ifndef DEBUG_H
#define DEBUG_H


 //#define DEBUG 2  //set to 1 for debug prints; 2 for custom string; undefine for normal
#ifdef DEBUG
 #define DEBUG_BEGIN(...) Serial.begin(__VA_ARGS__)
 #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
 #define DEBUG_PRINT(...)  Serial.print(__VA_ARGS__)
 #define DEBUG_DELAY(...) delay(__VA_ARGS__)
#else
 #define DEBUG_BEGIN(...)
 #define DEBUG_PRINTLN(...)
 #define DEBUG_PRINT(...)
 #define DEBUG_DELAY(...)
#endif

#endif
