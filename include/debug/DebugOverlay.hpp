#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include <iomanip>  // For setprecision
#include <unordered_map>
#include "GameController.hpp"

class DebugOverlay {
private:
    sf::RenderWindow debugWindow;
    sf::Font debugFont;
    std::unordered_map<std::string, sf::Text> debugTexts;
    static constexpr float WINDOW_WIDTH = 300.f;  // Slightly smaller width
    static constexpr float WINDOW_HEIGHT = 400.f; // Slightly smaller height
    static constexpr float LINE_HEIGHT = 24.f;    // Increased line height
    static constexpr float PADDING = 15.f;        // Increased padding
    sf::RenderWindow* gameWindow{nullptr};  // Add reference to game window

public:
    static DebugOverlay& getInstance() {
        static DebugOverlay instance;
        return instance;
    }

    void initialize(sf::RenderWindow* mainWindow) {
        gameWindow = mainWindow;
        debugWindow.create(
            sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
            "Debug Overlay",
            sf::Style::Default
        );
        
        if (gameWindow) {
            // Position debug window to the right of game window with small gap
            auto gamePos = gameWindow->getPosition();
            auto gameSize = gameWindow->getSize();
            debugWindow.setPosition(sf::Vector2i(
                gamePos.x + gameSize.x + 10,  // 10 pixel gap
                gamePos.y  // Same vertical position
            ));
        }

        if (!debugFont.loadFromFile("resources/fonts/arial.ttf")) {
            // Fallback to system font if needed
        }
    }

    void setValue(const std::string& key, const std::string& value) {
        if (debugTexts.find(key) == debugTexts.end()) {
            sf::Text text;
            text.setFont(debugFont);
            text.setCharacterSize(16);        // Larger text
            text.setFillColor(sf::Color::White);
            
            // Add category coloring
            if (key.find("Time") != std::string::npos) {
                text.setFillColor(sf::Color(150, 255, 150));  // Light green for time values
            } else if (key == "State") {
                text.setFillColor(sf::Color(150, 150, 255));  // Light blue for state info
            }
            
            text.setPosition(PADDING, PADDING + (debugTexts.size() * LINE_HEIGHT));
            debugTexts[key] = text;
        }
        
        // Format numbers to 2 decimal places if they're numeric
        std::string formattedValue = value;
        try {
            float num = std::stof(value);
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << num;
            formattedValue = ss.str();
        } catch (...) {}
        
        debugTexts[key].setString(key + ": " + formattedValue);
    }

    void update() {
        if (debugWindow.isOpen()) {
            sf::Event event;
            while (debugWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    debugWindow.close();
                }
            }
        }
    }

    void render() {
        if (!debugWindow.isOpen()) return;

        debugWindow.clear(sf::Color(30, 30, 30));

        for (const auto& [key, text] : debugTexts) {
            debugWindow.draw(text);
        }

        debugWindow.display();
    }

    // Add method to update state stack display
    void updateStateStack(const std::vector<std::string>& stackInfo) {
        std::string stackDisplay;
        for (size_t i = 0; i < stackInfo.size(); ++i) {
            stackDisplay += std::string(i * 2, ' ') + "└─ " + stackInfo[i];
            if (i < stackInfo.size() - 1) {
                stackDisplay += "\n";
            }
        }
        setValue("State Stack", stackDisplay);
    }
};