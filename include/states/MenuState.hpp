#pragma once
#include "../State.hpp"
#include <vector>

class MenuState : public State {
private:
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    int selectedOption;

public:
    explicit MenuState(Game* game, const StateContext& context);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    void pause() override;
    void resume() override;
};