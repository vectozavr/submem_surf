//
// Created by Иван Ильин on 19.09.2022.
//

#include <filesystem>
#include <algorithm>
#include <random>

#include "Level.h"
#include "SubmemConsts.h"
#include "engine/animation/Animations.h"
#include "engine/io/SoundController.h"

Level::Level(std::shared_ptr<World> world): _world(world) {}

void Level::play() {
    // create railways:
    for(int i = 0; i < SubmemConsts::RAILWAYS_NUMBER; i++) {
        auto railway = std::make_shared<Group>(ObjectNameTag("railway_" + std::to_string(i)));
        railway->loadGroup(SubmemConsts::RAILWAY_OBJ, SubmemConsts::OBJ_SCALE);

        _world->add(railway);
        railway->setCollider(false);
        railway->body(ObjectNameTag("railway_" + std::to_string(i) + "_temp_obj_524"))->setCollider(true);

        railway->translate(Vec3D(0, 0, SubmemConsts::LENGTH_OF_RAILWAY_SECTION*SubmemConsts::OBJ_SCALE.z()*i));
        _rails.push_back(railway);
    }

    // create coin packs:
    for(int i = 0; i < SubmemConsts::COIN_PACKS_NUMBER; i++) {
        initCoinPack(i);
    }

    // add initial obstacles:
    for(int i = 0; i < SubmemConsts::INITIAL_OBSTACLES_NUMBER; i++) {
        spawnRandomObstacle(true);
    }

    spawnObstacle(ObstacleType::Wall);

    auto hb = _world->body(ObjectNameTag("Player_hitbox"));
    hb->setAcceleration(Vec3D(0, -SubmemConsts::GRAVITY, 0));

    _isPlaying = true;

    initDialogs("sounds/dialogs");
    spawnBook();
}

void Level::free() {
    for (auto &[pack, obj] : _coinPacks) {
        _world->removeGroup(obj->name());
    }
    _coinPacks.clear();

    for (auto &rail : _rails) {
        _world->removeGroup(rail->name());
    }
    _rails.clear();

    for (auto &obs : _obstacles) {
        _world->removeGroup(obs->name());
    }
    _obstacles.clear();
    _world->remove(ObjectNameTag("book"));

    auto player = _world->body(ObjectNameTag("Player_hitbox"));
    player->translateToPoint(Vec3D(0, 10, 0));
    _isPlaying = false;
    player->setAcceleration(Vec3D(0));
    player->setVelocity(Vec3D(0));
    _railwaysTraveled = 0;
    _coins = 0;
    _books = 0;
    _isBookOnMap = true;
    _dialogs.clear();
}

void Level::spawnObstacle(ObstacleType type, bool random_pos) {
    int line = (int)floor(3*static_cast<double>(rand())/RAND_MAX) - 1;
    std::shared_ptr<Group> obstacle;

    auto player = _world->body(ObjectNameTag("Player_hitbox"));
    double zPos = 0;
    if(player != nullptr) {
        zPos = player->position().z();
    }
    double zShift = zPos + SubmemConsts::DISTANCE_OBSTACLE_SPAWN;
    if(random_pos) {
        zShift = zPos + SubmemConsts::MAX_DISTANCE_OBSTACLE_SPAWN * (0.5 + 0.5 * static_cast<double>(rand()) / RAND_MAX);
    }

    if(type == ObstacleType::Train) {
        auto train = std::make_shared<Group>(ObjectNameTag("train_" + std::to_string(_obstacles.size())));
        train->load(ObjectNameTag("train_body_" + std::to_string(_books)), SubmemConsts::TRAIN_OBJ, SubmemConsts::OBJ_SCALE);

        if(static_cast<double>(rand())/RAND_MAX > 0.3) {
            train->setVelocity(Vec3D(0, 0, -SubmemConsts::TRAIN_SPEED));
        }

        obstacle = train;
    }
    if(type == ObstacleType::TrainTramplin) {
        auto train = std::make_shared<Group>(ObjectNameTag("train_" + std::to_string(_obstacles.size())));
        train->loadGroup(SubmemConsts::TRAIN_TRAMPLIN_OBJ, SubmemConsts::OBJ_SCALE);

        obstacle = train;
    }
    if(type == ObstacleType::Block) {
        auto block = std::make_shared<Group>(ObjectNameTag("block_" + std::to_string(_obstacles.size())));
        block->load(ObjectNameTag("block_body_" + std::to_string(_books)), SubmemConsts::BLOCK_OBJ, SubmemConsts::OBJ_SCALE);

        obstacle = block;
    }
    if(type == ObstacleType::Wall) {
        auto block = std::make_shared<Group>(ObjectNameTag("wall_" + std::to_string(_obstacles.size())));
        block->loadGroup(SubmemConsts::WALL_OBJ, SubmemConsts::OBJ_SCALE);

        obstacle = block;
        obstacle->translate(Vec3D(-line*SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x(),
                                  0,  0));
    }

    obstacle->translate(Vec3D(line*SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x(),
                           0,  zShift));
    obstacle->rotateUp(Consts::PI);
    _world->add(obstacle);
    _obstacles.push_back(obstacle);
}

