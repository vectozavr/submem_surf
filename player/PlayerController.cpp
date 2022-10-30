//
// Created by Иван Ильин on 19.09.2021.
//

#include "PlayerController.h"
#include "../engine/utils/Log.h"
#include "../engine/animation/Animations.h"
#include "../engine/io/SoundController.h"

PlayerController::PlayerController(std::shared_ptr<RigidBody> player,
                                   std::shared_ptr<Keyboard> keyboard,
                                   std::shared_ptr<Mouse> mouse) : _player(player), _keyboard(keyboard),
                                                                   _mouse(mouse) {}

void PlayerController::update() {

    auto hor_movements = AnimationListTag("player_hor_movements");
    // Left and right

    if(!Timeline::isInAnimList(hor_movements)) {
        if (_keyboard->isKeyTapped(sf::Keyboard::A) || _keyboard->isKeyTapped(sf::Keyboard::Left)) {
            if(_playerPosition < 1) {
                _playerPosition++;

                Timeline::addAnimation<ATranslate>(hor_movements, _player, Vec3D(SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x(), 0, 0), SubmemConsts::HOR_WALK_TIME,
                                                   Animation::LoopOut::None, Animation::InterpolationType::Cos);
            }
        }
        if (_keyboard->isKeyTapped(sf::Keyboard::D) || _keyboard->isKeyTapped(sf::Keyboard::Right)) {
            if(_playerPosition > -1) {
                _playerPosition--;
                Timeline::addAnimation<ATranslate>(hor_movements, _player, Vec3D(-SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x(), 0, 0), SubmemConsts::HOR_WALK_TIME,
                                                   Animation::LoopOut::None, Animation::InterpolationType::Cos);
            }
        }
    }

    // Forward and backward
    if ((Keyboard::isKeyPressed(sf::Keyboard::W) || _keyboard->isKeyTapped(sf::Keyboard::Up)) && _player->inCollision()) {
        _player->addVelocity(Vec3D{0, sqrt(2 * SubmemConsts::GRAVITY * SubmemConsts::JUMP_HEIGHT), 0});
    }

    if ((_keyboard->isKeyTapped(sf::Keyboard::S) || _keyboard->isKeyTapped(sf::Keyboard::Down)) && !_player->inCollision()) {
        _player->setVelocity(Vec3D{0, -2*sqrt(2 * SubmemConsts::GRAVITY * SubmemConsts::JUMP_HEIGHT), _player->velocity().z()});
    }

    if (Keyboard::isKeyPressed(sf::Keyboard::R) && SoundController::getStatus(SoundTag("dialog")) == sf::Sound::Status::Playing) {
        SoundController::stop(SoundTag("dialog"));
    }

    checkLine();
}

void PlayerController::checkLine() {
    auto hor_movements = AnimationListTag("player_hor_movements");
    double hor_shift = std::abs(std::abs(_player->position().x()) - SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x());

    if(!Timeline::isInAnimList(hor_movements)) {
        if(_playerPosition != 0) {
            if(hor_shift > Consts::EPS) {
                Timeline::addAnimation<ATranslate>(hor_movements, _player, Vec3D(_playerPosition*hor_shift, 0, 0), 0.2,
                                                   Animation::LoopOut::None, Animation::InterpolationType::Cos);
            }
        } else if(std::abs(_player->position().x()) > Consts::EPS) {
            Timeline::addAnimation<ATranslate>(hor_movements, _player, Vec3D(-_player->position().x(), 0, 0), 0.2,
                                               Animation::LoopOut::None, Animation::InterpolationType::Cos);
        }
    }
}
