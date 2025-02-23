class HybridAStarStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
private:
    float calculateSafetyScore(const sf::Vector2i& pos);
    float calculateSpaceScore(const sf::Vector2i& pos);
    Direction findSafestPath(const Snake& snake, const sf::Vector2i& food);
};