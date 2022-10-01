// This file contains the implementation of the monster creature.

#include <components/rendercomponent.h>
#include <components/physicscomponent.h>
#include <components/armaturecomponent.h>
#include "moshkis.h"
#include "moshkiscomponent.h"

namespace Core {
    Moshkis::Moshkis(std::string_view& str){
        Entity::SetParameters(str);

        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        data->FromString(str);
    }

    void Moshkis::UpdateParameters() {
        if (!isloaded) return;
        rendercomponent->UpdateLocation(location);
        rendercomponent->UpdateRotation(rotation);
        rendercomponent->SetCellParams(cell->HasInteriorLighting());
    }
    
    void Moshkis::SetParameters() {
        if (!isloaded) return;
        UpdateParameters();
        physicscomponent->UpdateLocation(location);
        physicscomponent->UpdateRotation(rotation);
    }

    void Moshkis::Load(){
        auto data = (Data*) serialized_data;

        rendercomponent = PoolProxy<RenderComponent>::New();
        rendercomponent->SetModel(UID("creatures/moshkis"));
        rendercomponent->SetPose(nullptr);
        
        armaturecomponent = PoolProxy<ArmatureComponent>::New();
        armaturecomponent->SetModel(UID("creatures/moshkis"));

        physicscomponent = PoolProxy<PhysicsComponent>::New();
        physicscomponent->SetParent(this);
        physicscomponent->SetModel(UID("creatures/moshkis"));
        physicscomponent->SetMass(140.0f);
        physicscomponent->SetDamping(440.0f);
        physicscomponent->SetStartAsleep();
        
        moshkiscomponent = PoolProxy<MoshkisComponent>::New();
        moshkiscomponent->SetArmatureComponent(armaturecomponent);
        moshkiscomponent->SetPhysicsComponent(physicscomponent);
        moshkiscomponent->SetParent(this);
        moshkiscomponent->SetMoshkisHealth(data->health);

        delete serialized_data;
        serialized_data = nullptr;

        rendercomponent->Init();
        armaturecomponent->Init();
        physicscomponent->Init();
        
        rendercomponent->SetPose(armaturecomponent->GetPosePtr());
        
        isloaded = true;

        UpdateParameters();
    }

    void Moshkis::Unload() {
        isloaded = false;

        Serialize();

        rendercomponent->Uninit();
        armaturecomponent->Uninit();
        physicscomponent->Uninit();
        moshkiscomponent->Uninit();

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
        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        data->health = moshkiscomponent->GetMoshkisHealth();
    }

    void Moshkis::MessageHandler(Message& msg){
        if (msg.type == Message::ACTIVATE) moshkiscomponent->OofMoshkis((uint64_t)msg.data);
    }
}
