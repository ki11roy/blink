#pragma once

#include <cstdio>

#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE
#define DEBUG(X, ...) printf(X "\n", ##__VA_ARGS__)
#else
#define DEBUG(X, ...)
#endif

