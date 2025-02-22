#include "AI/AStarStrategy.hpp"
#include <queue>

Direction AStarStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    if (currentPath.empty() || pathUpdateClock.getElapsedTime().asSeconds() >= PATH_UPDATE_INTERVAL) {
        currentPath = findPath(snake, food);
        pathUpdateClock.restart();
    }

    if (!currentPath.empty()) {
        Direction nextMove = currentPath.front();
        currentPath.erase(currentPath.begin());
        return nextMove;
    }

    // Fallback to simple manhattan distance if no path found
    return snake.getCurrentDirection();
}

std::vector<Direction> AStarStrategy::findPath(const Snake& snake, const sf::Vector2i& food) {
    Position start{snake.getHead()};  // Use uniform initialization
    Position goal{food};              // Use uniform initialization
    
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::map<Position, Position> cameFrom;
    std::map<Position, float> gScore;
    std::map<Position, Direction> directionToParent;

    openSet.push({start, 0, calculateHeuristic(start, food)});
    gScore[start] = 0;

    exploredNodes.clear();  // Clear previous exploration data

    while (!openSet.empty()) {
        Position current = openSet.top().pos;
        openSet.pop();

        exploredNodes.push_back(current.pos);  // Record explored node

        if (current == goal) {
            std::vector<Direction> path;
            Position currentPos = goal;
            
            while (currentPos != start) {
                path.push_back(directionToParent[currentPos]);
                currentPos = cameFrom[currentPos];
            }
            
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = current;
            switch (dir) {
                case Direction::Up:    next.pos.y--; break;
                case Direction::Down:  next.pos.y++; break;
                case Direction::Left:  next.pos.x--; break;
                case Direction::Right: next.pos.x++; break;
            }

            if (!isPositionBlocked(next, snake)) {
                float tentativeGScore = gScore[current] + 1;
                
                if (!gScore.count(next) || tentativeGScore < gScore[next]) {
                    cameFrom[next] = current;
                    gScore[next] = tentativeGScore;
                    directionToParent[next] = dir;
                    
                    openSet.push({
                        next,
                        tentativeGScore,
                        calculateHeuristic(next, food)
                    });
                }
            }
        }
    }

    return {};
}

float AStarStrategy::calculateHeuristic(const Position& pos, const sf::Vector2i& goal) const {
    return getManhattanDistance(pos, goal);
}

void AStarStrategy::update() {
    // Empty for now - can be used for path visualization updates
}

void AStarStrategy::render(sf::RenderWindow& window) const {
    sf::RectangleShape cell(sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE - 2));
    
    // First draw explored nodes with heat map coloring
    for (const auto& node : exploredNodes) {
        // Calculate exploration heat (nodes explored more recently are "hotter")
        float heat = static_cast<float>(std::distance(exploredNodes.begin(), 
            std::find(exploredNodes.begin(), exploredNodes.end(), node))) / exploredNodes.size();
        
        // Create gradient from blue (cold/old) to red (hot/recent)
        sf::Color cellColor(
            static_cast<sf::Uint8>(255 * heat),     // Red
            0,                                       // Green
            static_cast<sf::Uint8>(255 * (1-heat)), // Blue
            64                                       // Alpha (transparency)
        );
        
        cell.setFillColor(cellColor);
        cell.setPosition(
            node.x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES + 1,
            node.y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP + 1
        );
        window.draw(cell);
    }
    
    // Then draw the current path with a bright, distinct color
    if (!currentPath.empty()) {
        sf::Vector2i currentPos = snake.getHead();
        cell.setFillColor(sf::Color(0, 255, 0, 128));  // Bright semi-transparent green
        
        for (const Direction& dir : currentPath) {
            switch (dir) {
                case Direction::Up:    currentPos.y--; break;
                case Direction::Down:  currentPos.y++; break;
                case Direction::Left:  currentPos.x--; break;
                case Direction::Right: currentPos.x++; break;
            }
            
            cell.setPosition(
                currentPos.x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES + 1,
                currentPos.y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP + 1
            );
            window.draw(cell);
            
            // Draw direction arrow
            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setFillColor(sf::Color::White);
            float arrowSize = GameConfig::CELL_SIZE * 0.3f;
            
            // Set arrow points based on direction
            switch (dir) {
                case Direction::Up:
                    arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE/2, 2));
                    arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE/2 - arrowSize, arrowSize + 2));
                    arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE/2 + arrowSize, arrowSize + 2));
                    break;
                case Direction::Down:
                    arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE/2, GameConfig::CELL_SIZE - 2));
                    arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE/2 - arrowSize, GameConfig::CELL_SIZE - arrowSize - 2));
                    arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE/2 + arrowSize, GameConfig::CELL_SIZE - arrowSize - 2));
                    break;
                case Direction::Left:
                    arrow.setPoint(0, sf::Vector2f(2, GameConfig::CELL_SIZE/2));
                    arrow.setPoint(1, sf::Vector2f(arrowSize + 2, GameConfig::CELL_SIZE/2 - arrowSize));
                    arrow.setPoint(2, sf::Vector2f(arrowSize + 2, GameConfig::CELL_SIZE/2 + arrowSize));
                    break;
                case Direction::Right:
                    arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE/2));
                    arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE - arrowSize - 2, GameConfig::CELL_SIZE/2 - arrowSize));
                    arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE - arrowSize - 2, GameConfig::CELL_SIZE/2 + arrowSize));
                    break;
            }
            
            arrow.setPosition(
                currentPos.x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES,
                currentPos.y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP
            );
            window.draw(arrow);
        }
    }
}