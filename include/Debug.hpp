#pragma once
#include <iostream>

#ifdef _DEBUG
    #define DEBUG_MODE 1
#else
    #define DEBUG_MODE 0
#endif

#define AI_DEBUG_OUTPUT 0  // Set to 1 to enable AI debug output

#define AI_DEBUG(x) do { \
    if (AI_DEBUG_OUTPUT && DEBUG_MODE) { \
        std::cout << x << std::endl; \
    } \
} while(0)