//
// Created by Иван Ильин on 13.01.2021.
//

#include <sstream>
#include <cmath>

#include "World.h"
#include "utils/Log.h"
#include "math/Plane.h"
#include "utils/ResourceManager.h"

using namespace std;

std::shared_ptr<RigidBody> World::add(std::shared_ptr<RigidBody> body) {
    if(_objects.contains(body->name())) {
        Log::log("World::add(): cannot add body '" + body->name().str() + "': already inserted");
        return body;
    }
    _objects.emplace(body->name(), body);
    Log::log("World::add(): inserted body '" + body->name().str() + "' with " +
             std::to_string(_objects[body->name()]->triangles().size()) + " tris.");
    return _objects[body->name()];
}

std::shared_ptr<RigidBody> World::load(const ObjectNameTag &tag, const string &filename, const Vec3D &scale) {
    if(_objects.contains(tag)) {
        Log::log("World::load(): cannot load body '" + tag.str() + "': already inserted");
        return _objects[tag];
    }

    _objects.emplace(tag, std::make_shared<RigidBody>(tag, filename, scale));
    Log::log("World::load(): inserted body from " + filename + " with title '" + tag.str() + "' with " +
             std::to_string(_objects[tag]->triangles().size()) + " tris.");
    return _objects[tag];
}

void World::loadMap(const std::string &filename, const Vec3D &scale) {
    auto objs = ResourceManager::loadObjects(filename, ObjectNameTag(filename));
    for (auto &i : objs) {
        std::shared_ptr<RigidBody> obj = std::make_shared<RigidBody>(*i, false);
        add(obj);
        obj->scale(scale);
    }
}

void World::remove(const ObjectNameTag &tag) {
    _objectsToRemove.push_back(tag);
}

void World::forceRemove(const ObjectNameTag &tag) {
    if (_objects.erase(tag) > 0) {
        Log::log("World::remove(): removed body '" + tag.str() + "'");
    } else {
        Log::log("World::remove(): cannot remove body '" + tag.str() + "': body does not exist.");
    }
}

std::shared_ptr<RigidBody> World::body(const ObjectNameTag &tag) {
    if (_objects.count(tag) == 0) {
        Log::log("World::body(): there is no body with name '" + tag.str() + "'");
        return nullptr;
    }

    return _objects.find(tag)->second;
}

void World::add(std::shared_ptr<Group> group) {
    if(group->setCallBacks([this](const ObjectNameTag& tag) { remove(tag); },
                           [this](std::shared_ptr<RigidBody> body) { add(body); })) {
        Log::log("World::add(): cannot add group '" + group->name().str() + "' with set call back functions.");
        return;
    }
    if (_groups.contains(group->name())) {
        if(std::find(_groupsToRemove.begin(), _groupsToRemove.end(), group->name()) != _groupsToRemove.end()) {
            forceRemoveGroup(group->name());
            std::cout << group->name().str() << std::endl;
        } else {
            Log::log("World::add(): cannot add group '" + group->name().str() + "': already inserted");
            return;
        }
    }

    _groups.emplace(group->name(), group);

    Log::log("World::add(): added group '" + group->name().str() + "'");

    // add all objects from group:
    for (auto &it : *group) {
        add(it.second);
    }
}

void World::removeGroup(const ObjectNameTag &tag) {
    _groupsToRemove.push_back(tag);
}

void World::clearObjectsToRemove() {
    for(auto &tag : _groupsToRemove) {
        forceRemoveGroup(tag);
    }
    _groupsToRemove.clear();

    for(auto &tag : _objectsToRemove) {
        forceRemove(tag);
    }
    _objectsToRemove.clear();
}

void World::forceRemoveGroup(const ObjectNameTag &tag) {
    if (_groups.count(tag)) {
        // remove all objects from group:
        for (auto &it : *_groups[tag]) {
            forceRemove(it.first);
        }
        _groups.erase(tag);

        Log::log("World::removeGroup(): removed group '" + tag.str() + "'");
    } else {
        Log::log("World::removeGroup(): cannot remove group '" + tag.str() + "': group does not exist.");
    }
}

std::shared_ptr<Group> World::group(const ObjectNameTag &tag) {
    if (_groups.count(tag) == 0) {
        Log::log("World::group(): there is no group with name '" + tag.str() + "'");
        return nullptr;
    }

    return _groups.find(tag)->second;
}

