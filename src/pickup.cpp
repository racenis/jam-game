// This file contains the implementation of the pickup item entity.

#include <components/rendercomponent.h>
#include <components/triggercomponent.h>
#include <components/armaturecomponent.h>
#include <framework/worldcell.h>
#include <framework/message.h>
#include "pickup.h"
#include "jamgame.h"

namespace tram {

enum {
    FIELD_PICKUP_TYPE,
    FIELD_PICKUP_AMMOUNT
};

static const uint32_t fields[2] = {
    TYPE_UINT64,
    TYPE_UINT64
}; 

void Pickup::Register() {
    Entity::RegisterType(
        "pickup", 
        [](const SharedEntityData& a, const SerializedFieldArray& b) -> Entity* { return new Pickup(a, b); },
        [](Entity* a) { delete a; },
        fields,
        2
    );
}

Pickup::Pickup(const SharedEntityData& shared_data, const SerializedFieldArray& field_array) : Entity(shared_data) {
    pickup_type = field_array[FIELD_PICKUP_TYPE];
    pickup_ammount = field_array[FIELD_PICKUP_AMMOUNT];
}

void Pickup::UpdateParameters() {
    if (!is_loaded) return;
    rendercomponent->SetLocation(location);
    rendercomponent->SetRotation(rotation);
}

void Pickup::SetParameters() {
    if (!is_loaded) return;
    UpdateParameters();
}

void Pickup::Load(){
    name_t pickup_model;
    pickup_model = pickup_type == 0 ? UID("items/pickups_lifeparticle") : pickup_model;
    pickup_model = pickup_type == 1 ? UID("items/pickups_rifle") : pickup_model;
    pickup_model = pickup_type == 2 ? UID("items/pickups_stapler") : pickup_model;

    rendercomponent = PoolProxy<RenderComponent>::New();
    rendercomponent->SetModel(pickup_model);
    rendercomponent->SetWorldParameters(cell->HasInteriorLighting());
    
    armaturecomponent = PoolProxy<ArmatureComponent>::New();
    armaturecomponent->SetModel(pickup_model);
    armaturecomponent->PlayAnimation(UID("PickupSpin"), -1, 1.0f, 1.0f);

    triggercomponent = PoolProxy<TriggerComponent>::New();
    triggercomponent->SetLocation(location);
    triggercomponent->SetRotation(rotation);
    triggercomponent->SetParent(this);
    triggercomponent->SetModel(UID("big_barrel"));
    triggercomponent->SetActivationCallback([](TriggerComponent* comp, Physics::Collision){ Message msg { .type = Message::ACTIVATE }; comp->GetParent()->MessageHandler(msg); });
    triggercomponent->SetFilterCallback([](TriggerComponent* trig, PhysicsComponent* comp){ return comp && comp->GetParent() && comp->GetParent()->GetName() == UID("player"); });

    rendercomponent->Init();
    armaturecomponent->Init();
    triggercomponent->Init();
    
    //rendercomponent->SetPose(armaturecomponent->GetPosePtr());
    rendercomponent->SetArmature(armaturecomponent);
    
    is_loaded = true;

    UpdateParameters();
}

void Pickup::Unload() {
    is_loaded = false;

    Serialize();

    PoolProxy<RenderComponent>::Delete(rendercomponent);
    rendercomponent = nullptr;
    PoolProxy<ArmatureComponent>::Delete(armaturecomponent);
    armaturecomponent = nullptr;
    PoolProxy<TriggerComponent>::Delete(triggercomponent);
    triggercomponent = nullptr;
}

void Pickup::Serialize() {
    if (rendercomponent->GetModel() == UID("items/pickups_lifeparticle")) pickup_type = 0;
    if (rendercomponent->GetModel() == UID("items/pickups_rifle")) pickup_type = 1;
    if (rendercomponent->GetModel() == UID("items/pickups_stapler")) pickup_type = 2;
}

void Pickup::MessageHandler(Message& msg){
    if (msg.type == Message::ACTIVATE) {
        PlayerPickedUpPickup(rendercomponent->GetModel());
        Unload();
        delete this;
    }
}

}
