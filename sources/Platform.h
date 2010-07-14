/* 
 * File:   Platform.h
 * Author: b4n92uid
 *
 * Created on 6 juin 2010, 13:18
 */

#ifndef _PLATFORM_H
#define	_PLATFORM_H

#if defined(__WIN32__) || defined(WIN32)
#define COMPILE_FOR_WINDOWS
#include <windows.h>

#elif defined(__linux__)
#define COMPILE_FOR_LINUX

#else
#error Platform not suported

#endif

#endif	/* _PLATFORM_H */

