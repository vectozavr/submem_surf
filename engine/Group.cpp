//
// Created by Иван Ильин on 20.09.2022.
//

#include "utils/ResourceManager.h"
#include "utils/Log.h"
#include "Group.h"

#include <utility>

std::shared_ptr<RigidBody> Group::add(std::shared_ptr<RigidBody> body) {
    if(_objects.contains(body->name())) {
        Log::log("Group::add(): cannot add body '" + body->name().str() + "': already inserted");
        return body;
    }

    _objects.emplace(body->name(), body);
    if(_addCallBack != nullptr) {
        _addCallBack(body);
    }
    attach(body);
    body->setCollision(_hasCollision);
    body->setCollider(_isCollider);
    return _objects[body->name()];
}

std::shared_ptr<RigidBody> Group::body(const ObjectNameTag &tag) {
    if (_objects.count(tag) == 0) {
        Log::log("Group::body(): there is no body with name '" + tag.str() + "'");
        return nullptr;
    }

    return _objects.find(tag)->second;
}

void Group::remove(const ObjectNameTag &tag) {
    if(_objects.contains(tag)){
        unattach(tag);
        _objects.erase(tag);
        if(_removeCallBack != nullptr) {
            _removeCallBack(tag);
        }
    } else {
        Log::log("Group::remove(): cannot remove body '" + tag.str() + "': body does not exist.");
    }
}

std::shared_ptr<RigidBody> Group::load(const ObjectNameTag &tag, const std::string &filename, const Vec3D &scale) {
    if(_objects.contains(tag)) {
        Log::log("Group::load(): cannot load body '" + tag.str() + "': already inserted");
        return _objects[tag];
    }

    _objects.emplace(tag, std::make_shared<RigidBody>(tag, filename, scale));
    if(_addCallBack != nullptr) {
        _addCallBack(_objects[tag]);
    }
    attach(_objects[tag]);
    _objects[tag]->setCollision(_hasCollision);
    _objects[tag]->setCollider(_isCollider);
    return _objects[tag];
}

void Group::loadGroup(const std::string &filename, const Vec3D &scale) {
    auto objs = ResourceManager::loadObjects(filename, name());
    for (auto &i : objs) {
        std::shared_ptr<RigidBody> obj = std::make_shared<RigidBody>(*i, false);
        add(obj);
        obj->scale(scale);
        obj->setCollision(_hasCollision);
        obj->setCollider(_isCollider);
    }
}

void Group::setCollision(bool c) {
    _hasCollision = c;

    for (auto &[nameTag, obj] : _objects) {
        obj->setCollision(c);
    }
}

void Group::setCollider(bool c) {
    _isCollider = c;

    for (auto &[nameTag, obj] : _objects) {
        obj->setCollider(c);
    }
}

int Group::setCallBacks(std::function<void(ObjectNameTag)> rm, std::function<void(std::shared_ptr<RigidBody>)> add) {
    if(_removeCallBack != nullptr || _addCallBack != nullptr) {
        return 1;
    }
    _removeCallBack = std::move(rm);
    _addCallBack = std::move(add);
    return 0;
}
