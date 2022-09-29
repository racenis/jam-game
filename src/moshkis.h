#ifndef MOSHKIS_H
#define MOSHKIS_H

#include <core.h>
#include <serializeddata.h>

namespace Core {
    class RenderComponent;
    class PhysicsComponent;
    class ArmatureComponent;
    class MoshkisComponent;
    
    class Moshkis : public Entity {
    public:
        Moshkis(std::string_view& str);

        void UpdateParameters();
        void SetParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);

        class Data: public SerializedEntityData {
        public:
            Field<uint64_t> health;

            void ToString(std::string& str) {
                health.ToString(str);
            }

            void FromString(std::string_view& str) {
                health.FromString(str);
            }
            
            name_t GetEditorModel() {
                return UID("creatures/moshkis");
            }
            
            std::vector<FieldInfo> GetEditorFieldInfo() {
                return std::vector<FieldInfo> {
                    { FieldInfo::FIELD_UINT64, "Health", &health }
                };
            }
            
            char const* GetDataName() {
                return "moshkis";
            }
            
            char const* GetEditorName() {
                return "Moshkis";
            }
        };
    protected:
        RenderComponent* rendercomponent;
        PhysicsComponent* physicscomponent;
        ArmatureComponent* armaturecomponent;
        MoshkisComponent* moshkiscomponent;
    };
}

#endif //MOSHKIS_H