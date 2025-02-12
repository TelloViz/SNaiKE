#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>

const int CELL_SIZE = 20;
const int GRID_WIDTH = 40;
const int GRID_HEIGHT = 30;

enum class Direction { Up, Down, Left, Right };

class Snake {
private:
    std::deque<sf::Vector2i> body;
    Direction direction;
    bool hasEaten;

public:
    Snake() : direction(Direction::Right), hasEaten(false) {
        // Start with 3 segments
        body.push_front(sf::Vector2i(5, 5));
        body.push_front(sf::Vector2i(6, 5));
        body.push_front(sf::Vector2i(7, 5));
    }

    void setDirection(Direction newDir) {
        // Prevent 180-degree turns
        if ((direction == Direction::Up && newDir != Direction::Down) ||
            (direction == Direction::Down && newDir != Direction::Up) ||
            (direction == Direction::Left && newDir != Direction::Right) ||
            (direction == Direction::Right && newDir != Direction::Left)) {
            direction = newDir;
        }
    }

    void move() {
        sf::Vector2i head = body.front();
        switch (direction) {
            case Direction::Up: head.y--; break;
            case Direction::Down: head.y++; break;
            case Direction::Left: head.x--; break;
            case Direction::Right: head.x++; break;
        }
        body.push_front(head);
        if (!hasEaten) {
            body.pop_back();
        }
        hasEaten = false;
    }

    bool checkCollision() {
        sf::Vector2i head = body.front();
        
        // Wall collision
        if (head.x < 0 || head.x >= GRID_WIDTH || 
            head.y < 0 || head.y >= GRID_HEIGHT) {
            return true;
        }

        // Self collision
        for (size_t i = 1; i < body.size(); i++) {
            if (head == body[i]) return true;
        }
        return false;
    }

    bool eat(const sf::Vector2i& food) {
        if (body.front() == food) {
            hasEaten = true;
            return true;
        }
        return false;
    }

    void draw(sf::RenderWindow& window) {
        for (const auto& segment : body) {
            sf::RectangleShape rect(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
            rect.setPosition(segment.x * CELL_SIZE, segment.y * CELL_SIZE);
            rect.setFillColor(sf::Color::Green);
            window.draw(rect);
        }
    }
};

int main() {
    auto window = sf::RenderWindow(
        sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE),
        "Snake Game"
    );
    window.setFramerateLimit(10); // Slower speed for snake movement

    Snake snake;
    sf::Vector2i food(15, 15);
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Up: snake.setDirection(Direction::Up); break;
                    case sf::Keyboard::Down: snake.setDirection(Direction::Down); break;
                    case sf::Keyboard::Left: snake.setDirection(Direction::Left); break;
                    case sf::Keyboard::Right: snake.setDirection(Direction::Right); break;
                }
            }
        }

        snake.move();
        
        if (snake.checkCollision()) {
            window.close();
        }

        if (snake.eat(food)) {
            // Generate new food position
            food = sf::Vector2i(rand() % GRID_WIDTH, rand() % GRID_HEIGHT);
        }

        window.clear(sf::Color::Black);
        
        // Draw food
        sf::RectangleShape foodShape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
        foodShape.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
        foodShape.setFillColor(sf::Color::Red);
        window.draw(foodShape);
        
        snake.draw(window);
        window.display();
    }

    return 0;
}
