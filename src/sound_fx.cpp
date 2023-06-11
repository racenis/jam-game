// This file contains stuff for playing sound effects.

#include <audio/audio.h>
#include <components/audiocomponent.h>
#include "jamgame.h"

#include <vector>

using namespace tram;

Audio::Sound* sounds[SOUND_LAST_SOUND] = {nullptr};
AudioComponent* players[SOUND_LAST_SOUND] = {nullptr};

std::vector<AudioComponent*> audiocomponent_list;

void LoadSoundEffects() {
    sounds[SOUND_MOSHKIS_HIT] =     Audio::Sound::Find(UID("moshkis_hit"));
    sounds[SOUND_MOSHKIS_POP] =     Audio::Sound::Find(UID("moshkis_pop"));
    sounds[SOUND_MOSHKIS_ATTACK] =  Audio::Sound::Find(UID("moshkis_attack"));
    sounds[SOUND_RIFLE_FIRE] =      Audio::Sound::Find(UID("rifle_fire"));
    sounds[SOUND_STAPLER_FIRE] =    Audio::Sound::Find(UID("stapler_fire"));
    sounds[SOUND_PICKUP_CLICK] =    Audio::Sound::Find(UID("pickup_click"));
    sounds[SOUND_PICKUP_PICKUP] =   Audio::Sound::Find(UID("pickup_pickup"));
    
    for (size_t i = 0; i < SOUND_LAST_SOUND; i++) sounds[i]->Load();
    
    for (size_t i = 0; i < SOUND_LAST_SOUND; i++) {
        players[i] = PoolProxy<AudioComponent>::New();
        players[i]->SetLocation(glm::vec3(0.0f, 0.0f, 0.0f));
        players[i]->SetRepeating(false);
        players[i]->SetSound(sounds[i]->GetName());
        players[i]->Init();
    }
}

void PlaySoundEffect(soundeffect sound, glm::vec3 position) {
    players[sound]->SetLocation(position);
    players[sound]->Play();
}
