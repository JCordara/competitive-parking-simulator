#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

class GameSystem {
public:
    virtual void update() = 0;
    virtual ~GameSystem() {}
};

#endif // GAME_SYSTEM_H