void Level::spawnRandomObstacle(bool random_pos) {
    ObstacleType type = static_cast<ObstacleType>(floor(3*static_cast<double>(rand())/RAND_MAX));
    spawnObstacle(type, random_pos);
}

void Level::spawnBook() {
    _world->forceRemove(ObjectNameTag("book"));
    _world->load(ObjectNameTag("book"), SubmemConsts::BOOK_OBJ, SubmemConsts::OBJ_SCALE);
    auto book = _world->body(ObjectNameTag("book"));
    book->setCollider(false);
    Timeline::addAnimation<ARotate>(book, Vec3D(0, Consts::PI, 0), 1, Animation::LoopOut::Continue, Animation::InterpolationType::Linear);
    book->setTrigger(true);

    int line = (int)floor(3*static_cast<double>(rand())/RAND_MAX) - 1;
    auto player = _world->body(ObjectNameTag("Player_hitbox"));
    double zPos = 0;
    if(player != nullptr) {
        zPos = player->position().z();
    }
    double zShift = zPos + SubmemConsts::DISTANCE_OBSTACLE_SPAWN;
    book->translateToPoint(Vec3D(line*SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x(), 0, zShift));
}

void Level::update() {
    if(!_isPlaying) {
        return;
    }

    auto player = _world->body(ObjectNameTag("Player_hitbox"));

    uint traveled = (uint)floor(player->position().z()/(SubmemConsts::LENGTH_OF_RAILWAY_SECTION*SubmemConsts::OBJ_SCALE.z()));
    if(traveled > _railwaysTraveled) {
        uint index = _railwaysTraveled%SubmemConsts::RAILWAYS_NUMBER;
        _railwaysTraveled++;

        _rails[index]->translate(Vec3D(0, 0, SubmemConsts::RAILWAYS_NUMBER*SubmemConsts::LENGTH_OF_RAILWAY_SECTION*SubmemConsts::OBJ_SCALE.z()));
    }

    double zPos = player->position().z();
    for (auto &[pack, obj] : _coinPacks) {
        if(zPos - obj->position().z() > SubmemConsts::LENGTH_OF_RAILWAY_SECTION*SubmemConsts::OBJ_SCALE.z()) {
            initCoinPack(pack);
        }
    }

    for (auto &obstacle : _obstacles) {
        if(zPos - obstacle->position().z() > SubmemConsts::LENGTH_OF_RAILWAY_SECTION*SubmemConsts::OBJ_SCALE.z()) {
            double zShift = zPos + SubmemConsts::MAX_DISTANCE_OBSTACLE_SPAWN * (0.5 + 0.5 * static_cast<double>(rand()) / RAND_MAX);
            int line = (int)floor(3*static_cast<double>(rand())/RAND_MAX) - 1;
            if(obstacle->name().contains("wall")) {
                line = 0;
            }
            obstacle->translateToPoint(Vec3D(line*SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x(),0, zShift));
        }
    }

    if(player->position().y() < -30) {
        player->translateToPoint(Vec3D(player->position().x(), 10, player->position().z()));
    }

    player->setVelocity(Vec3D(0, player->velocity().y(), SubmemConsts::WALK_SPEED + SubmemConsts::SPEED_BOOST*score()));

    if(_obstacles.size() - SubmemConsts::INITIAL_OBSTACLES_NUMBER < (uint)(score()/1000)) {
        if(_obstacles.size() < SubmemConsts::MAX_OBSTACLES_NUMBER) {
            spawnRandomObstacle();
        }
    }

    auto book = _world->body(ObjectNameTag("book"));
    if(book != nullptr) {
        if( (zPos - book->position().z() > SubmemConsts::LENGTH_OF_RAILWAY_SECTION*SubmemConsts::OBJ_SCALE.z()) && _isBookOnMap ||
            !_isBookOnMap && SoundController::getStatus(SoundTag("dialog")) != sf::Sound::Status::Playing) {
            spawnBook();
            _isBookOnMap = true;
        }
    }
}

