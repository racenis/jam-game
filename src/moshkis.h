// This file contains the declaration of the monster creature.

#ifndef MOSHKIS_H
#define MOSHKIS_H

#include <framework/core.h>
#include <framework/serialization.h>
#include <framework/entity.h>

namespace tram {
    class RenderComponent;
    class PhysicsComponent;
    class ArmatureComponent;
    class MoshkisComponent;
    
    class Moshkis : public Entity {
    public:
        Moshkis(const SharedEntityData& shared_data, const SerializedFieldArray& field_array);
        ~Moshkis();

        void UpdateParameters();
        void SetParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);

        static void Register();
    protected:
        RenderComponent* rendercomponent;
        PhysicsComponent* physicscomponent;
        ArmatureComponent* armaturecomponent;
        MoshkisComponent* moshkiscomponent;
        
        uint64_t health;
    };
}

#endif //MOSHKIS_H