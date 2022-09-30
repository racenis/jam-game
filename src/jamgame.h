#ifndef MOSHKIS_H
#define MOSHKIS_H

#include <core.h>

namespace Core {
    class Player;
}

extern Core::Player* main_player;

void PlayerPickedUpPickup(Core::name_t pickup_model);

void PlayerGotHitInFace(uint64_t oof_size);

#endif // MOSHKIS_H
