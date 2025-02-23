class VoronoiStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
private:
    float calculateVoronoiScore(const Snake& snake, const sf::Vector2i& pos);
    std::map<sf::Vector2i, float> generateVoronoiMap(const Snake& snake);
};