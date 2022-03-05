#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include "Framework.h"

class GameSystem : public std::enable_shared_from_this<GameSystem> {
public:
    virtual void update() = 0;
    virtual ~GameSystem() {}
};

#endif // GAME_SYSTEM_H
