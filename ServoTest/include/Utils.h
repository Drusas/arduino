#ifndef _UTILS_H
#define _UTILS_H

// #define NDEBUG

#if defined NDEBUG
    #define TRACESC(format, ...)
    #define TRACE(format, ...)
#else
    #define TRACESC(format, ...)   sender->GetSerial()->printf( "%s::%s(%d) " format, __FILE__, __FUNCTION__,  __LINE__, __VA_ARGS__ )
    #define TRACE(format, ...)   Serial.printf( "%s::%s(%d) " format, __FILE__, __FUNCTION__,  __LINE__, __VA_ARGS__ )
#endif

float round_up(float value, uint8_t decimal_places);

#endif