// This file contains random function and global variable definitions.

#ifndef JAMGAME_H
#define JAMGAME_H

#include <framework/core.h>
#include <framework/uid.h>
#include <framework/math.h>

namespace tram {
    class Player;
}

extern tram::Player* main_player;

void PlayerPickedUpPickup(tram::name_t pickup_model);

void PlayerGotHitInFace(uint64_t oof_size);

enum soundeffect {
    SOUND_MOSHKIS_HIT,
    SOUND_MOSHKIS_POP,
    SOUND_MOSHKIS_ATTACK,
    SOUND_RIFLE_FIRE,
    SOUND_STAPLER_FIRE,
    SOUND_PICKUP_CLICK,
    SOUND_PICKUP_PICKUP,
    SOUND_LAST_SOUND
};

void LoadSoundEffects();

void PlaySoundEffect(soundeffect sound, glm::vec3 position);

#endif // JAMGAME_H
