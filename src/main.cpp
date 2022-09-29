#include <iostream>

#include <core.h>
#include <async.h>
#include <ui.h>
#include <physics/physics.h>
#include <audio.h>

#include <gui.h>

#include <glfw3.h>

#include <entities/crate.h>
#include <entities/staticworldobject.h>
#include <entities/player.h>
#include "moshkis.h"

#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>
#include <components/lightcomponent.h>

#include <components/spritecomponent.h>
#include <components/particlecomponent.h>
#include <components/audiocomponent.h>

#include <components/controllercomponent.h>
#include <components/physicscomponent.h>

#include "moshkiscomponent.h"

#include "jamgame.h"

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;

Player* main_player = nullptr;

class PlayerStuff {
public:
    PlayerStuff (Player* player) {
        this->player = player;
        
        viewmodel = PoolProxy<RenderComponent>::New();
        viewmodel->SetModel(UID("items/viewmodel_stapler"));
        viewmodel->SetPose(poseList.begin().ptr);
        viewmodel->Init();
        viewmodel->UpdateLocation(glm::vec3(37.0f, 1.0f, -22.0f));
        viewmodel->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

        viewmodel_animator = PoolProxy<ArmatureComponent>::New();
        viewmodel_animator->SetModel(UID("items/viewmodel_stapler"));
        viewmodel_animator->Init();

        viewmodel->SetPose(viewmodel_animator->GetPosePtr());
    }
    
    void Update() {
        if (!viewmodel_animator->IsReady()) return;
        
        viewmodel->UpdateLocation(Render::CAMERA_POSITION - glm::vec3(0.0f, 0.35f, 0.0f));
        viewmodel->UpdateRotation(Render::CAMERA_ROTATION);
        
        if (player_state == PLAYER_HAMMER_IDLING && !viewmodel_animator->IsPlayingAnimation(UID("AamursIdle"))) {
            viewmodel_animator->PlayAnimation(UID("AamursIdle"), -1, 1.0f, 1.0f);
        }
        
        if (player_state == PLAYER_RIFLE_IDLING && !viewmodel_animator->IsPlayingAnimation(UID("RifleIdle"))) {
            viewmodel_animator->PlayAnimation(UID("RifleIdle"), -1, 1.0f, 1.0f);
        }
        
        if (player_state == PLAYER_STAPLER_IDLING && !viewmodel_animator->IsPlayingAnimation(UID("StaplerIdle"))) {
            viewmodel_animator->PlayAnimation(UID("StaplerIdle"), -1, 1.0f, 1.0f);
        }
        
        // it would probably be best if these were driven by events or something
        if (player_state == PLAYER_HAMMER_FIRING && !viewmodel_animator->IsPlayingAnimation(UID("AamursFire"))) {
            player_state = PLAYER_HAMMER_IDLING;
        }
        
        if (player_state == PLAYER_RIFLE_FIRING && !viewmodel_animator->IsPlayingAnimation(UID("RifleFire"))) {
            player_state = PLAYER_RIFLE_IDLING;
        }
        
        if (player_state == PLAYER_STAPLER_FIRING && !viewmodel_animator->IsPlayingAnimation(UID("StaplerFire"))) {
            player_state = PLAYER_STAPLER_IDLING;
        }
        
        if (UI::ismouse_left) {
            if (player_state == PLAYER_RIFLE_IDLING) {
                viewmodel_animator->StopAnimation(UID("RifleIdle"));
                viewmodel_animator->PlayAnimation(UID("RifleFire"), 1, 1.0f, 1.0f);
                
                auto result = Physics::Raycast(Render::CAMERA_POSITION, Render::CAMERA_POSITION + ((Render::CAMERA_ROTATION * Render::CAMERA_FORWARD)) * 100.0f);
                if (result && result->GetParent()) {
                    auto target_ent = result->GetParent();
                    if (typeid(*target_ent) == typeid(Moshkis)) {
                        Message msg {.type = Message::ACTIVATE, .data = (void*)500};
                        target_ent->MessageHandler(msg);
                        std::cout << "HIT MOSHKIS!" <<std::endl;
                    }
                }
        
                player_state = PLAYER_RIFLE_FIRING;
            } else if (player_state == PLAYER_STAPLER_IDLING) {
                viewmodel_animator->StopAnimation(UID("StaplerIdle"));
                viewmodel_animator->PlayAnimation(UID("StaplerFire"), 1, 1.0f, 1.0f);
            
                auto result = Physics::Raycast(Render::CAMERA_POSITION, Render::CAMERA_POSITION + ((Render::CAMERA_ROTATION * Render::CAMERA_FORWARD)) * 100.0f);
                if (result && result->GetParent()) {
                    auto target_ent = result->GetParent();
                    if (typeid(*target_ent) == typeid(Moshkis)) {
                        float damage = 50.0f * (7.0f - glm::distance(player->GetLocation(), target_ent->GetLocation()));
                        if (damage > 0.0f) {
                            Message msg {.type = Message::ACTIVATE, .data = (void*)((uint64_t)(damage))};
                            target_ent->MessageHandler(msg);
                        }
                        std::cout << "HIT MOSHKIS!" <<std::endl;
                    }
                }
            
                player_state = PLAYER_STAPLER_FIRING;
            } else {
                std::cout << "player is already firing" << std::endl;
            }
        }
    }
    
