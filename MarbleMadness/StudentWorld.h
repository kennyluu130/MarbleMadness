//StudentWorld.h

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"


#include <string>
#include <sstream>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

//Classes that are needed!
class Actor;
class Player;
class Pea;
class Enemy;
class RageBot;
class ThiefBot;

class StudentWorld : public GameWorld 
{
public:
    //constructor
    StudentWorld(std::string assetPath)
        : GameWorld(assetPath), m_player(nullptr), m_ticks(0), m_bonus(1000), m_crystals(0), m_levelComplete(false) {}

    ~StudentWorld() { cleanUp(); } //destructor

    // Required
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    // Setters
    void addActor(Actor* actor) { m_actors.push_back(actor); }
    void decCrystals() { m_crystals--; }
    void setComplete(bool complete) { m_levelComplete = complete; }
    void setDisplayText();

    // Getters
    int getCrystals() const { return m_crystals; }
    int getTicks() const { return m_ticks; }
    Player* player() const { return m_player; }
    int countThiefBots(double x, double y) const;

    // Coordinate functions
    void targetCoordinates(double& x, double& y, int dir) const;
    Actor* actorAtCoordinates(Actor* caller, double x, double y) const;

    // Can helper functions
    bool canShootAtPlayer(Enemy* bot) const;
    bool enemyCanMoveHere(double x, double y) const;
    Actor* canStealGoodie(ThiefBot* bot) const;

    // Helpers
    void movePlayer();
    void moveRageBot(RageBot* bot);
    void moveThiefBot(ThiefBot* bot);
    void movePea(Pea* pea);

private:
    int m_ticks;
    int m_bonus;
    int m_crystals;
    bool m_levelComplete;

    Player* m_player;
    std::list<Actor*> m_actors; //using a list to keep track of actors
};

#endif // STUDENTWORLD_H_