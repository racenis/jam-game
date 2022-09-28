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

#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>
#include <components/lightcomponent.h>

#include <components/spritecomponent.h>
#include <components/particlecomponent.h>
#include <components/audiocomponent.h>

#include <components/controllercomponent.h>

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;

Player* pler = nullptr;

int main() {
    std::cout << "Dziiviibas Partikula v0.1-alpha" << std::endl;
    //std::cout << std::filesystem::current_path() << std::endl;

    // register the entity types, so that they can be loaded from level files
    Entity::Register("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::Register("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});

    Core::Init();           // core init should always be first
    UI::Init();
    Physics::InitPhysics(); // optional, but needed for StaticWorldObject, Crate and Player entities
    Render::Init();         // render init must always come after the ui inited
    Async::Init();          // async init must always come after render init
    Audio::Init();

    // any kind of material or model loading must happen after both ui and render are inited
    Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(new Model(UID("errorstatic")));

    // load all of the language strings
    LoadText("data/lv.lang");

    // texture info stuff
    Material::LoadMaterialInfo("data/texture.list");

    // animations
    Animation mongusrun(UID("mongus"));
    Animation floppaidle(UID("turtle"));
    Animation bingusidle(UID("bingus_idle"));
    mongusrun.LoadFromDisk();
    floppaidle.LoadFromDisk();
    bingusidle.LoadFromDisk();
    
    Animation monster_animations (UID("creatures/moshkis"));
    monster_animations.LoadFromDisk();
    
    // audios
    Audio::Sound derp (UID("derp"));
    //derp.LoadFromDisk();

    // loading the demo level
    //auto demo = PoolProxy<WorldCell>::New();
    //demo->SetName(UID("demo"));
    //demo->LoadFromDisk();
    //demo->Load();
    //demo->SetInterior(true);
    //demo->SetInteriorLights(true);
    
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
    


    // create the player entity
    Player player;
    player.SetLocation(37.0f, 1.0f, -22.0f);
    player.Load();
    pler = &player;



    // create the mongus model
    RenderComponent* monguser = PoolProxy<RenderComponent>::New();
    monguser->SetModel(UID("creatures/moshkis"));
    monguser->SetPose(poseList.begin().ptr);
    monguser->Init();
    monguser->UpdateLocation(glm::vec3(37.0f, 1.0f, -22.0f));
    monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    LightComponent* lit = PoolProxy<LightComponent>::New();
    lit->Init();
    lit->UpdateColor(glm::vec3(1.0f, 0.0f, 1.0f));
    lit->UpdateDistance(100.0f);

    // create the animation player for the mongus model
    ArmatureComponent* monguser_armature = PoolProxy<ArmatureComponent>::New();
    monguser_armature->SetModel(UID("creatures/moshkis"));
    monguser_armature->Init();

    // link the mongus model and his animation player
    monguser->SetPose(monguser_armature->GetPosePtr());
    
    // turn on physics drawing
    //DRAW_PHYSICS_DEBUG = true;
    
    
    /*auto tolet_sprite = new Sprite;
    tolet_sprite->SetMaterial(Material::Find(UID("poland")));
    tolet_sprite->AutogenTiledFrames(0, 0, 40, 40, 6, 24, 15.0f, 1.0f);
    
    auto tolet_spinner = PoolProxy<SpriteComponent>::New();
    tolet_spinner->SetSprite(tolet_sprite);
    tolet_spinner->UpdateLocation(glm::vec3(0.0f, 1.2f, -2.0f));
    tolet_spinner->Init();
    tolet_spinner->Play();
    
    auto tolet_emitter = PoolProxy<ParticleComponent>::New();
    tolet_emitter->SetSprite(tolet_sprite);
    tolet_emitter->UpdateLocation(glm::vec3(0.0f, 1.2f, -2.0f));
    tolet_emitter->Init();*/
    
    //auto derp_player = PoolProxy<AudioComponent>::New();
    //derp_player->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    //derp_player->SetSound(&derp);
    //derp_player->SetRepeating(true);
    //derp_player->Init();
    
    //auto crate_ent = Entity::Find(UID("estijs"));
    
    
    
    
    
    KeyActionBindings[GLFW_KEY_R]  = KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ glm::vec3 ploc; pler->GetLocation(ploc); ploc += glm::vec3(0.0f, 3.0f, 0.0f); pler->SetLocation(ploc); }};
    
    
    
        
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

        // this will make the light spin
        lit->UpdateLocation(glm::vec3(cos(((float)tick) / 60.0f) * 5.0f, 0.01 ,sin(((float)tick) / 60.0f) * 5.0f));
        
        // this makes the mongus model bob up and down
        //monguser->UpdateLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.1f, 0.0f));
        
        
        GUI::Begin();
        GUI::DebugMenu();
        GUI::EscapeMenu();        
        GUI::End();
        
        Audio::Update();
        
        // this loads the models and textures into video memory
        Async::ResourceLoader2ndStage();
        Async::FinishResource();

        if(tick == 100){
            //derp_player->Play();
            monguser_armature->PlayAnimation(UID("MoshkisWalk"), -1, 1.0f, 1.0f);
        }
        

        //std::cout << crate_ent << std::endl;

        if (tick > 50) {
            //glm::vec3 crate_loc;
            //crate_ent->GetLocation(crate_loc);
            //derp_player->UpdateLocation(crate_loc);
        }


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
