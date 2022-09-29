#include <components/rendercomponent.h>
#include <components/triggercomponent.h>
#include <components/armaturecomponent.h>
#include "pickup.h"

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
        triggercomponent->SetModel(UID("creatures/moshkis"));
        triggercomponent->SetCollisionMask(Physics::COLL_PLAYER);
        triggercomponent->SetActivationCallback([](TriggerComponent* comp){ std::cout << "active" << std::endl; });

        delete serialized_data;
        serialized_data = nullptr;

        rendercomponent->Init();
        armaturecomponent->Init();
        triggercomponent->Init();
        
        rendercomponent->SetPose(armaturecomponent->GetPosePtr());
        
        isloaded = true;
        
        armaturecomponent->PlayAnimation(UID("PickupSpin"), -1, 1.0f, 1.0f);

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
        return;
    }
}