    void SwitchWeapon() {
        if (player_state == PLAYER_HAMMER_IDLING) {
            viewmodel_animator->StopAnimation(UID("AamursIdle"));
        } else if (player_state == PLAYER_RIFLE_IDLING) {
            viewmodel_animator->StopAnimation(UID("RifleIdle"));
        } else if (player_state == PLAYER_STAPLER_IDLING) {
            viewmodel_animator->StopAnimation(UID("StaplerIdle"));
        } else {
            std::cout << "player is firing, so can't switch" << std::endl;
            return;
        }
        
        
        // if you don't restart the components, then the models won't get updated
        // kinda dumb, but will fix later sometime
        viewmodel->Uninit();
        viewmodel_animator->Uninit();
        
        if (player_state == PLAYER_STAPLER_IDLING) {
            viewmodel->SetModel(UID("items/viewmodel_rifle"));
            viewmodel_animator->SetModel(UID("items/viewmodel_rifle"));
            player_state = PLAYER_RIFLE_IDLING;
            std::cout << "switched player to stapler" << std::endl;
        } else if (player_state == PLAYER_RIFLE_IDLING) {
            viewmodel->SetModel(UID("items/viewmodel_stapler"));
            viewmodel_animator->SetModel(UID("items/viewmodel_stapler"));
            player_state = PLAYER_STAPLER_IDLING;
            std::cout << "switched player to stapler" << std::endl;
        }
        
        viewmodel->Init();
        viewmodel_animator->Init();
        viewmodel->SetPose(viewmodel_animator->GetPosePtr());
    }
    
    Player* player;
    RenderComponent* viewmodel;
    ArmatureComponent* viewmodel_animator;
    
    enum {
        PLAYER_HAMMER_IDLING,
        PLAYER_RIFLE_IDLING,
        PLAYER_STAPLER_IDLING,
        PLAYER_HAMMER_FIRING,
        PLAYER_RIFLE_FIRING,
        PLAYER_STAPLER_FIRING,
    } player_state = PLAYER_STAPLER_IDLING;
    
    uint32_t hammer_ammo = 10;
    uint32_t rifle_ammo = 10;
    uint32_t stapler_ammo = 10;
};

PlayerStuff* main_player_stuff = nullptr;

