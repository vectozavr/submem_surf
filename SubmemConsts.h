//
// Created by Иван Ильин on 10.10.2021.
//

#ifndef SHOOTER_SHOOTERCONSTS_H
#define SHOOTER_SHOOTERCONSTS_H

#include "engine/Consts.h"

namespace SubmemConsts {
    const Vec3D OBJ_SCALE = Vec3D(3, 3, 3);
    const Vec3D PLAYER_HIT_BOX = Vec3D(0.5, 1.5, 0.5);
    const double CAMERA_LEFT_ROTATION = 0.05*Consts::PI;
    const Vec3D CAMERA_SHIFT = Vec3D(0, 6, -15);
    const double GRAVITY = 80;
    const double JUMP_HEIGHT = 7;
    const double WALK_SPEED = 50;
    const double SPEED_BOOST = WALK_SPEED/50000;
    const double HOR_WALK_TIME = 0.2;
    const double TRAIN_SPEED = 50;
    const uint RAILWAYS_NUMBER = 7;
    const uint MAX_DISTANCE_OBSTACLE_SPAWN = 5000;
    const uint DISTANCE_OBSTACLE_SPAWN = 1000;
    const uint COIN_PACKS_NUMBER = 10;
    const uint ONE_PACK_COINS_NUMBER = 5;
    const uint INITIAL_OBSTACLES_NUMBER = 15;
    const uint MAX_OBSTACLES_NUMBER = 100;
    const double LENGTH_OF_RAILWAY_SECTION = 48;
    const double LENGTH_OF_HORIZONTAL_MOVEMENT = 2.1;

    const std::string PLAYER_NAME = "Player";
    const std::string PROJECT_NAME = "Submem surf";

    const std::string CUBE_OBJ = "obj/cube.obj";

    const std::string BOOK_OBJ = "obj/book.obj";
    const std::string COIN_OBJ = "obj/coin.obj";
    const std::string TRAIN_OBJ = "obj/train_long.obj";
    const std::string TRAIN_TRAMPLIN_OBJ = "obj/train_tramplin.obj";
    const std::string BLOCK_OBJ = "obj/block.obj";
    const std::string WALL_OBJ = "obj/wall.obj";
    const std::string RAILWAY_OBJ = "obj/railway.obj";
    const std::string BLICK_OBJ = "obj/blick.obj";
    const std::string MAN_OBJ = "obj/man.obj";

    const std::string COIN_OGG = "sounds/sound/coin.ogg";
    const std::string BOOK_OGG = "sounds/sound/book.ogg";
    const std::string DEATH_OGG = "sounds/sound/hurt.ogg";
    const std::string SOUNDTRACK_OGG = "sounds/sound/.ogg";
    const std::string BACKGROUND_OGG = "sounds/sound/backNoise.ogg";
}

#endif //SHOOTER_SHOOTERCONSTS_H