void World::update() {
    // update physics for all objects
    for (auto &[nameTag, obj] : _objects) {
        obj->updatePhysicsState();
        checkCollision(nameTag);
    }

    // update physics for groups
    for (auto &[nameTag, obj] : _groups) {
        obj->updatePhysicsState();
    }

    /*
     * You cannot delete groups instantly (because group)
     * contains several objects. While checkCollision(nameTag) you can delete group
     * with all objects inside, but you continue to iterate through all _objects in checkCollision(nameTag).
     * So you can meet situation when you already delete object but try to iterate through it.
     * To avoid this we implicitly delete groups after checking collisions when it is safe.
     */
    clearObjectsToRemove();
}

void World::checkCollision(const ObjectNameTag &tag) {
    if (_objects[tag]->hasCollision()) {

        _objects[tag]->setInCollision(false);

        for (auto it = _objects.begin(); it != _objects.end();) {
            auto obj = it->second;
            ObjectNameTag name = it->first;
            it++;

            if ((name == tag) || !(obj->isCollider() || obj->isTrigger())) {
                continue;
            }

            std::pair<bool, Simplex> gjk = _objects[tag]->checkGJKCollision(obj);
            if (gjk.first) {
                if (obj->isCollider()) {
                    CollisionPoint epa = _objects[tag]->EPA(gjk.second, obj);
                    _objects[tag]->solveCollision(epa);
                    _objects[tag]->solveCollision(RigidBody::CollisionInfo{epa, obj});
                    if (_objects[tag]->collisionCallBack() != nullptr) {
                        _objects[tag]->collisionCallBack()(RigidBody::CollisionInfo{epa, obj});
                    }
                } else if (_objects[tag]->collisionCallBack() != nullptr) {
                    _objects[tag]->collisionCallBack()(RigidBody::CollisionInfo{{Vec3D(0), 0}, obj});
                }
            }

        }
    }
}

IntersectionInformation World::rayCast(const Vec3D &from, const Vec3D &to, const std::string &skipTags) {

    // make vector of tags, that we are going to escape
    vector<std::string> tagsToSkip;
    stringstream s(skipTags);
    std::string t;
    while (s >> t) {
        tagsToSkip.push_back(t);
    }

    bool intersected = false;
    Vec3D point{};
    Triangle triangle;
    std::string bodyName;
    double minDistance = Consts::RAY_CAST_MAX_DISTANCE;
    std::shared_ptr<RigidBody> intersectedBody = nullptr;

    for (auto&[name, body]  : _objects) {

        bool escapeThisBody = false;
        for (auto &escapeTag : tagsToSkip) {
            if (name.contains(escapeTag)) {
                escapeThisBody = true;
                break;
            }
        }
        if (escapeThisBody) {
            continue;
        }

        Matrix4x4 model = body->model();
        // It is computationally more efficient not to transform all object's triangles from model to global
        // coordinate system, but translate 'from' and 'to' vectors inside once and check triangles without performing
        // many matrix multiplication.
        Matrix4x4 invModel = body->invModel();

        Vec3D v = (to - from).normalized();
        Vec3D v_model = invModel*v;
        Vec3D from_model = invModel*(from - body->position());
        Vec3D to_model = invModel*(to - body->position());


        for (auto &tri : body->triangles()) {

            if(tri.norm().dot(v_model) > 0) {
                continue;
            }

            auto intersection = Plane(tri).intersection(from_model, to_model);

            if (intersection.second > 0 && tri.isPointInside(intersection.first)) {

                // When you change to model coordinate system you also will get distance scaled by invModel.
                // Due-to this effect if you scale some object in x times you will get distance in x times smaller.
                // That's why we need to perform distance calculation in the global coordinate system where metric
                // is the same for all objects.
                Triangle globalTriangle(model * tri[0], model * tri[1], model * tri[2], tri.color());
                auto globalIntersection = Plane(globalTriangle).intersection(from, to);
                double globalDistance = (globalIntersection.first - from).abs();

                if(globalDistance < minDistance) {
                    minDistance = globalDistance;
                    point = globalIntersection.first;
                    triangle = globalTriangle;
                    bodyName = name.str();
                    intersected = true;
                    intersectedBody = body;
                    //Triangle triangleRED = Triangle(model * tri[0], model * tri[1], model * tri[2], sf::Color(255, 0, 0));
                    //addBody(std::make_shared<RigidBody>(Mesh(ObjectNameTag("Test" + std::to_string(rand())), std::vector<Triangle>({triangleRED}))));
                }
            }
        }
    }

    return IntersectionInformation{point, sqrt(minDistance), triangle, ObjectNameTag(bodyName), intersectedBody, intersected};
}
