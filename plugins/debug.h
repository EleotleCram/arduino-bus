#pragma once

#include <Arduino.h>

#define _OFF_		0
#define _FATAL_	1
#define _ERROR_	2
#define _WARN_	3
#define _INFO_	4
#define _DEBUG_	5
#define _TRACE_	6

#define DEBUG_LEVEL _INFO_

#if DEBUG_LEVEL >= _FATAL_
#define LOG_FATAL(X) Serial.println(X)
#else
#define LOG_FATAL(X)
#endif

#if DEBUG_LEVEL >= _ERROR_
#define LOG_ERROR(X) Serial.println(X)
#else
#define LOG_ERROR(X)
#endif

#if DEBUG_LEVEL >= _WARN_
#define LOG_WARN(X) Serial.println(X)
#else
#define LOG_WARN(X)
#endif

#if DEBUG_LEVEL >= _INFO_
#define LOG_INFO(X) Serial.println(X)
#else
#define LOG_INFO(X)
#endif

#if DEBUG_LEVEL >= _DEBUG_
#define LOG_DEBUG(X) Serial.println(X)
#else
#define LOG_DEBUG(X)
#endif

#if DEBUG_LEVEL >= _TRACE_
#define LOG_TRACE(X) Serial.println(X)
#else
#define LOG_TRACE(X)
#endif
