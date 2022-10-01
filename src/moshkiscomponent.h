// This file contains the declaration of the monster creature behaviour.

#ifndef MOSHKISCOMPONENT_H
#define MOSHKISCOMPONENT_H

#include <core.h>

namespace Core {
    class PhysicsComponent;
    class ArmatureComponent;
    
    class MoshkisComponent : public EntityComponent {
    public:
        void Init();
        void Uninit(){}
        void Start(){}
        void EventHandler(Event &event){}
        
        void UpdateMoshkis();
        
        void OofMoshkis(uint64_t oof_strength);
        
        void SetMoshkisHealth(uint64_t health) { moshkis_health = health; }
        uint64_t GetMoshkisHealth() { return moshkis_health; }
        
        void SetParent(Entity* parent) { this->parent = parent; }
        void SetPhysicsComponent(PhysicsComponent* physcomp) { this->physcomp = physcomp; }
        void SetArmatureComponent(ArmatureComponent* armcomp) { this->armcomp = armcomp; }
    protected:
        Entity* parent = nullptr;
        PhysicsComponent* physcomp = nullptr;
        ArmatureComponent* armcomp = nullptr;
        
        uint64_t moshkis_health;
        bool rotation_disabled = false; // a hack, because the physicscomponent is kinda dumb
        
        enum {
            MOSHKIS_IDLE,
            MOSHKIS_CHARGE,
            MOSHKIS_ATTACK,
            MOSHKIS_DEAD
        } moshkis_state = MOSHKIS_IDLE;
        uint64_t attack_frames = 0;
    };
}

#endif //MOSHKISCOMPONENT_H