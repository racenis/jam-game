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
#include <entities/lamp.h>
#include <entities/player.h>
#include "moshkis.h"
#include "pickup.h"

#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>
#include <components/lightcomponent.h>

#include <components/triggercomponent.h>
#include <components/audiocomponent.h>

#include <components/controllercomponent.h>
#include <components/physicscomponent.h>

#include "moshkiscomponent.h"

#include "jamgame.h"

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;

Player* main_player = nullptr;

bool is_finished = false;

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
        auto cell = WorldCell::Find(Render::CAMERA_POSITION);
        if (cell) viewmodel->SetCellParams(cell->HasInteriorLighting());
        
        ticks_since_oof++;
        
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
            if (player_state == PLAYER_RIFLE_IDLING && rifle_ammo) {
                viewmodel_animator->StopAnimation(UID("RifleIdle"));
                viewmodel_animator->PlayAnimation(UID("RifleFire"), 1, 1.0f, 1.0f);
                
                PlaySoundEffect(SOUND_RIFLE_FIRE, Render::CAMERA_POSITION);
                
                auto result = Physics::Raycast(Render::CAMERA_POSITION, Render::CAMERA_POSITION + ((Render::CAMERA_ROTATION * Render::CAMERA_FORWARD)) * 100.0f);
                if (result && result->GetParent()) {
                    auto target_ent = result->GetParent();
                    if (typeid(*target_ent) == typeid(Moshkis)) {
                        Message msg {.type = Message::ACTIVATE, .data = (void*)500};
                        target_ent->MessageHandler(msg);
                        std::cout << "HIT MOSHKIS!" <<std::endl;
                    }
                }
                rifle_ammo--;
                player_state = PLAYER_RIFLE_FIRING;
            } else if (player_state == PLAYER_STAPLER_IDLING && stapler_ammo) {
                viewmodel_animator->StopAnimation(UID("StaplerIdle"));
                viewmodel_animator->PlayAnimation(UID("StaplerFire"), 1, 1.0f, 1.0f);
                
                PlaySoundEffect(SOUND_STAPLER_FIRE, Render::CAMERA_POSITION);
            
                auto result = Physics::Raycast(Render::CAMERA_POSITION, Render::CAMERA_POSITION + ((Render::CAMERA_ROTATION * Render::CAMERA_FORWARD)) * 100.0f);
                if (result && result->GetParent()) {
                    auto target_ent = result->GetParent();
                    if (typeid(*target_ent) == typeid(Moshkis)) {
                        float damage = 50.0f * (7.0f - glm::distance(player->GetLocation(), target_ent->GetLocation()));
                        if (damage > 0.0f) {
                            Message msg {.type = Message::ACTIVATE, .data = (void*)((uint64_t)(damage))};
                            target_ent->MessageHandler(msg);
                            PlaySoundEffect(SOUND_MOSHKIS_POP, Render::CAMERA_POSITION);
                        }
                        
                        std::cout << "HIT MOSHKIS!" <<std::endl;
                    }
                }
                stapler_ammo--;
                player_state = PLAYER_STAPLER_FIRING;
            } else {
                std::cout << "player is already firing or out of ammo" << std::endl;
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
    int32_t player_health = 420;
    
    uint32_t ticks_since_oof = 100;
};

PlayerStuff* main_player_stuff = nullptr;

void PlayerPickedUpPickup(Core::name_t pickup_model) {
    std::cout << "PICKED UP " << ReverseUID(pickup_model) << std::endl;
    
    if (pickup_model == UID("items/pickups_lifeparticle")) main_player_stuff->player_health += 150;
    if (pickup_model == UID("items/pickups_rifle")) main_player_stuff->rifle_ammo += 5;
    if (pickup_model == UID("items/pickups_stapler")) main_player_stuff->stapler_ammo += 20;
    
    PlaySoundEffect(SOUND_PICKUP_PICKUP, Render::CAMERA_POSITION);
}

void PlayerGotHitInFace(uint64_t oof_size) {
    std::cout << "GOT HIT IN FACE " << oof_size << std::endl;
    
    main_player_stuff->ticks_since_oof = 0;
    main_player_stuff->player_health -= oof_size;
}