int main() {
    std::cout << "Dziiviibas Partikula v0.1-alpha" << std::endl;

    Entity::Register("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::Register("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});
    Entity::Register("moshkis", [](std::string_view& params) -> Entity* {return new Moshkis(params);});

    Core::Init();
    UI::Init();
    Physics::InitPhysics();
    Render::Init();
    Async::Init();
    Audio::Init();

    // I think I should move this into engine code
    Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(new Model(UID("errorstatic")));

    LoadText("data/lv.lang");

    Material::LoadMaterialInfo("data/texture.list");
    
    Animation monster_animations (UID("creatures/moshkis"));
    monster_animations.LoadFromDisk();
    
    Animation viewmodel_animations (UID("items/viewmodels"));
    viewmodel_animations.LoadFromDisk();
    
    Animation pisckup_animations (UID("items/pickups"));
    pisckup_animations.LoadFromDisk();
    
    // adding references to viewmodels, so that they get loaded first
    Model::Find(UID("items/viewmodel_aamuris"))->AddRef();
    Model::Find(UID("items/viewmodel_rifle"))->AddRef();
    Model::Find(UID("items/viewmodel_stapler"))->AddRef();
    
    // audios
    Audio::Sound derp (UID("derp"));
    //derp.LoadFromDisk();


    WorldCell* outside1 = PoolProxy<WorldCell>::New();
    WorldCell* outside2 = PoolProxy<WorldCell>::New();
    WorldCell* factory1 = PoolProxy<WorldCell>::New();
    WorldCell* factory2 = PoolProxy<WorldCell>::New();
    WorldCell* dungeon1 = PoolProxy<WorldCell>::New();
    WorldCell* dungeon2 = PoolProxy<WorldCell>::New();
    WorldCell* minima = PoolProxy<WorldCell>::New();
    
    outside1->SetName(UID("outside1"));
    outside2->SetName(UID("outside2"));
    factory1->SetName(UID("factory1"));
    factory2->SetName(UID("factory2"));
    dungeon1->SetName(UID("dungeon1"));
    dungeon2->SetName(UID("dungeon2"));
    minima->SetName(UID("minima"));
    
    outside1->LoadFromDisk();
    outside2->LoadFromDisk();
    factory1->LoadFromDisk();
    factory2->LoadFromDisk();
    dungeon1->LoadFromDisk();
    dungeon2->LoadFromDisk();
    minima->LoadFromDisk();
    
    outside1->SetInterior(false); outside1->SetInteriorLights(false);
    outside2->SetInterior(false); outside2->SetInteriorLights(false);
    factory1->SetInterior(true); factory1->SetInteriorLights(false);
    factory2->SetInterior(true); factory2->SetInteriorLights(false);
    dungeon1->SetInterior(true); dungeon1->SetInteriorLights(false);
    dungeon2->SetInterior(true); dungeon2->SetInteriorLights(false);
    minima->SetInterior(true); minima->SetInteriorLights(false);
    
    outside1->AddLink(outside2);
    outside1->AddLink(dungeon1);
    outside1->AddLink(minima);
    
    outside2->AddLink(outside1);
    outside2->AddLink(dungeon2);
    outside2->AddLink(factory1);
    outside2->AddLink(factory2);
    
    
    Player player;
    //player.SetLocation(37.0f, 1.0f, -22.0f); // in the exit of dungeon1
    player.SetLocation(0.0f, 8.5f, -15.5f); // in the start of dungeon1
    player.Load();
    main_player = &player;

    PlayerStuff playerstuff(&player);
    main_player_stuff = &playerstuff;

    KeyActionBindings[GLFW_KEY_R]  = KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ glm::vec3 ploc; main_player->GetLocation(ploc); ploc += glm::vec3(0.0f, 3.0f, 0.0f); main_player->SetLocation(ploc); }};
    
    KeyActionBindings[GLFW_KEY_Q]  = KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ main_player_stuff->SwitchWeapon(); }};
    
    while(!SHOULD_CLOSE){
        UI::Update();

        if (UI::INPUT_STATE == STATE_DEFAULT) {
            player.GetLocation(Render::CAMERA_POSITION);
            player.GetLocation(Audio::LISTENER_POSITION);
            Render::CAMERA_POSITION += glm::vec3(0.0f, 0.5f, 0.0f);
            Audio::LISTENER_POSITION += glm::vec3(0.0f, 0.5f, 0.0f);
            Audio::LISTENER_ORIENTATION[0] = Render::CAMERA_ROTATION * Render::CAMERA_FORWARD;
            Audio::LISTENER_ORIENTATION[1] = Render::CAMERA_UP;
        }
        
        //time_of_day += 0.001f;
        SetSun(time_of_day); // this sets the ambient lighting


        static int tick = 0;
        tick++;

        playerstuff.Update();
        
        for (auto& comp : PoolProxy<MoshkisComponent>::GetPool()) comp.UpdateMoshkis();
        
        
        
        
        
        
        
        
        
        GUI::Begin();
        GUI::DebugMenu();
        GUI::EscapeMenu();        
        GUI::End();
        
        Audio::Update();
        
        // this loads the models and textures into video memory
        Async::ResourceLoader2ndStage();
        Async::FinishResource();


        Event::Dispatch();
        Message::Dispatch();
        
        WorldCell::Loader::LoadCells();
        
        ControllerComponent::UpdateAll();

        // updates the physics world
        float phys_step = 1.0f / 60.0f;
        Physics::StepPhysics(phys_step);

        Render::UpdateArmatures();
        Render::Render();

        UI::EndFrame();
    }

    Async::Yeet();

    Audio::Uninit();
    UI::Uninit();
}
