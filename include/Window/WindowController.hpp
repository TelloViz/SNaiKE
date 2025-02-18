#pragma once
#include <SFML/Graphics.hpp>

class WindowController {
public:
    virtual ~WindowController() = default;
    
    // Window lifecycle and rendering
    virtual void create(unsigned int width, unsigned int height, const std::string& title) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual void clear() = 0;
    virtual void display() = 0;
    virtual void setVerticalSyncEnabled(bool enabled) = 0;
    
    // Window properties
    virtual void setTitle(const std::string& title) = 0;
    virtual void setSize(unsigned int width, unsigned int height) = 0;
    virtual std::pair<unsigned int, unsigned int> getSize() const = 0;
    
    // Raw window access (for transitioning period)
    virtual void* getNativeHandle() const = 0;

    // Add method to poll window events and translate them to GameInput
    virtual bool pollWindowEvent(GameInput& input) = 0;
};

class SFMLWindowController : public WindowController {
private:
    sf::RenderWindow window;

public:
    SFMLWindowController() = default;
    
    void create(unsigned int width, unsigned int height, const std::string& title) override {
        window.create(sf::VideoMode(width, height), title);
    }
    
    void close() override { window.close(); }
    bool isOpen() const override { return window.isOpen(); }
    void clear() override { window.clear(); }
    void display() override { window.display(); }
    void setVerticalSyncEnabled(bool enabled) override { window.setVerticalSyncEnabled(enabled); }
    
    void* getNativeHandle() const override { return (void*)&window; }
    sf::RenderWindow& getWindow() { return window; }
    
    void setTitle(const std::string& title) override { window.setTitle(title); }
    void setSize(unsigned int width, unsigned int height) override { 
        window.setSize(sf::Vector2u(width, height)); 
    }
    std::pair<unsigned int, unsigned int> getSize() const override {
        auto size = window.getSize();
        return {size.x, size.y};
    }

    bool pollWindowEvent(GameInput& input) override {
        sf::Event event;
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    input = GameInput{InputType::WindowClosed, GameButton::Quit};
                    return true;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Up:     
                            input = GameInput{InputType::ButtonPressed, GameButton::Up}; 
                            return true;
                        case sf::Keyboard::Down:   
                            input = GameInput{InputType::ButtonPressed, GameButton::Down}; 
                            return true;
                        // ...handle other keys...
                    }
                    break;
            }
        }
        return false;
    }
};