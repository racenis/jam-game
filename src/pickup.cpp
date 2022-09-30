#include <components/rendercomponent.h>
#include <components/triggercomponent.h>
#include <components/armaturecomponent.h>
#include "pickup.h"
#include "jamgame.h"

namespace Core {
    template <> Pool<PickupWorkaroundComponent> PoolProxy<PickupWorkaroundComponent>::pool("PickupWorkaroundComponent pool" , 50);
    
    void PickupWorkaroundComponent::Start() {
        if (!is_ready) return;
        assert(armcomp);
        armcomp->PlayAnimation(UID("PickupSpin"), -1, 1.0f, 1.0f);
    }
    
    void PickupWorkaroundComponent::MakeWorkaround(name_t model, ArmatureComponent* armcomp) {
        this->armcomp = armcomp;
        this->model.SetResource(Render::Model::Find(model));
    }
    
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

        rendercomponent = PoolProxy<RenderComponent>::New();
        if (data->pickup_type == 0) rendercomponent->SetModel(UID("items/pickups_lifeparticle"));
        if (data->pickup_type == 1) rendercomponent->SetModel(UID("items/pickups_rifle"));
        if (data->pickup_type == 2) rendercomponent->SetModel(UID("items/pickups_stapler"));
        rendercomponent->SetPose(nullptr);
        
        armaturecomponent = PoolProxy<ArmatureComponent>::New();
        if (data->pickup_type == 0) armaturecomponent->SetModel(UID("items/pickups_lifeparticle"));
        if (data->pickup_type == 1) armaturecomponent->SetModel(UID("items/pickups_rifle"));
        if (data->pickup_type == 2) armaturecomponent->SetModel(UID("items/pickups_stapler"));

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
        
        workaroundcomponent = PoolProxy<PickupWorkaroundComponent>::New();
        if (data->pickup_type == 0) workaroundcomponent->MakeWorkaround(UID("items/pickups_lifeparticle"), armaturecomponent);
        if (data->pickup_type == 1) workaroundcomponent->MakeWorkaround(UID("items/pickups_rifle"), armaturecomponent);
        if (data->pickup_type == 2) workaroundcomponent->MakeWorkaround(UID("items/pickups_stapler"), armaturecomponent);
        workaroundcomponent->Init();
        
        isloaded = true;

        UpdateParameters();
    }

    void Pickup::Unload() {
        isloaded = false;

        Serialize();

        rendercomponent->Uninit();
        armaturecomponent->Uninit();
        triggercomponent->Uninit();
        workaroundcomponent->Uninit();

        PoolProxy<RenderComponent>::Delete(rendercomponent);
        rendercomponent = nullptr;
        PoolProxy<ArmatureComponent>::Delete(armaturecomponent);
        armaturecomponent = nullptr;
        PoolProxy<TriggerComponent>::Delete(triggercomponent);
        triggercomponent = nullptr;
        PoolProxy<PickupWorkaroundComponent>::Delete(workaroundcomponent);
        workaroundcomponent = nullptr;

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
