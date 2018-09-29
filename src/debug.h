#include <Particle.h>
#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
 #define DEBUG_BEGIN(int) Serial.begin(int)
 #define DEBUG_PRINTLN(str)  Serial.println(str)
 #define DEBUG_PRINT(str)  Serial.print(str)
#else
 #define DEBUG_BEGIN(int)
 #define DEBUG_PRINTLN(str)
 #define DEBUG_PRINT(str)
#endif

#endif
