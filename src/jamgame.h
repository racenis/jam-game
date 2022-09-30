#ifndef JAMGAME_H
#define JAMGAME_H

#include <core.h>

namespace Core {
    class Player;
}

extern Core::Player* main_player;

void PlayerPickedUpPickup(Core::name_t pickup_model);

void PlayerGotHitInFace(uint64_t oof_size);

enum soundeffect {
    SOUND_MOSHKIS_HIT,
    SOUND_MOSHKIS_POP,
    SOUND_MOSHKIS_ATTACK,
    SOUND_RIFLE_FIRE,
    SOUND_STAPLER_FIRE,
    SOUND_PICKUP_PICKUP,
    SOUND_LAST_SOUND
};

void LoadSoundEffects();

void PlaySoundEffect(soundeffect sound, glm::vec3 position);

#endif // JAMGAME_H
