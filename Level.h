//
// Created by Иван Ильин on 19.09.2022.
//

#ifndef SUBMEM_SURF_LEVEL_H
#define SUBMEM_SURF_LEVEL_H

#include "engine/World.h"

class Level {
public:
    enum class ObstacleType {
        Train,
        TrainTramplin,
        Block,
        Wall,
    };
private:
    std::vector<std::shared_ptr<Group>> _rails;
    std::map<int, std::shared_ptr<Group>> _coinPacks;
    std::vector<std::shared_ptr<Group>> _obstacles;
    std::vector<std::string> _dialogs;

    const std::shared_ptr<World> _world;
    void spawnObstacle(ObstacleType type, bool random_pos = false);
    void spawnBook();
    void spawnRandomObstacle(bool random_pos = false);

    uint _railwaysTraveled = 0;
    uint _coins = 0;
    uint _books = 0;

    bool _isPlaying = false;
    bool _isBookOnMap = true;

    std::shared_ptr<Group> initCoinPack(int pack);

    void initDialogs(const std::string& dirName);
    [[nodiscard]] std::string getRandomDialogName();

public:
    explicit Level(std::shared_ptr<World> world);

    void update();
    void playerCollisionCallBack(RigidBody::CollisionInfo);

    [[nodiscard]] bool isPlaying() const { return _isPlaying; }
    void play();
    void free();

    [[nodiscard]] uint score() const { return _coins*10 + _books*300 + _railwaysTraveled*100; }
};


#endif //SUBMEM_SURF_LEVEL_H
