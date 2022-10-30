//
// Created by Иван Ильин on 14.03.2021.
//

#include "Player.h"

#include <utility>
#include "../engine/io/Screen.h"
#include "../engine/utils/Log.h"
#include "../engine/animation/Animations.h"

Player::Player() : Group(ObjectNameTag(SubmemConsts::PLAYER_NAME)) {
    loadGroup(SubmemConsts::MAN_OBJ);
    setCollider(false);
    translate(Vec3D(0, -1.2, 0));

    auto lHand = attached(ObjectNameTag("Player_temp_obj_11"));
    auto rHand = attached(ObjectNameTag("Player_temp_obj_12"));

    lHand->rotateLeft(Consts::PI/2);
    rHand->rotateLeft(-Consts::PI/2);

    auto lFoot = attached(ObjectNameTag("Player_temp_obj_1"));
    auto rFoot = attached(ObjectNameTag("Player_temp_obj_10"));

    lFoot->rotateLeft(-Consts::PI/4);
    rFoot->rotateLeft(Consts::PI/4);
}

void Player::addRunAnimation() {
    auto lHand = attached(ObjectNameTag("Player_temp_obj_11"));
    auto rHand = attached(ObjectNameTag("Player_temp_obj_12"));

    auto lFoot = attached(ObjectNameTag("Player_temp_obj_1"));
    auto rFoot = attached(ObjectNameTag("Player_temp_obj_10"));

    auto lHandAnim = AnimationListTag("player_lHand_animation");
    auto rHandAnim = AnimationListTag("player_rHand_animation");

    if(!Timeline::isInAnimList(lHandAnim) && !Timeline::isInAnimList(rHandAnim)) {
        // left hand
        Timeline::addAnimation<ARotateLeft>(lHandAnim, lHand, -Consts::PI, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<ARotateLeft>(lHandAnim, lFoot, Consts::PI/2, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<AWait>(lHandAnim, 0);
        Timeline::addAnimation<ARotateLeft>(lHandAnim, lHand, Consts::PI, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<ARotateLeft>(lHandAnim, lFoot, -Consts::PI/2, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<AWait>(lHandAnim, 0);
        // right hand
        Timeline::addAnimation<ARotateLeft>(rHandAnim, rHand, Consts::PI, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<ARotateLeft>(rHandAnim, rFoot, -Consts::PI/2, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<AWait>(rHandAnim, 0);
        Timeline::addAnimation<ARotateLeft>(rHandAnim, rHand, -Consts::PI, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<ARotateLeft>(rHandAnim, rFoot, Consts::PI/2, 0.2, Animation::LoopOut::None,
                                            Animation::InterpolationType::Cos);
        Timeline::addAnimation<AWait>(rHandAnim, 0);
    }
}

void Player::addDeathAnimation() {

}
