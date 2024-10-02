//Actor.cpp


#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//Player Implementations
void Player::doSomething() 
{
    if (getHitPoints() <= 0) return; // do nothing if dead

    int dir; // get user input
    if (getWorld()->getKey(dir))
    {
        switch (dir) {
        case KEY_PRESS_UP:
            dir = up;
            break;
        case KEY_PRESS_DOWN:
            dir = down;
            break;
        case KEY_PRESS_LEFT:
            dir = left;
            break;
        case KEY_PRESS_RIGHT:
            dir = right;
            break;
        case KEY_PRESS_SPACE:
            shoot();
            return;
        case KEY_PRESS_ESCAPE:
            setHitPoints(0);
            return;
        default:
            return;
        }
        setDirection(dir); //change direction
        getWorld()->movePlayer(); //attempt to move player
    }
}

void Player::shoot() 
{
    if (m_peas > 0) //check if player has peas
    {
        double x = getX();
        double y = getY();

        getWorld()->targetCoordinates(x, y, getDirection());
        getWorld()->addActor(new Pea(getWorld(), x, y, getDirection())); // create Pea
        getWorld()->playSound(SOUND_PLAYER_FIRE);
        m_peas--; //decrement
    }
}

// Pea Implementations
void Pea::doSomething() 
{
    if (getHitPoints() <= 0) return; // do nothing if dead
    if (!m_moved) // do not move if just spawned
    {
        m_moved = true;
        return;
    }
    getWorld()->movePea(this); // attempt to move pea
}

// Pit Implementation
void Pit::doSomething() 
{
    if (getHitPoints() <= 0) return; // if dead do nothing
    Actor* actor = getWorld()->actorAtCoordinates(this, getX(), getY()); // find a marble and swallow
    if (actor != nullptr && actor->isPushable()) {
        setHitPoints(0);
        actor->setHitPoints(0);
        setVisible(false); //pit no longer visible
        actor->setVisible(false); //marble no longer visible
    }
}

// Exit Implementation
void Exit::doSomething() 
{
    if (getWorld()->getCrystals() == 0) // update visibility if all crystals are collected
    {
        setVisible(true);
        m_opened = true;
    }

    if (m_opened && getWorld()->player()->getX() == getX() && getWorld()->player()->getY() == getY()) //player on exit and exit is open
    {
        getWorld()->increaseScore(2000);
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->setComplete(true);
    }
}

// Pickup Implementations
void PickupableItem::doSomething() 
{
    if (getHitPoints() <= 0 || isStolen()) return; //dead or not visible
    if (getWorld()->player()->getX() == getX() && getWorld()->player()->getY() == getY()) //player on spot
    {
        setHitPoints(0);
        setVisible(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        pickUp();
    }
}

// Crystal Implementation
void Crystal::pickUp() 
{
    getWorld()->increaseScore(50);
    getWorld()->decCrystals();
}

// ExtraLife Implementation
void ExtraLifeGoodie::pickUp() 
{
    getWorld()->increaseScore(1000);
    getWorld()->incLives();
}

// RestoreHealth Implementation
void RestoreHealthGoodie::pickUp() 
{
    getWorld()->increaseScore(500);
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->player()->setHitPoints(20);
}

// Ammo Implementation
void AmmoGoodie::pickUp() 
{
    getWorld()->increaseScore(100);
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->player()->increasePeas(20);
}

// Enemy Implementation
void Enemy::shoot() 
{
    double x = getX();
    double y = getY();
    getWorld()->targetCoordinates(x, y, getDirection());
    getWorld()->addActor(new Pea(getWorld(), x, y, getDirection()));
    getWorld()->playSound(SOUND_ENEMY_FIRE);
}

// RageBot Implementation
void RageBot::doSomething() 
{
    if (getHitPoints() <= 0) return; //do nothing if dead
    int ticks = (28 - getWorld()->getLevel()) / 4; //ticks to wait
    if (ticks < 3) { ticks = 3; }
    if (getWorld()->getTicks() % ticks == 0) 
    {
        if (getWorld()->canShootAtPlayer(this)) {shoot();}
        else {getWorld()->moveRageBot(this);}
    }
}

bool RageBot::decHitPoints() 
{
    setHitPoints(getHitPoints() - 2);
    if (getHitPoints() > 0) //if still alive
    {
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
    else 
    {
        setVisible(false); //if dead
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(100);
    }
    return true;
}

// ThiefBot Implementations
void ThiefBot::doSomething() 
{
    if (getHitPoints() <= 0) return; // do nothing if dead
    int ticks = (28 - getWorld()->getLevel()) / 4; // ticks to wait
    if (ticks < 3) ticks = 3;

    if (getWorld()->getTicks() % ticks == 0) 
    {
        Actor* toSteal = getWorld()->canStealGoodie(this);
        if (m_mean && getWorld()->canShootAtPlayer(this)) {shoot();} //if meanThiefBot then shoot at player if possible
        else if (toSteal != nullptr) {stealGoodie(toSteal);} //steal goodie if can
        else {getWorld()->moveThiefBot(this);}
    }
}

bool ThiefBot::decHitPoints() 
{
    setHitPoints(getHitPoints() - 2);
    if (getHitPoints() > 0) {getWorld()->playSound(SOUND_ROBOT_IMPACT);} //if still alive
    else //if dead
    {
        setVisible(false);
        getWorld()->playSound(SOUND_ROBOT_DIE);
        if (m_mean) //mean thiefbot score
        {
            getWorld()->increaseScore(20);
        }
        else //regular thiefbot score
        {
            getWorld()->increaseScore(10);
        }
        if (stolenGoods() != nullptr) // drop stolen goods on square if thief has goods
        {
            stolenGoods()->moveTo(getX(), getY()); //move goodie to spot of thiefbot
            stolenGoods()->setVisible(true);
        }
    }
    return true;
}

void ThiefBot::stealGoodie(Actor* toSteal) 
{
    if (toSteal != nullptr) 
    {
        if (rand() % 10 == 0) //chance to steal
        {
            toSteal->setVisible(false);
            m_stolenGoodies = toSteal; //steal goodie
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
        }
    }
}

void ThiefBot::turn() 
{
    int directions[] = { up, down, left, right }; //directions list

    double x = getX();
    double y = getY();

    int index = rand() % 4; //randomize direction
    setDirection(directions[index]);

    for (int i = 1; i <= 4; i++) //set random direction
    {
        getWorld()->targetCoordinates(x, y, directions[index]);
        if (getWorld()->enemyCanMoveHere(x, y)) 
        {
            setDirection(directions[index]);
            moveTo(x, y);
            return;
        }
        index = (index + 1) % 4;
    }
}

// ThiefBotFactory Implementations
void ThiefBotFactory::doSomething() 
{
    if (getWorld()->countThiefBots(getX(), getY()) < 3) //less than 3 thiefbots
    {
        if (rand() % 50 == 0) 
        {
            if (m_mean) //mean factory
            {
                getWorld()->addActor(new ThiefBot(getWorld(), IID_MEAN_THIEFBOT, 5, getX(), getY(), true));
            }
            else //regular factory
            {
                getWorld()->addActor(new ThiefBot(getWorld(), IID_THIEFBOT, 8, getX(), getY(), false));
            }
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
}