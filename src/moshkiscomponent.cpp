#include <components/physicscomponent.h>
#include <components/armaturecomponent.h>
#include <entities/player.h>
#include "moshkiscomponent.h"
#include "jamgame.h"

using namespace Core;

template <> Pool<MoshkisComponent> PoolProxy<MoshkisComponent>::pool("moshkis component pool", 100);

void MoshkisComponent::Init() {
    assert(parent);
    assert(physcomp);
    assert(armcomp);
}

void MoshkisComponent::Uninit() {
    
    
}

void MoshkisComponent::Start() {
    
    
}

void MoshkisComponent::OofMoshkis(uint64_t oof_strength) {
    if (oof_strength > moshkis_health) {
        moshkis_health = 0;
    } else {
        moshkis_health = moshkis_health - oof_strength;
    }
    
    std::cout << "Moshkis oof: " << oof_strength << " New health: " << moshkis_health;
    
    if (moshkis_health == 0) {
        moshkis_state = MOSHKIS_DEAD;
        
        parent->SetPersistent(false);
        
        if (armcomp->IsPlayingAnimation(UID("MoshkisIdle"))) armcomp->StopAnimation(UID("MoshkisIdle"));
        if (armcomp->IsPlayingAnimation(UID("MoshkisWalk"))) armcomp->StopAnimation(UID("MoshkisWalk"));
        if (armcomp->IsPlayingAnimation(UID("MoshkisAttack"))) armcomp->StopAnimation(UID("MoshkisAttack"));
        
        armcomp->PlayAnimation(UID("MoshkisDie"), 1, 1.0f, 1.0f, true, true);
    }
}

void MoshkisComponent::UpdateMoshkis() {
    if (!(physcomp->IsReady() && armcomp->IsReady())) return;
    if (!rotation_disabled) { physcomp->DisableRotation(); rotation_disabled = true; }

    assert(main_player);
    
    glm::vec3 player_location;
    glm::vec3 moshkis_location;
    glm::quat moshkis_rotation;
    main_player->GetLocation(player_location);
    parent->GetLocation(moshkis_location);
    parent->GetRotation(moshkis_rotation);
    
    if (moshkis_state == MOSHKIS_IDLE) {
        if (!armcomp->IsPlayingAnimation(UID("MoshkisIdle"))) armcomp->PlayAnimation(UID("MoshkisIdle"), -1, 1.0f, 1.0f);
    
        if (glm::distance(player_location, moshkis_location) < 10.0f) {
            auto moshkis_to_player = glm::normalize(player_location - moshkis_location);
            auto moshkis_front = glm::normalize(moshkis_rotation * glm::vec3(0.0f, 0.0f, -1.0f));
            
            Render::AddLine(moshkis_location, moshkis_location + moshkis_front, Render::COLOR_BLUE);
            
            
            auto raycast_result = Physics::Raycast(moshkis_location, player_location);
            
            if (glm::dot(moshkis_front, moshkis_to_player) > 0.2f && raycast_result && raycast_result->GetParent() && raycast_result->GetParent()->GetName() == UID("player")) {
                moshkis_state = MOSHKIS_CHARGE;
                
                armcomp->StopAnimation(UID("MoshkisIdle"));
                armcomp->PlayAnimation(UID("MoshkisWalk"), -1, 1.0f, 1.0f);
                physcomp->DisableActivation(); // will actually disable deactivation
            }
        }
    } else if (moshkis_state == MOSHKIS_CHARGE) {
        if (glm::distance(player_location, moshkis_location) > 15.0f) {
            moshkis_state = MOSHKIS_IDLE;
            armcomp->StopAnimation(UID("MoshkisWalk"));
            physcomp->EnableActivation(); // will actually enable deactivation
        } else if (glm::distance(player_location, moshkis_location) < 1.5f) {
            moshkis_state = MOSHKIS_ATTACK;
            attack_frames = 0;
            PlaySoundEffect(SOUND_MOSHKIS_ATTACK, moshkis_location);
            armcomp->StopAnimation(UID("MoshkisWalk"));
            armcomp->PlayAnimation(UID("MoshkisAttack"), 1, 1.0f, 2.0f);
        } else {
            auto moshkis_to_player = glm::normalize((player_location - moshkis_location) * glm::vec3(1.0f, 0.0f, 1.0f));
            auto moshkis_front = glm::normalize(moshkis_rotation * glm::vec3(0.0f, 0.0f, -1.0f));
            auto moshkis_new_front = glm::mix(moshkis_to_player, moshkis_front, 0.01f);
            glm::quat moshkis_new_rotation = glm::quatLookAt(moshkis_new_front, glm::vec3(0.0f, 1.0f, 0.0f));
            physcomp->SetRotation(moshkis_new_rotation);
            
            if (physcomp->GetVelocity() < 4.0f) physcomp->PushLocal(glm::vec3(0.0f, 0.0f, -25.0f));
        }
    } else if (moshkis_state == MOSHKIS_ATTACK) {
        if (!armcomp->IsPlayingAnimation(UID("MoshkisAttack"))) {
            moshkis_state = MOSHKIS_IDLE;
        }
        
        // probably should add a method to check on which frame the animation is on, instead of counting myself
        if (attack_frames == 50) {
            auto moshkis_to_player = glm::normalize(player_location - moshkis_location);
            auto moshkis_front = glm::normalize(moshkis_rotation * glm::vec3(0.0f, 0.0f, -1.0f));
            auto raycast_result = Physics::Raycast(moshkis_location, player_location);
            if (glm::dot(moshkis_front, moshkis_to_player) > 0.2f && raycast_result && raycast_result->GetParent() && raycast_result->GetParent()->GetName() == UID("player")) {
                std::cout << "HIT!" << std::endl;
                PlaySoundEffect(SOUND_MOSHKIS_HIT, Render::CAMERA_POSITION);
                PlayerGotHitInFace(100);
            } else {
                std::cout << "MISS!" << std::endl;
            }
        }
        
        attack_frames++;
    }
}