#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <android/log.h>

#define LOG_TAG "ANTI-RK"
#define ENABLE_DEBUG 1

#ifdef ENABLE_DEBUG
	#define EGIS_DEBUG(fmt, args...)  do{ __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args); }while(0)
#else
	#define EGIS_DEBUG(fmt, args...)  do{}while(0)
#endif
	
#define EGIS_ERROR(fmt, args...)  do{ __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args); }while(0)


#endif
/* _DEBUG_H_*/
