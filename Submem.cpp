//
// Created by Иван Ильин on 19.09.2022.
//

#include "Submem.h"
#include "engine/Group.h"
#include "engine/io/SoundController.h"

using namespace std;

void Submem::start() {
    // This code executed once in the beginning:

    screen->setMouseCursorVisible(false);

    world->load(ObjectNameTag("Player_hitbox"), SubmemConsts::CUBE_OBJ, SubmemConsts::PLAYER_HIT_BOX);
    auto hb = world->body(ObjectNameTag("Player_hitbox"));
    playerController = std::make_shared<PlayerController>(hb, keyboard, mouse);
    hb->setCollision(true);
    hb->attach(player);
    hb->translate(Vec3D(0, 10, 0));
    hb->setVelocity(Vec3D(0, 0, SubmemConsts::WALK_SPEED));
    hb->setCollisionCallBack([this](RigidBody::CollisionInfo info){level->playerCollisionCallBack(info);});

    //hb->attach(camera);
    world->add(player);
    hb->setVisible(false);

    camera->translateToPoint(hb->position() + SubmemConsts::CAMERA_SHIFT);
    camera->rotateLeft(SubmemConsts::CAMERA_LEFT_ROTATION);

    // change origin for Player's hands and foot:
    world->body(ObjectNameTag("Player_temp_obj_11"))->setOrigin(Vec3D(0, 2.6, 0));
    world->body(ObjectNameTag("Player_temp_obj_12"))->setOrigin(Vec3D(0, 2.6, 0));
    world->body(ObjectNameTag("Player_temp_obj_1"))->setOrigin(Vec3D(0, 1.6, 0));
    world->body(ObjectNameTag("Player_temp_obj_10"))->setOrigin(Vec3D(0, 1.6, 0));

    // loading resources:
    SoundController::load(SoundTag("coin"), SubmemConsts::COIN_OGG);
    SoundController::load(SoundTag("book"), SubmemConsts::BOOK_OGG);
    SoundController::load(SoundTag("death"), SubmemConsts::DEATH_OGG);
    SoundController::load(SoundTag("background"), SubmemConsts::BACKGROUND_OGG);

    SoundController::play(SoundTag("background"));
    
    level->play();
}

void Submem::update() {
    // This code executed every time step:

    auto hb = world->body(ObjectNameTag("Player_hitbox"));

    // Check all input after this condition please
    if (!screen->hasFocus()) {
        return;
    }

    if (keyboard->isKeyTapped(sf::Keyboard::O)) {
        setGlEnable(!glEnable());
    }

    if (keyboard->isKeyTapped(sf::Keyboard::Tab)) {
        setDebugInfo(!showDebugInfo());
    }

    if (keyboard->isKeyTapped(sf::Keyboard::R)) {
        if(!level->isPlaying()) {
            level->play();
        }
    }

    if (keyboard->isKeyTapped(sf::Keyboard::Escape)) {
        exit();
    }

    if(level->isPlaying()) {
        level->update();

        if(hb->inCollision()) {
            player->addRunAnimation();
        }

        playerController->update();

        Vec3D playerPos = getPlayerOveragePosition();

        camera->translateToPoint(SubmemConsts::CAMERA_SHIFT + Vec3D(playerPos.x()/1.2, playerPos.y(), player->position().z()));
        _score = level->score();

    } else {
        // TODO: implement game menu

    }

    //cameraController->update();
}

void Submem::gui() {

    screen->drawText(std::to_string(_score), Vec2D{30, 20}, 100,
                     sf::Color(0, 0, 0, 100));

    if(!level->isPlaying()) {
        screen->drawText("Press R to restart", Vec2D{static_cast<double>(screen->width()/2) - 280, static_cast<double>(screen->height()/2)}, 80,
                         sf::Color(0, 0, 0, 100));
    }
}

Vec3D Submem::getPlayerOveragePosition() {
    int interval = 10;
    _playerPositions.push_back(player->position());
    if(_playerPositions.size() > interval) {
        _playerPositions.pop_front();
    }
    double x_ovr = 0.0;
    double y_ovr = 0.0;
    for(auto& pos : _playerPositions) {
        x_ovr += pos.x();
        y_ovr += pos.y();
    }
    x_ovr /= interval;
    y_ovr /= interval;

    return Vec3D(x_ovr, y_ovr, 0);
}
