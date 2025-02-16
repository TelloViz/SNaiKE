#pragma once
#include "debug/DebugOverlay.hpp"
#include <iostream>
#include <string>

class DebugLogger {
private:
    static bool enabled;
    static bool consoleEnabled;

public:
    static bool isEnabled() { return enabled && consoleEnabled; }
    
    static void setEnabled(bool enable) { 
        enabled = enable;
        consoleEnabled = enable; // Make sure both flags are in sync
        DebugOverlay::getInstance().setValue("Debug", enable ? "ON" : "OFF");
    }
    
    static void toggleDebug();
    
    template<typename T>
    static void log(const T& message) {
        if (isEnabled()) {  // Check both flags
            std::cout << "[DEBUG] " << message << std::endl;
        }
    }
};