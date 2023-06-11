// This file contains the declaration of the pickup item entity.

#ifndef PICKUP_H
#define PICKUP_H

#include <framework/core.h>
#include <framework/serialization.h>
#include <framework/entity.h>

#include <render/render.h>

namespace tram {

class RenderComponent;
class ArmatureComponent;
class TriggerComponent;
    
class Pickup : public Entity {
public:
    Pickup(const SharedEntityData& shared_data, const SerializedFieldArray& field_array);

    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    
    static void Register();
protected:
    RenderComponent* rendercomponent;
    ArmatureComponent* armaturecomponent;
    TriggerComponent* triggercomponent;
    
    uint64_t pickup_type;
    uint64_t pickup_ammount;
};

}

#endif //PICKUP_H