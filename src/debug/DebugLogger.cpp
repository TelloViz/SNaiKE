#include "debug/DebugLogger.hpp"

bool DebugLogger::enabled = true;        // Start enabled by default
bool DebugLogger::consoleEnabled = true; // Start enabled by default

void DebugLogger::toggleDebug() {
    bool newState = !enabled;
    enabled = newState;
    consoleEnabled = newState;
    
    DebugLogger::log("Debug logging " + std::string(newState ? "enabled" : "disabled"));
    DebugOverlay::getInstance().setValue("Debug", newState ? "ON" : "OFF");
    DebugOverlay::getInstance().setValue("Console", newState ? "ON" : "OFF");
}