int main() {
    std::cout << "Dziiviibas Partikula v0.1-alpha" << std::endl;

    Entity::Register("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::Register("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});
    Entity::Register("lamp", [](std::string_view& params) -> Entity* {return new Lamp(params);});
    Entity::Register("moshkis", [](std::string_view& params) -> Entity* {return new Moshkis(params);});
    Entity::Register("pickup", [](std::string_view& params) -> Entity* {return new Pickup(params);});

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
    
    LoadSoundEffects();
    
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
    factory1->SetInterior(true); factory1->SetInteriorLights(true);
    factory2->SetInterior(true); factory2->SetInteriorLights(true);
    dungeon1->SetInterior(true); dungeon1->SetInteriorLights(true);
    dungeon2->SetInterior(true); dungeon2->SetInteriorLights(true);
    minima->SetInterior(true); minima->SetInteriorLights(true);
    
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
    //player.SetLocation(0.0f, 0.0f, 0.0f);
    player.Load();
    main_player = &player;

    PlayerStuff playerstuff(&player);
    main_player_stuff = &playerstuff;

    KeyActionBindings[GLFW_KEY_R]  = KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ glm::vec3 ploc; main_player->GetLocation(ploc); ploc += glm::vec3(0.0f, 3.0f, 0.0f); main_player->SetLocation(ploc); }};
    
    KeyActionBindings[GLFW_KEY_Q]  = KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ main_player_stuff->SwitchWeapon(); }};
    
    auto skybox = PoolProxy<RenderComponent>::New();
    skybox->SetLightmap(UID("fullbright"));
    skybox->SetCellParams(true);
    skybox->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    skybox->UpdateRotation(glm::quat(glm::vec3(0.0f, 3.14f, 0.0f)));
    skybox->SetModel(UID("skybox"));
    skybox->Init();
    
    auto end_trigger = PoolProxy<TriggerComponent>::New();
    end_trigger->SetLocation(glm::vec3(-65.3f, -6.9f, 65.9f));
    end_trigger->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
    end_trigger->SetParent(nullptr);
    end_trigger->SetModel(UID("box1meter"));
    end_trigger->SetActivationCallback([](TriggerComponent* comp){ is_finished = true; });
    end_trigger->SetFilterCallback([](TriggerComponent* trig, PhysicsComponent* comp){ return comp && comp->GetParent() && comp->GetParent()->GetName() == UID("player"); });
    
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
        
            GUI::Frame(Core::GUI::FRAME_BOTTOM, 50.0f);
                char ammobuffer[100]; char healthbuffer[100];
                if (playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_IDLING || playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_FIRING)
                    sprintf(ammobuffer, "Ammunition: %d",  playerstuff.stapler_ammo);
                else
                    sprintf(ammobuffer, "Ammunition: %d",  playerstuff.rifle_ammo);
                if (playerstuff.player_health > 0)
                    sprintf(healthbuffer, "Health: %d", playerstuff.player_health);
                else
                    sprintf(healthbuffer, "YOURE DEAD: %d", playerstuff.player_health);
                bool firing = playerstuff.player_state == PlayerStuff::PLAYER_RIFLE_FIRING || playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_FIRING;
                GUI::Text(ammobuffer, 1, Core::GUI::TEXT_CENTER, firing ? COLOR_DISABLED : COLOR_WHITE); GUI::FrameBreakLine();
                GUI::Text(healthbuffer, 1, Core::GUI::TEXT_CENTER, playerstuff.ticks_since_oof > 100 ? COLOR_WHITE : glm::vec3(1.0f, ((float)playerstuff.ticks_since_oof)/100.0f, ((float)playerstuff.ticks_since_oof)/100.0f));
            GUI::EndFrame();
            
            if (is_finished) {
                GUI::Frame(Core::GUI::FRAME_BOTTOM, 100.0f);
                GUI::Text("YOU ARE WINNER!", 1, Core::GUI::TEXT_CENTER, COLOR_GREEN);
                GUI::EndFrame();
            }
            
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
