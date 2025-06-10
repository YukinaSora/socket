#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _MSC_VER
#include "msvc.h"
#elif __GNUC__
#include "gcc.h"
#endif

#endif // !PLATFORM_H