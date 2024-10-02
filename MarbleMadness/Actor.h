//Actor.h

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject // derive from GraphObject
{
public:
    // Constructor of Actor
    Actor(StudentWorld* world, int imageID, int hp, double startX, double startY, int dir = none)
        : GraphObject(imageID, startX, startY, dir, 1.0), m_world(world), m_hp(hp) 
    {
        setVisible(true);
    }

    virtual void doSomething() {} // doSomething!
    virtual bool decHitPoints() { return false; }

    // Getter
    StudentWorld* getWorld() const { return m_world; }
    int getHitPoints() const { return m_hp; }

    //Setter
    void setHitPoints(int newhp) { m_hp = newhp; }

    // Attributes
    virtual bool isPushable() const { return false; }
    virtual bool isStealable() const { return false; }
    virtual bool canSteal() const { return false; }
    virtual bool isStolen() const { return false; }
    virtual bool canSpawn() const { return false; }

    // Visibility
    virtual bool playerVisibility() const { return false; }
    virtual bool robotVisibility() const { return false; }
    virtual bool marbleVisibility() const { return false; }
    virtual bool peaVisibility() const { return false; }

private:
    StudentWorld* m_world;
    int m_hp;
};

class Wall : public Actor
{
public:
    Wall(StudentWorld* world, double startX, double startY)
        : Actor(world, IID_WALL, 100, startX, startY) {}
};

class Player : public Actor
{
public:
    Player(StudentWorld* world, double startX, double startY)
        : Actor(world, IID_PLAYER, 20, startX, startY, right), m_peas(20) {}
    virtual void doSomething();
    virtual bool decHitPoints() 
    {
        setHitPoints(getHitPoints() - 2);
        if (getHitPoints() > 0) 
        {
            getWorld()->playSound(SOUND_PLAYER_IMPACT);
        }
        else //Player Died
        {
            getWorld()->playSound(SOUND_PLAYER_DIE);
        }
        return true;
    }
    void increasePeas(int peas) { m_peas += peas; } //Setter
    int getPeas() const { return m_peas; } //Getter

private:
    int m_peas;
    void shoot();
};

class Marble : public Actor 
{
public:
    Marble(StudentWorld* world, double startX, double startY)
        : Actor(world, IID_MARBLE, 10, startX, startY, 1) {}
    virtual bool decHitPoints() { setHitPoints(getHitPoints() - 2); return true; }
    virtual bool isPushable() const { return true; }
};

class Pea : public Actor 
{
public:
    Pea(StudentWorld* world, double startX, double startY, int dir)
        : Actor(world, IID_PEA, 100, startX, startY, dir), m_moved(false) {}
    virtual void doSomething();
    virtual bool peaVisibility() const { return true; }
    virtual bool playerVisibility() const { return true; }
    virtual bool marbleVisibility() const { return true; }
    virtual bool robotVisibility() const { return true; }
private:
    bool m_moved;
};


class Enemy : public Actor // Enemy: RageBot, ThiefBot
{
public:
    Enemy(StudentWorld* world, int imageID, int hp, double startX, double startY, int dir)
        : Actor(world, imageID, hp, startX, startY, dir) {}
    virtual void shoot();
};

class RageBot : public Enemy 
{
public:
    RageBot(StudentWorld* world, double startX, double startY, int dir)
        : Enemy(world, IID_RAGEBOT, 10, startX, startY, dir) {}
    virtual void doSomething();
    virtual bool decHitPoints();
};

class ThiefBot : public Enemy 
{
public:
    ThiefBot(StudentWorld* world, int imageID, int hp, double startX, double startY, bool isMean)
        : Enemy(world, imageID, hp, startX, startY, right), m_mean(isMean), m_stolenGoodies(nullptr), m_spacesMoved(0) 
    {
        m_distanceBeforeTurning = (rand() % 6) + 1;
    }
    virtual void doSomething();
    virtual bool decHitPoints();
    virtual bool canSteal() const { return true; }

    void stealGoodie(Actor* toSteal);
    void turn();

    int spacesMoved() const { return m_spacesMoved; } //Getter
    int distanceBeforeTurning() const { return m_distanceBeforeTurning; }
    Actor* stolenGoods() { return m_stolenGoodies; }

    void setSpacesMoved(int spaces) { m_spacesMoved = spaces; } //Setter
    void setDistanceBeforeTurning(int newDist) { m_distanceBeforeTurning = newDist; }

private:
    bool m_mean; //mean or regular thief bot
    int m_distanceBeforeTurning;
    int m_spacesMoved;
    Actor* m_stolenGoodies;
};


class Pit : public Actor 
{
public:
    Pit(StudentWorld* world, double startX, double startY)
        : Actor(world, IID_PIT, 100, startX, startY) {}
    virtual void doSomething();
    virtual bool marbleVisibility() const { return true; }
    virtual bool peaVisibility() const { return true; }
};

class Exit : public Actor 
{
public:
    Exit(StudentWorld* world, double startX, double startY)
        : Actor(world, IID_EXIT, 100, startX, startY), m_opened(false) 
    {
        setVisible(false);
    }
    virtual void doSomething();
    virtual bool playerVisibility() const { return true; }
    virtual bool peaVisibility() const { return true; }
    virtual bool robotVisibility() const { return true; }
private:
    bool m_opened;
};

class PickupableItem : public Actor //PickableItem: ExtraHealthGoodie, AmmoGoodie, RestoreHealthGoodie, Crystal
{
public:
    PickupableItem(StudentWorld* world, int imageID, double startX, double startY)
        : Actor(world, imageID, 100, startX, startY), m_stolen(false) {}
    
    virtual void doSomething();
    virtual bool playerVisibility() const { return true; }
    virtual bool peaVisibility() const { return true; }
    virtual bool robotVisibility() const { return true; }
    virtual bool isStolen() const { return m_stolen; }

private:
    virtual void pickUp() = 0;
    bool m_stolen;
};

class Crystal : public PickupableItem 
{
public:
    Crystal(StudentWorld* world, double startX, double startY)
        : PickupableItem(world, IID_CRYSTAL, startX, startY) {}
private:
    virtual void pickUp();
};

class ExtraLifeGoodie : public PickupableItem 
{
public:
    ExtraLifeGoodie(StudentWorld* world, double startX, double startY)
        : PickupableItem(world, IID_EXTRA_LIFE, startX, startY) {}
    virtual bool isStealable() const { return true; }
private:
    virtual void pickUp();
};

class RestoreHealthGoodie : public PickupableItem 
{
public:
    RestoreHealthGoodie(StudentWorld* world, double startX, double startY)
        : PickupableItem(world, IID_RESTORE_HEALTH, startX, startY) {}
    virtual bool isStealable() const { return true; }
private:
    virtual void pickUp();
};

class AmmoGoodie : public PickupableItem 
{
public:
    AmmoGoodie(StudentWorld* world, double startX, double startY)
        : PickupableItem(world, IID_AMMO, startX, startY) {}
    virtual bool isStealable() const { return true; }
private:
    virtual void pickUp();
};

class ThiefBotFactory : public Actor 
{
public:
    ThiefBotFactory(StudentWorld* world, double startX, double startY, bool isMean)
        : Actor(world, IID_ROBOT_FACTORY, 100, startX, startY), m_mean(isMean) {}
    virtual void doSomething();
    virtual bool canSpawn() { return true; } //spawn in thiefbots
private:
    bool m_mean; //mean or regular thiefbot factory
};



#endif // ACTOR_H_