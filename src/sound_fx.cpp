// This file contains stuff for playing sound effects.

#include <audio.h>
#include <components/audiocomponent.h>
#include "jamgame.h"

using namespace Core;

Audio::Sound* sounds[SOUND_LAST_SOUND] = {nullptr};
AudioComponent* players[SOUND_LAST_SOUND] = {nullptr};

std::vector<AudioComponent*> audiocomponent_list;

void LoadSoundEffects() {
    sounds[SOUND_MOSHKIS_HIT] =     new Audio::Sound(UID("moshkis_hit"));
    sounds[SOUND_MOSHKIS_POP] =     new Audio::Sound(UID("moshkis_pop"));
    sounds[SOUND_MOSHKIS_ATTACK] =  new Audio::Sound(UID("moshkis_attack"));
    sounds[SOUND_RIFLE_FIRE] =      new Audio::Sound(UID("rifle_fire"));
    sounds[SOUND_STAPLER_FIRE] =    new Audio::Sound(UID("stapler_fire"));
    sounds[SOUND_PICKUP_CLICK] =    new Audio::Sound(UID("pickup_click"));
    sounds[SOUND_PICKUP_PICKUP] =   new Audio::Sound(UID("pickup_pickup"));
    
    for (size_t i = 0; i < SOUND_LAST_SOUND; i++) sounds[i]->Load();
    
    for (size_t i = 0; i < SOUND_LAST_SOUND; i++) {
        players[i] = PoolProxy<AudioComponent>::New();
        players[i]->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
        players[i]->SetRepeating(false);
        players[i]->SetSound(sounds[i]);
        players[i]->Init();
    }
}

void PlaySoundEffect(soundeffect sound, glm::vec3 position) {
    players[sound]->UpdateLocation(position);
    players[sound]->Play();
}
