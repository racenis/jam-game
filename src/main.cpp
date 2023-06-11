// This file contains main() function and a big chunk of gameplay logic code.

#include <iostream>

#include <framework/core.h>
#include <framework/async.h>
#include <framework/ui.h>
#include <framework/message.h>
#include <framework/event.h>
#include <framework/worldcell.h>
#include <framework/loader.h>
#include <framework/language.h>
#include <physics/physics.h>
#include <audio/audio.h>

#include <framework/gui.h>

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

#include <extensions/menu/menu.h>
#include <extensions/camera/camera.h>

#include "moshkiscomponent.h"

#include "jamgame.h"

using namespace tram;
using namespace tram::Render;
using namespace tram::UI;

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
        //viewmodel->SetPose(poseList.begin().ptr);
        viewmodel->Init();
        viewmodel->SetLocation(glm::vec3(37.0f, 1.0f, -22.0f));
        viewmodel->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

        viewmodel_animator = PoolProxy<ArmatureComponent>::New();
        viewmodel_animator->SetModel(UID("items/viewmodel_stapler"));
        viewmodel_animator->Init();

        //viewmodel->SetPose(viewmodel_animator->GetPosePtr());
        viewmodel->SetArmature(viewmodel_animator);
        
        
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
        
        vec3 new_position = Render::GetCameraPosition() - glm::vec3(0.0f, 0.35f, 0.0f);
        quat new_rotation = Render::GetCameraRotation();
        
        ammortized_position = glm::mix(ammortized_position, new_position, 0.4f);
        ammortized_rotation = glm::mix(ammortized_rotation, new_rotation, 0.2f);
        
        viewmodel->SetLocation(ammortized_position);
        viewmodel->SetRotation(ammortized_rotation);
        auto cell = WorldCell::Find(Render::GetCameraPosition());
        if (cell) viewmodel->SetWorldParameters(cell->HasInteriorLighting());
        
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
        
        bool firing = !mouse_last && UI::PollKeyboardKey(KEY_LEFTMOUSE);
        mouse_last = UI::PollKeyboardKey(KEY_LEFTMOUSE);
        // if left mouse button is clicked
        if (firing/*UI::ismouse_left*/) {
            if (player_state == PLAYER_RIFLE_IDLING && rifle_ammo) {
                viewmodel_animator->StopAnimation(UID("RifleIdle"));
                viewmodel_animator->PlayAnimation(UID("RifleFire"), 1, 1.0f, 1.2f);
                
                PlaySoundEffect(SOUND_RIFLE_FIRE, Render::GetCameraPosition());
                
                auto result = Physics::Raycast(Render::GetCameraPosition(), Render::GetCameraPosition() + ((Render::GetCameraRotation() * DIRECTION_FORWARD)) * 100.0f).collider;
                
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
                
                PlaySoundEffect(SOUND_STAPLER_FIRE, Render::GetCameraPosition());
            
                auto result = Physics::Raycast(Render::GetCameraPosition(), Render::GetCameraPosition() + ((Render::GetCameraRotation() * DIRECTION_FORWARD)) * 100.0f).collider;
                
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
                            PlaySoundEffect(SOUND_MOSHKIS_POP, Render::GetCameraPosition());
                        }
                    }
                }
                stapler_ammo--;
                player_state = PLAYER_STAPLER_FIRING;
            } else {
                PlaySoundEffect(SOUND_PICKUP_CLICK, Render::GetCameraPosition());
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
            PlaySoundEffect(SOUND_PICKUP_CLICK, Render::GetCameraPosition());
            return;
        }
        
        
        // if you don't restart the components, then the models won't get updated
        // not sure if that's an error, or bad design on my part
        
        PoolProxy<RenderComponent>::Delete(viewmodel);
        PoolProxy<ArmatureComponent>::Delete(viewmodel_animator);
        
        viewmodel = PoolProxy<RenderComponent>::New();
        viewmodel_animator = PoolProxy<ArmatureComponent>::New();
        
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
        //viewmodel->SetPose(viewmodel_animator->GetPosePtr());
        viewmodel->SetArmature(viewmodel_animator);
        
        viewmodel_animator->SetOnAnimationFinishCallback([](ArmatureComponent* comp, name_t name) {
            main_player_stuff->AnimationStopped(name);
        });
    }
    
    Player* player;
    RenderComponent* viewmodel;
    ArmatureComponent* viewmodel_animator;
    
    vec3 ammortized_position = {0.0f, 0.0f, 0.0f};
    quat ammortized_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    
    enum {
        PLAYER_HAMMER_IDLING,
        PLAYER_RIFLE_IDLING,
        PLAYER_STAPLER_IDLING,
        PLAYER_HAMMER_FIRING,
        PLAYER_RIFLE_FIRING,
        PLAYER_STAPLER_FIRING,
    } player_state = PLAYER_STAPLER_IDLING;
    
    
    bool mouse_last = false;
    
    uint32_t hammer_ammo = 10;
    uint32_t rifle_ammo = 10;
    uint32_t stapler_ammo = 10;
    int32_t player_health = 420;
    
    uint32_t ticks_since_oof = 100;
};