std::shared_ptr<Group> Level::initCoinPack(int pack) {

    ObjectNameTag packName = ObjectNameTag("coin_pack_" + std::to_string(pack));
    if(_coinPacks.contains(pack)) {
        _world->forceRemoveGroup(packName);
    }
    auto coinPack = std::make_shared<Group>(packName);

    auto player = _world->body(ObjectNameTag("Player_hitbox"));
    double zPos = 0;
    if(player != nullptr) {
        zPos = player->position().z();
    }
    double zShift = zPos + (SubmemConsts::RAILWAYS_NUMBER-1)*SubmemConsts::OBJ_SCALE.z()*SubmemConsts::LENGTH_OF_RAILWAY_SECTION*(0.5 + 0.5*static_cast<double>(rand())/RAND_MAX);

    for(int j = 0; j < SubmemConsts::ONE_PACK_COINS_NUMBER; j++) {
        ObjectNameTag coinName = ObjectNameTag("coin_" + std::to_string(pack) + "_" + std::to_string(j));
        coinPack->load(coinName, SubmemConsts::COIN_OBJ, SubmemConsts::OBJ_SCALE);
        coinPack->body(coinName)->translate(Vec3D((pack%3 - 1)*SubmemConsts::LENGTH_OF_HORIZONTAL_MOVEMENT*SubmemConsts::OBJ_SCALE.x(),
                                                  0,  5*j));
        coinPack->body(coinName)->rotateUp(j*Consts::PI/12.0);
        Timeline::addAnimation<ARotateLeftUpLookAt>(AnimationListTag("coin_rotation"),
                                                    coinPack->body(coinName),
                                                    Vec3D(0, Consts::PI, 0),
                                                    1,
                                                    Animation::LoopOut::Continue,
                                                    Animation::InterpolationType::Linear);
        coinPack->body(coinName)->setCollider(false);
        coinPack->body(coinName)->setTrigger(true);
    }
    coinPack->translateToPoint(Vec3D(0, 0, zShift));
    _world->add(coinPack);
    _coinPacks[pack] = coinPack;
    return coinPack;
}

void Level::playerCollisionCallBack(RigidBody::CollisionInfo collisionInfo) {
    auto player = _world->body(ObjectNameTag("Player_hitbox"));

    auto tag = collisionInfo.obj->name();
    auto body = collisionInfo.obj;

    if(tag.contains("coin")) {
        _coins++;
        body->setTrigger(false);
        auto coin_take_anim = AnimationListTag("coin_take_anim");
        Timeline::addAnimation<AScale>(coin_take_anim, body, Vec3D(0, 0, 0), 0.1,
                                       Animation::LoopOut::None,
                                       Animation::InterpolationType::Cos);
        SoundController::play(SoundTag("coin"));
    }
    if(tag.contains("book")) {
        _books++;
        body->setTrigger(false);
        _isBookOnMap = false;

        auto book_take_anim = AnimationListTag("book_take_anim");
        Timeline::addAnimation<AScale>(book_take_anim, body, Vec3D(0, 0, 0), 0.1,
                                       Animation::LoopOut::None,
                                       Animation::InterpolationType::Cos);
        SoundController::play(SoundTag("book"));
        SoundController::loadAndPlay(SoundTag("dialog"), _dialogs[(_books-1)%_dialogs.size()]);
    }
    if(tag.contains("train") || tag.contains("block") || tag.contains("wall")) {
        if(collisionInfo.point.normal.z() > 0.7) {
            SoundController::play(SoundTag("death"));
            free();
        }
    }

}

void Level::initDialogs(const std::string& dirName) {
    for (const auto &entry: std::filesystem::recursive_directory_iterator(dirName)) {
        if (!std::filesystem::is_directory(entry)) {
            std::string name = entry.path().string();
            std::string extension = name.substr(name.find_last_of('.') + 1);
            if(extension == "ogg") {
                _dialogs.push_back(entry.path().string());
            }
        }
    }
    auto rng = std::default_random_engine {};
    std::time_t t;
    rng.seed(std::time(&t));
    std::shuffle(std::begin(_dialogs), std::end(_dialogs), rng);
}
