// This file contains the implementation of the monster creature.

#include <components/rendercomponent.h>
#include <components/physicscomponent.h>
#include <components/armaturecomponent.h>
#include <framework/worldcell.h>
#include <framework/message.h>
#include "moshkis.h"
#include "moshkiscomponent.h"

namespace tram {

enum {
    FIELD_HEALTH
};

static const uint32_t fields[1] = {
    TYPE_UINT64
}; 

void Moshkis::Register() {
    Entity::RegisterType(
        "moshkis", 
        [](const SharedEntityData& a, const SerializedFieldArray& b) -> Entity* { return new Moshkis(a, b); },
        [](Entity* a) { delete a; },
        fields,
        1
    );
}

Moshkis::Moshkis(const SharedEntityData& shared_data, const SerializedFieldArray& field_array) : Entity(shared_data) {
    health = field_array[FIELD_HEALTH];
}
    

Moshkis::~Moshkis() {
    PoolProxy<RenderComponent>::Delete(rendercomponent);
    rendercomponent = nullptr;
    PoolProxy<ArmatureComponent>::Delete(armaturecomponent);
    armaturecomponent = nullptr;
    PoolProxy<PhysicsComponent>::Delete(physicscomponent);
    physicscomponent = nullptr;
    PoolProxy<MoshkisComponent>::Delete(moshkiscomponent);
    moshkiscomponent = nullptr;
}

void Moshkis::UpdateParameters() {
    if (!is_loaded) return;
    rendercomponent->SetLocation(location);
    rendercomponent->SetRotation(rotation);
    rendercomponent->SetWorldParameters(cell->HasInteriorLighting());
}

void Moshkis::SetParameters() {
    if (!is_loaded) return;
    UpdateParameters();
    //physicscomponent->UpdateLocation(location);
    //physicscomponent->UpdateRotation(rotation);
    physicscomponent->SetLocation(location);
    physicscomponent->SetRotation(rotation);
}

void Moshkis::Load(){
    rendercomponent = PoolProxy<RenderComponent>::New();
    rendercomponent->SetModel(UID("creatures/moshkis"));
    //rendercomponent->SetPose(nullptr);
    
    armaturecomponent = PoolProxy<ArmatureComponent>::New();
    armaturecomponent->SetModel(UID("creatures/moshkis"));

    physicscomponent = PoolProxy<PhysicsComponent>::New();
    physicscomponent->SetParent(this);
    physicscomponent->SetModel(UID("creatures/moshkis"));
    physicscomponent->SetMass(140.0f);
    //physicscomponent->SetDamping(440.0f);
    //physicscomponent->SetStartAsleep();
    
    moshkiscomponent = PoolProxy<MoshkisComponent>::New();
    moshkiscomponent->SetArmatureComponent(armaturecomponent);
    moshkiscomponent->SetPhysicsComponent(physicscomponent);
    moshkiscomponent->SetParent(this);
    moshkiscomponent->SetMoshkisHealth(this->health);

    rendercomponent->Init();
    armaturecomponent->Init();
    physicscomponent->Init();
    
    //rendercomponent->SetPose(armaturecomponent->GetPosePtr());
    rendercomponent->SetArmature(armaturecomponent);
    
    is_loaded = true;

    UpdateParameters();
}

void Moshkis::Unload() {
    is_loaded = false;

    Serialize();

    PoolProxy<RenderComponent>::Delete(rendercomponent);
    rendercomponent = nullptr;
    PoolProxy<ArmatureComponent>::Delete(armaturecomponent);
    armaturecomponent = nullptr;
    PoolProxy<PhysicsComponent>::Delete(physicscomponent);
    physicscomponent = nullptr;
    PoolProxy<MoshkisComponent>::Delete(moshkiscomponent);
    moshkiscomponent = nullptr;
}

void Moshkis::Serialize() {

}

void Moshkis::MessageHandler(Message& msg){
    if (msg.type == Message::ACTIVATE) moshkiscomponent->OofMoshkis((uint64_t)msg.data);
}
    
}
