//
// Created by Иван Ильин on 20.09.2022.
//

#ifndef SUBMEM_SURF_GROUP_H
#define SUBMEM_SURF_GROUP_H

#include <cstdarg>
#include "physics/RigidBody.h"

class Group : public Object {
private:
    std::map<ObjectNameTag, std::shared_ptr<RigidBody>> _objects;

    std::function<void(ObjectNameTag)> _removeCallBack;
    std::function<void(std::shared_ptr<RigidBody>)> _addCallBack;

    bool _hasCollision = false;
    bool _isCollider = true;
public:

    explicit Group(const ObjectNameTag &tag) : Object(tag) {}

    std::shared_ptr<RigidBody> add(std::shared_ptr<RigidBody> mesh);
    [[nodiscard]] std::shared_ptr<RigidBody> body(const ObjectNameTag &tag);
    void remove(const ObjectNameTag &tag);
    std::shared_ptr<RigidBody> load(const ObjectNameTag &tag, const std::string &filename, const Vec3D &scale = Vec3D{1, 1, 1});
    void loadGroup(const std::string &filename, const Vec3D &scale = Vec3D{1, 1, 1});

    void setCollision(bool c);
    void setCollider(bool c);

    [[nodiscard]] std::map<ObjectNameTag, std::shared_ptr<RigidBody>>::iterator begin() { return _objects.begin(); }
    [[nodiscard]] std::map<ObjectNameTag, std::shared_ptr<RigidBody>>::iterator end() { return _objects.end(); }

    int setCallBacks(std::function<void(ObjectNameTag)> rm, std::function<void(std::shared_ptr<RigidBody>)> add);
};


#endif //SUBMEM_SURF_GROUP_H
