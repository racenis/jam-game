// This file contains main() function and a big chunk of gameplay logic code.

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
//#include <components/lightcomponent.h>

#include <components/triggercomponent.h>
#include <components/audiocomponent.h>

#include <components/controllercomponent.h>
//#include <components/physicscomponent.h>

#include "moshkiscomponent.h"

#include "jamgame.h"

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;

// forward declaration
class PlayerStuff;

PlayerStuff* main_player_stuff = nullptr;

// definition of the global variable declaration in jamgame.h
Player* main_player = nullptr;

// set to true when player reaches the end of the final dungeon,
// so that congratulatory message can be displayed
bool is_finished = false;

// this class keeps track of the player's health, ammunition count,
// as well as the viewmodels and their animations
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
        
        viewmodel_animator->SetOnAnimationFinishCallback([](ArmatureComponent* comp, name_t name) {
            main_player_stuff->AnimationStopped(name);
        });
    }
    
    void AnimationStopped(name_t animation) {
        if (animation == UID("AamursFire")) {
            player_state = PLAYER_HAMMER_IDLING;
        } else if (animation == UID("RifleFire")) {
            player_state = PLAYER_RIFLE_IDLING;
        } else if (animation == UID("StaplerFire")) {
            player_state = PLAYER_STAPLER_IDLING;
        } else {
            return;
        }
        
        Update();
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
        
        // if left mouse button is clicked
        if (UI::ismouse_left) {
            if (player_state == PLAYER_RIFLE_IDLING && rifle_ammo) {
                viewmodel_animator->StopAnimation(UID("RifleIdle"));
                viewmodel_animator->PlayAnimation(UID("RifleFire"), 1, 1.0f, 1.2f);
                
                PlaySoundEffect(SOUND_RIFLE_FIRE, Render::CAMERA_POSITION);
                
                auto result = Physics::Raycast(Render::CAMERA_POSITION, Render::CAMERA_POSITION + ((Render::CAMERA_ROTATION * DIRECTION_FORWARD)) * 100.0f);
                
                if (result && result->GetParent()) {
                    auto target_ent = result->GetParent();
                    
                    if (typeid(*target_ent) == typeid(Moshkis)) {
                        // notice how we're type-punning the data pointer into an integer.
                        // there's no real benefit to doing it this way, I just felt like
                        // avoiding a memory allocation
                        Message msg {.type = Message::ACTIVATE, .data = (void*)500};
                        target_ent->MessageHandler(msg);
                    }
                }
                
                rifle_ammo--;
                player_state = PLAYER_RIFLE_FIRING;
            } else if (player_state == PLAYER_STAPLER_IDLING && stapler_ammo) {
                viewmodel_animator->StopAnimation(UID("StaplerIdle"));
                viewmodel_animator->PlayAnimation(UID("StaplerFire"), 1, 1.0f, 2.0f);
                
                PlaySoundEffect(SOUND_STAPLER_FIRE, Render::CAMERA_POSITION);
            
                auto result = Physics::Raycast(Render::CAMERA_POSITION, Render::CAMERA_POSITION + ((Render::CAMERA_ROTATION * DIRECTION_FORWARD)) * 100.0f);
                
                if (result && result->GetParent()) {
                    auto target_ent = result->GetParent();
                    
                    if (typeid(*target_ent) == typeid(Moshkis)) {
                        // linear drop-off, from 350 damage when shooting right into the monster's face,
                        // decreasing by 50 damage for every meter and stopping at 0 after 7 meters
                        float damage = 50.0f * (7.0f - glm::distance(player->GetLocation(), target_ent->GetLocation()));
                        
                        // if you don't check for negative damage, then shooting at distances over 7 meters
                        // would give health to the monster, instead of damaging it
                        if (damage > 0.0f) {
                            Message msg {.type = Message::ACTIVATE, .data = (void*)((uint64_t)(damage))};
                            target_ent->MessageHandler(msg);
                            PlaySoundEffect(SOUND_MOSHKIS_POP, Render::CAMERA_POSITION);
                        }
                    }
                }
                stapler_ammo--;
                player_state = PLAYER_STAPLER_FIRING;
            } else {
                PlaySoundEffect(SOUND_PICKUP_CLICK, Render::CAMERA_POSITION);
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
            PlaySoundEffect(SOUND_PICKUP_CLICK, Render::CAMERA_POSITION);
            return;
        }
        
        
        // if you don't restart the components, then the models won't get updated
        // not sure if that's an error, or bad design on my part
        viewmodel->Uninit();
        viewmodel_animator->Uninit();
        
        if (player_state == PLAYER_STAPLER_IDLING) {
            viewmodel->SetModel(UID("items/viewmodel_rifle"));
            viewmodel_animator->SetModel(UID("items/viewmodel_rifle"));
            player_state = PLAYER_RIFLE_IDLING;
        } else if (player_state == PLAYER_RIFLE_IDLING) {
            viewmodel->SetModel(UID("items/viewmodel_stapler"));
            viewmodel_animator->SetModel(UID("items/viewmodel_stapler"));
            player_state = PLAYER_STAPLER_IDLING;
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

// this is called by the pickup entity when the player walks into it
void PlayerPickedUpPickup(Core::name_t pickup_model) {
    if (pickup_model == UID("items/pickups_lifeparticle")) main_player_stuff->player_health += 150;
    if (pickup_model == UID("items/pickups_rifle")) main_player_stuff->rifle_ammo += 5;
    if (pickup_model == UID("items/pickups_stapler")) main_player_stuff->stapler_ammo += 20;
    
    PlaySoundEffect(SOUND_PICKUP_PICKUP, Render::CAMERA_POSITION);
}

// this is called by the monster entity when it successfully hits the player
void PlayerGotHitInFace(uint64_t oof_size) {
    main_player_stuff->ticks_since_oof = 0;
    main_player_stuff->player_health -= oof_size;
}

int main() {
    std::cout << "Dziiviibas Partikula v1.1" << std::endl;

    // registering in all of the entities, so that they can be loaded from the level file
    Entity::Register("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::Register("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});
    Entity::Register("lamp", [](std::string_view& params) -> Entity* {return new Lamp(params);});
    Entity::Register("moshkis", [](std::string_view& params) -> Entity* {return new Moshkis(params);});
    Entity::Register("pickup", [](std::string_view& params) -> Entity* {return new Pickup(params);});

    // initialization of the systems that we'll be using
    Core::Init();
    UI::Init();
    Physics::InitPhysics();
    Render::Init();
    Async::Init();
    Audio::Init();

    // I think I should move this into engine code
    Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(new Model(UID("errorstatic")));

    // all of the text for this game is stored in the executable, but
    // if you don't load a language file, then some parts of the engine
    // will glitch out for some reason
    LoadText("data/lv.lang");

    // load up all of the material (texture) definitions
    Material::LoadMaterialInfo("data/texture.list");
    
    // loading up all of the animations
    Animation monster_animations (UID("creatures/moshkis"));
    Animation viewmodel_animations (UID("items/viewmodels"));
    Animation pisckup_animations (UID("items/pickups"));
    
    viewmodel_animations.LoadFromDisk();
    monster_animations.LoadFromDisk();
    pisckup_animations.LoadFromDisk();
    
    // loading up all of the audio files
    LoadSoundEffects();
    
    // pre-loading viewmodels, so that there's no awkward pause when switching between them
    Model::Find(UID("items/viewmodel_aamuris"))->AddRef(); Async::RequestResource(nullptr, Model::Find(UID("items/viewmodel_aamuris")));
    Model::Find(UID("items/viewmodel_rifle"))->AddRef(); Async::RequestResource(nullptr, Model::Find(UID("items/viewmodel_rifle")));
    Model::Find(UID("items/viewmodel_stapler"))->AddRef(); Async::RequestResource(nullptr, Model::Find(UID("items/viewmodel_stapler")));
    
    // creating the parts of the level that will be streamed in
    WorldCell* outside1 = PoolProxy<WorldCell>::New();
    WorldCell* outside2 = PoolProxy<WorldCell>::New();
    WorldCell* factory1 = PoolProxy<WorldCell>::New();
    WorldCell* factory2 = PoolProxy<WorldCell>::New();
    WorldCell* dungeon1 = PoolProxy<WorldCell>::New();
    WorldCell* dungeon2 = PoolProxy<WorldCell>::New();
    WorldCell* minima = PoolProxy<WorldCell>::New();
    
    // self-explanatory
    outside1->SetName(UID("outside1"));
    outside2->SetName(UID("outside2"));
    factory1->SetName(UID("factory1"));
    factory2->SetName(UID("factory2"));
    dungeon1->SetName(UID("dungeon1"));
    dungeon2->SetName(UID("dungeon2"));
    minima->SetName(UID("minima"));
    
    // this loads the entity and transition definitions from the disk
    outside1->LoadFromDisk();
    outside2->LoadFromDisk();
    factory1->LoadFromDisk();
    factory2->LoadFromDisk();
    dungeon1->LoadFromDisk();
    dungeon2->LoadFromDisk();
    minima->LoadFromDisk();
    
    // SetInterior() is so that the transitions work properly
    // SetInteriorLights() is so that objects inside interiors don't get lit by the sun
    outside1->SetInterior(false); outside1->SetInteriorLights(false);
    outside2->SetInterior(false); outside2->SetInteriorLights(false);
    factory1->SetInterior(true); factory1->SetInteriorLights(true);
    factory2->SetInterior(true); factory2->SetInteriorLights(true);
    dungeon1->SetInterior(true); dungeon1->SetInteriorLights(true);
    dungeon2->SetInterior(true); dungeon2->SetInteriorLights(true);
    minima->SetInterior(true); minima->SetInteriorLights(true);
    
    // adding links between parts of the level, so that the level loader
    // understands in which part of the level you are inside of
    outside1->AddLink(outside2);
    outside1->AddLink(dungeon1);
    outside1->AddLink(minima);
    
    outside2->AddLink(outside1);
    outside2->AddLink(dungeon2);
    outside2->AddLink(factory1);
    outside2->AddLink(factory2);
    
    
    Player player;
    //player.SetLocation(37.0f, 1.0f, -22.0f); // in the exit of dungeon1
    player.SetLocation(0.0f, 8.75f, -15.5f); // in the start of dungeon1
    player.Load();
    main_player = &player;

    PlayerStuff playerstuff(&player);
    main_player_stuff = &playerstuff;

    KeyActionBindings[GLFW_KEY_R]  = KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ main_player->SetLocation(0.0f, 8.75f, -15.5f); }};
    KeyActionBindings[GLFW_KEY_Q]  = KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ main_player_stuff->SwitchWeapon(); }};
    
    // ideally you would update the skybox's each frame, so that it is centered
    // on the player, but whatever
    auto skybox = PoolProxy<RenderComponent>::New();
    skybox->SetLightmap(UID("fullbright"));
    skybox->SetCellParams(true);
    skybox->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    skybox->UpdateRotation(glm::quat(glm::vec3(0.0f, 3.14f, 0.0f)));
    skybox->SetModel(UID("skybox"));
    skybox->Init();
    
    // this will get triggered when the player reaches the end of the last dungeon
    auto end_trigger = PoolProxy<TriggerComponent>::New();
    end_trigger->SetLocation(glm::vec3(-65.3f, -6.9f, 65.9f));
    end_trigger->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
    end_trigger->SetParent(nullptr);
    end_trigger->SetModel(UID("box1meter"));
    end_trigger->SetActivationCallback([](TriggerComponent* comp){ is_finished = true; });
    end_trigger->SetFilterCallback([](TriggerComponent* trig, PhysicsComponent* comp){ return comp && comp->GetParent() && comp->GetParent()->GetName() == UID("player"); });
    
    Render::SUN_DIRECTION = glm::normalize(glm::vec3(0.0f, 1.0f, 0.5f));
    Render::SUN_COLOR = glm::vec3(250.0f, 214.0f, 165.0f) / 256.0f * 0.8f;
    Render::AMBIENT_COLOR = Render::SUN_COLOR * 0.7f;
    
    while(!SHOULD_CLOSE){
        UI::Update();

        // this makes the camera follow the player character
        if (UI::INPUT_STATE == STATE_DEFAULT) {
            glm::vec3 player_head;
            player.GetLocation(player_head);
            player_head += glm::vec3(0.0f, 0.5f, 0.0f);
            Render::CAMERA_POSITION = player_head;
            Audio::SetListenerPosition(player_head);
            Audio::SetListenerOrientation(Render::CAMERA_ROTATION);
        }

        playerstuff.Update();
        
        for (auto& comp : PoolProxy<MoshkisComponent>::GetPool()) comp.UpdateMoshkis();
        
        GUI::Begin();
            GUI::DebugMenu();
            GUI::EscapeMenu();
        
            GUI::Frame(Core::GUI::FRAME_BOTTOM, 50.0f);
                char ammobuffer[100]; char healthbuffer[100];
                
                bool player_using_stapler = playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_IDLING || playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_FIRING;
                auto ammocount = player_using_stapler ? playerstuff.stapler_ammo : playerstuff.rifle_ammo;
                sprintf(ammobuffer, "Ammunition: %d",  ammocount);
                
                auto healthtext = (playerstuff.player_health > 0) ? "Health: %d" : "YOURE DEAD: %d";
                sprintf(healthbuffer, healthtext, playerstuff.player_health);
                    
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
        
        Async::ResourceLoader2ndStage();
        Async::FinishResource();


        Event::Dispatch();
        Message::Dispatch();
        
        WorldCell::Loader::LoadCells();
        
        ControllerComponent::UpdateAll();

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
