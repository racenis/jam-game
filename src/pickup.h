// This file contains the declaration of the pickup item entity.

#ifndef PICKUP_H
#define PICKUP_H

#include <core.h>
#include <serializeddata.h>

#include <render/render.h>

namespace Core {
    class RenderComponent;
    class ArmatureComponent;
    class TriggerComponent;
        
    class Pickup : public Entity {
    public:
        Pickup(std::string_view& str);

        void UpdateParameters();
        void SetParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);

        class Data: public SerializedEntityData {
        public:
            Field<uint64_t> pickup_type;
            Field<uint64_t> pickup_ammount;

            void ToString(std::string& str) {
                pickup_type.ToString(str);
                pickup_ammount.ToString(str);
            }

            void FromString(std::string_view& str) {
                pickup_type.FromString(str);
                pickup_ammount.FromString(str);
            }
            
            name_t GetEditorModel() {
                return UID("items/pickups_stapler");
            }
            
            std::vector<FieldInfo> GetEditorFieldInfo() {
                return std::vector<FieldInfo> {
                    { FieldInfo::FIELD_UINT64, "Type", &pickup_type },
                    { FieldInfo::FIELD_UINT64, "Ammount", &pickup_ammount }
                };
            }
            
            char const* GetDataName() {
                return "pickup";
            }
            
            char const* GetEditorName() {
                return "Pickup";
            }
        };
    protected:
        RenderComponent* rendercomponent;
        ArmatureComponent* armaturecomponent;
        TriggerComponent* triggercomponent;
    };
}

#endif //PICKUP_H