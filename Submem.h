//
// Created by Иван Ильин on 19.09.2022.
//

#ifndef SUBMEM_SURF_SUBMEM_H
#define SUBMEM_SURF_SUBMEM_H

#include "engine/Engine.h"
#include "player/Player.h"
#include "player/PlayerController.h"
#include "player/PlayerController.h"
#include "engine/utils/ObjectController.h"
#include "Level.h"

class Submem final : public Engine {
private:
    std::shared_ptr<Player> player = std::make_shared<Player>();
    std::shared_ptr<PlayerController> playerController;

    std::list<Vec3D> _playerPositions;

    std::shared_ptr<ObjectController> cameraController = std::make_shared<ObjectController>(camera, mouse);

    std::shared_ptr<Level> level = std::make_shared<Level>(world);

    void start() override;
    void update() override;
    void gui() override;
    [[nodiscard]] Vec3D getPlayerOveragePosition();

    uint _score = 0;

public:
    Submem() {};
};


#endif //SUBMEM_SURF_SUBMEM_H
