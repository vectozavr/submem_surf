//
// Created by Иван Ильин on 14.03.2021.
//

#ifndef SHOOTER_PLAYER_H
#define SHOOTER_PLAYER_H

#include <SFML/Audio/Sound.hpp>
#include <utility>
#include "../engine/utils/ResourceManager.h"
#include "../engine/Camera.h"
#include "../SubmemConsts.h"
#include "../engine/Group.h"

class Player final : public Group {
private:

public:
    explicit Player();

    void addRunAnimation();
    void addDeathAnimation();
};


#endif //MINECRAFT_3DZAVR_PLAYER_H
