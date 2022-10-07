// This file contains the implementation of the pickup item entity.

#include <components/rendercomponent.h>
#include <components/triggercomponent.h>
#include <components/armaturecomponent.h>
#include "pickup.h"
#include "jamgame.h"

namespace Core {
    Pickup::Pickup(std::string_view& str){
        Entity::SetParameters(str);

        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        data->FromString(str);
    }

    void Pickup::UpdateParameters() {
        if (!isloaded) return;
        rendercomponent->UpdateLocation(location);
        rendercomponent->UpdateRotation(rotation);
    }
    
    void Pickup::SetParameters() {
        if (!isloaded) return;
        UpdateParameters();
        //triggercomponent->UpdateLocation(location);
        //triggercomponent->UpdateRotation(rotation);
    }

    void Pickup::Load(){
        auto data = (Data*) serialized_data;

        name_t pickup_model = 0;
        pickup_model = data->pickup_type == 0 ? UID("items/pickups_lifeparticle") : pickup_model;
        pickup_model = data->pickup_type == 1 ? UID("items/pickups_rifle") : pickup_model;
        pickup_model = data->pickup_type == 2 ? UID("items/pickups_stapler") : pickup_model;

        rendercomponent = PoolProxy<RenderComponent>::New();
        rendercomponent->SetModel(pickup_model);
        rendercomponent->SetCellParams(cell->HasInteriorLighting());
        rendercomponent->SetPose(nullptr);
        
        armaturecomponent = PoolProxy<ArmatureComponent>::New();
        armaturecomponent->SetModel(pickup_model);
        armaturecomponent->PlayAnimation(UID("PickupSpin"), -1, 1.0f, 1.0f);

        triggercomponent = PoolProxy<TriggerComponent>::New();
        triggercomponent->SetLocation(location);
        triggercomponent->SetRotation(rotation);
        triggercomponent->SetParent(this);
        triggercomponent->SetModel(UID("big_barrel"));
        triggercomponent->SetActivationCallback([](TriggerComponent* comp){ Message msg { .type = Message::ACTIVATE }; comp->GetParent()->MessageHandler(msg); });
        triggercomponent->SetFilterCallback([](TriggerComponent* trig, PhysicsComponent* comp){ return comp && comp->GetParent() && comp->GetParent()->GetName() == UID("player"); });

        delete serialized_data;
        serialized_data = nullptr;

        rendercomponent->Init();
        armaturecomponent->Init();
        triggercomponent->Init();
        
        rendercomponent->SetPose(armaturecomponent->GetPosePtr());
        
        isloaded = true;

        UpdateParameters();
    }

    void Pickup::Unload() {
        isloaded = false;

        Serialize();

        rendercomponent->Uninit();
        armaturecomponent->Uninit();
        triggercomponent->Uninit();

        PoolProxy<RenderComponent>::Delete(rendercomponent);
        rendercomponent = nullptr;
        PoolProxy<ArmatureComponent>::Delete(armaturecomponent);
        armaturecomponent = nullptr;
        PoolProxy<TriggerComponent>::Delete(triggercomponent);
        triggercomponent = nullptr;
    }

    void Pickup::Serialize() {
        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        if (rendercomponent->GetModel() == UID("items/pickups_lifeparticle")) data->pickup_type = 0;
        if (rendercomponent->GetModel() == UID("items/pickups_rifle")) data->pickup_type = 1;
        if (rendercomponent->GetModel() == UID("items/pickups_stapler")) data->pickup_type = 2;
    }

    void Pickup::MessageHandler(Message& msg){
        if (msg.type == Message::ACTIVATE) {
            PlayerPickedUpPickup(rendercomponent->GetModel());
            Yeet(); // this makes it commit suicide
        }
    }
}