// this is called by the pickup entity when the player walks into it
void PlayerPickedUpPickup(name_t pickup_model) {
    if (pickup_model == UID("items/pickups_lifeparticle")) main_player_stuff->player_health += 150;
    if (pickup_model == UID("items/pickups_rifle")) main_player_stuff->rifle_ammo += 5;
    if (pickup_model == UID("items/pickups_stapler")) main_player_stuff->stapler_ammo += 20;
    
    PlaySoundEffect(SOUND_PICKUP_PICKUP, Render::GetCameraPosition());
}

// this is called by the monster entity when it successfully hits the player
void PlayerGotHitInFace(uint64_t oof_size) {
    main_player_stuff->ticks_since_oof = 0;
    main_player_stuff->player_health -= oof_size;
}

int main() {
    std::cout << "Dziiviibas Partikula v1.1" << std::endl;

    // registering in all of the entities, so that they can be loaded from the level file
    StaticWorldObject::Register();
    Crate::Register();
    Lamp::Register();
    Moshkis::Register();
    Pickup::Register();
    
    //Entity::Register("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    //Entity::Register("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});
    //Entity::Register("lamp", [](std::string_view& params) -> Entity* {return new Lamp(params);});
    //Entity::Register("moshkis", [](std::string_view& params) -> Entity* {return new Moshkis(params);});
    //Entity::Register("pickup", [](std::string_view& params) -> Entity* {return new Pickup(params);});

    // initialization of the systems that we'll be using
    Core::Init();
    UI::Init();
    Physics::Init();
    Render::Init();
    Async::Init(0);
    Audio::Init();
    GUI::Init();

    // I think I should move this into engine code
    //Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    //Model::SetErrorModel(new Model(UID("errorstatic")));

    Ext::Menu::Init();
    Ext::Camera::Init();

    // all of the text for this game is stored in the executable, but
    // if you don't load a language file, then some parts of the engine
    // will glitch out for some reason
    //LoadText("data/lv.lang");
    Language::Load("lv");

    // load up all of the material (texture) definitions
    Material::LoadMaterialInfo("texture");
    
    // loading up all of the animations
    Animation::Find(UID("AamursFire"))->LoadFromDisk();
    Animation::Find(UID("AamursIdle"))->LoadFromDisk();
    Animation::Find(UID("MoshkisAttack"))->LoadFromDisk();
    Animation::Find(UID("MoshkisDie"))->LoadFromDisk();
    Animation::Find(UID("MoshkisFlinch"))->LoadFromDisk();
    Animation::Find(UID("MoshkisIdle"))->LoadFromDisk();
    Animation::Find(UID("MoshkisWalk"))->LoadFromDisk();
    Animation::Find(UID("PickupSpin"))->LoadFromDisk();
    Animation::Find(UID("RifleFire"))->LoadFromDisk();
    Animation::Find(UID("RifleIdle"))->LoadFromDisk();
    Animation::Find(UID("StaplerFire"))->LoadFromDisk();
    Animation::Find(UID("StaplerIdle"))->LoadFromDisk();
    
    // loading up all of the audio files
    LoadSoundEffects();
    
    // pre-loading viewmodels, so that there's no awkward pause when switching between them
    Model::Find(UID("items/viewmodel_aamuris"))->AddReference(); Async::RequestResource(nullptr, Model::Find(UID("items/viewmodel_aamuris")));
    Model::Find(UID("items/viewmodel_rifle"))->AddReference(); Async::RequestResource(nullptr, Model::Find(UID("items/viewmodel_rifle")));
    Model::Find(UID("items/viewmodel_stapler"))->AddReference(); Async::RequestResource(nullptr, Model::Find(UID("items/viewmodel_stapler")));
    
    // creating the parts of the level that will be streamed in
    WorldCell::Make(UID("outside1"));
    WorldCell::Make(UID("outside2"));
    WorldCell::Make(UID("factory1"));
    WorldCell::Make(UID("factory2"));
    WorldCell::Make(UID("dungeon1"));
    WorldCell::Make(UID("dungeon2"));
    WorldCell::Make(UID("minima"));
    
    WorldCell::Find(UID("outside1"))->LoadFromDisk();
    WorldCell::Find(UID("outside2"))->LoadFromDisk();
    WorldCell::Find(UID("factory1"))->LoadFromDisk();
    WorldCell::Find(UID("factory2"))->LoadFromDisk();
    WorldCell::Find(UID("dungeon1"))->LoadFromDisk();
    WorldCell::Find(UID("dungeon2"))->LoadFromDisk();
    WorldCell::Find(UID("minima"))->LoadFromDisk();
    
    // SetInterior() is so that the transitions work properly
    // SetInteriorLights() is so that objects inside interiors don't get lit by the sun
    //outside1->SetInterior(false); outside1->SetInteriorLights(false);
    //outside2->SetInterior(false); outside2->SetInteriorLights(false);
    //factory1->SetInterior(true); factory1->SetInteriorLights(true);
    //factory2->SetInterior(true); factory2->SetInteriorLights(true);
    //dungeon1->SetInterior(true); dungeon1->SetInteriorLights(true);
    //dungeon2->SetInterior(true); dungeon2->SetInteriorLights(true);
    //minima->SetInterior(true); minima->SetInteriorLights(true);
    
    // adding links between parts of the level, so that the level loader
    // understands in which part of the level you are inside of
    //outside1->AddLink(outside2);
    //outside1->AddLink(dungeon1);
    //outside1->AddLink(minima);
    
    //outside2->AddLink(outside1);
    //outside2->AddLink(dungeon2);
    //outside2->AddLink(factory1);
    //outside2->AddLink(factory2);
    
    
    Player player;
    //player.SetLocation(vec3(37.0f, 1.0f, -22.0f)); // in the exit of dungeon1
    player.SetLocation(vec3(0.0f, 8.75f, -15.5f)); // in the start of dungeon1
    player.Load();
    main_player = &player;
    
    Ext::Camera::Camera camera;
    camera.SetMouselook(true);
    camera.SetRotateFollowing(true);
    camera.SetBobbingDistance(0.15f);
    camera.SetFollowingOffset({0.0f, 0.5f, 0.0f});
    camera.SetFollowing(&player);
    
    Ext::Camera::SetCamera(&camera);

    PlayerStuff playerstuff(&player);
    main_player_stuff = &playerstuff;

    BindKeyboardKey(KEY_R, [](){ main_player->SetLocation(vec3(0.0f, 8.75f, -15.5f)); });
    BindKeyboardKey(KEY_T, [](){ main_player->SetLocation(vec3(0.0f, 25.0f, -15.5f)); });
    BindKeyboardKey(KEY_Q, [](){ main_player_stuff->SwitchWeapon(); });
    
    // ideally you would update the skybox's each frame, so that it is centered
    // on the player, but whatever
    auto skybox = PoolProxy<RenderComponent>::New();
    skybox->SetLightmap(UID("fullbright"));
    skybox->SetWorldParameters(true);
    skybox->SetLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    skybox->SetRotation(glm::quat(glm::vec3(0.0f, 3.14f, 0.0f)));
    skybox->SetModel(UID("skybox"));
    skybox->Init();
    
    // this will get triggered when the player reaches the end of the last dungeon
    auto end_trigger = PoolProxy<TriggerComponent>::New();
    end_trigger->SetLocation(glm::vec3(-65.3f, -6.9f, 65.9f));
    end_trigger->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
    end_trigger->SetParent(nullptr);
    end_trigger->SetModel(UID("box1meter"));
    end_trigger->SetActivationCallback([](TriggerComponent* comp, Physics::Collision){ is_finished = true; });
    end_trigger->SetFilterCallback([](TriggerComponent* trig, PhysicsComponent* comp){ return comp && comp->GetParent() && comp->GetParent()->GetName() == UID("player"); });
    
    Render::SetSunDirection(glm::normalize(glm::vec3(0.0f, 1.0f, 0.5f)));
    Render::SetSunColor(glm::vec3(250.0f, 214.0f, 165.0f) / 256.0f * 0.8f);
    Render::SetAmbientColor((glm::vec3(250.0f, 214.0f, 165.0f) / 256.0f * 0.8f) * 0.7f);
    
    while(!EXIT){
        Core::Update();
        UI::Update();

        playerstuff.Update();
        
        if (UI::PollKeyboardKey(UI::KEY_A)) {
            camera.SetTilt(0.1f);
        } else if (UI::PollKeyboardKey(UI::KEY_D)) {
            camera.SetTilt(-0.1f);
        } else {
            camera.SetTilt(0.0f);
        }
        
        if (UI::PollKeyboardKey(UI::KEY_W) || UI::PollKeyboardKey(UI::KEY_S) ||
            UI::PollKeyboardKey(UI::KEY_A) || UI::PollKeyboardKey(UI::KEY_D)) {
            camera.SetBobbing(1.0f);
        } else {
            camera.SetBobbing(0.0f);
        }
        
        Ext::Camera::Update();
        
        for (auto& comp : PoolProxy<MoshkisComponent>::GetPool()) comp.UpdateMoshkis();
        
        GUI::Begin();
            Ext::Menu::DebugMenu();
            Ext::Menu::EscapeMenu();
        
            GUI::Frame(GUI::FRAME_BOTTOM, 50.0f);
                char ammobuffer[100]; char healthbuffer[100];
                
                bool player_using_stapler = playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_IDLING || playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_FIRING;
                auto ammocount = player_using_stapler ? playerstuff.stapler_ammo : playerstuff.rifle_ammo;
                sprintf(ammobuffer, "Ammunition: %d",  ammocount);
                
                auto healthtext = (playerstuff.player_health > 0) ? "Health: %d" : "YOURE DEAD: %d";
                sprintf(healthbuffer, healthtext, playerstuff.player_health);
                    
                bool firing = playerstuff.player_state == PlayerStuff::PLAYER_RIFLE_FIRING || playerstuff.player_state == PlayerStuff::PLAYER_STAPLER_FIRING;
                
                GUI::Text(ammobuffer, 1, GUI::TEXT_CENTER, firing ? COLOR_GRAY : COLOR_WHITE); GUI::FrameBreakLine();
                GUI::Text(healthbuffer, 1, GUI::TEXT_CENTER, playerstuff.ticks_since_oof > 100 ? COLOR_WHITE : glm::vec3(1.0f, ((float)playerstuff.ticks_since_oof)/100.0f, ((float)playerstuff.ticks_since_oof)/100.0f));
            GUI::EndFrame();
            
            if (is_finished) {
                GUI::Frame(GUI::FRAME_BOTTOM, 100.0f);
                GUI::Text("YOU ARE WINNER!", 1, GUI::TEXT_CENTER, COLOR_GREEN);
                GUI::EndFrame();
            }
            
        GUI::End();
        GUI::Update();
        
        Audio::Update();
        
        Async::ResourceLoader1stStage();
        Async::ResourceLoader2ndStage();
        Async::FinishResource();


        Event::Dispatch();
        Message::Dispatch();
        
        Loader::Update();
        //WorldCell::Loader::LoadCells();
        
        // ugly hack
        if (GetTick() > 100) {
            ControllerComponent::Update();
        }
        
        
        
        ArmatureComponent::Update();

        Physics::Update();

        Render::Render();

        UI::EndFrame();
    }

    Async::Yeet();

    Audio::Uninit();
    UI::Uninit();
}
