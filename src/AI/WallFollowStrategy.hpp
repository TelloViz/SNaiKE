class WallFollowStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
private:
    bool isWallAdjacent(const sf::Vector2i& pos);
    Direction findWallDirection(const Snake& snake);
};