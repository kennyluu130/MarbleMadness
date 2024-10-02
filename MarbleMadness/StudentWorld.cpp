//StudentWorld.cpp


#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
using namespace std;

//create world
GameWorld* createStudentWorld(string assetPath) {
    return new StudentWorld(assetPath);
}

//Required Implementations
int StudentWorld::init() 
{
    //initialize member variables
    m_ticks = 0;
    m_bonus = 1000;
    m_levelComplete = false;
    m_crystals = 0;

    // format levelPath string
    string currentLevel;
    if (getLevel() <= 9)
    {
        currentLevel = "level0" + to_string(getLevel()) + ".txt";
    }
    else
    {
        currentLevel = "level" + to_string(getLevel()) + ".txt";
    }

    // creating level
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(currentLevel);

    if (result == Level::load_fail_file_not_found) //not found
    {
        cerr << "Could not find " + currentLevel + " data file" << endl;
    }
    else if (result == Level::load_fail_bad_format) //bad format
    {
        cerr << "Improperly formatted" << endl;
    }
    else if (result == Level::load_success) 
    {
        // create actors by square
        for (int x = 0; x < VIEW_WIDTH; x++) 
        {
            for (int y = 0; y < VIEW_HEIGHT; y++) 
            {
                Level::MazeEntry ge = lev.getContentsOf(x, y);
                switch (ge) 
                {
                case Level::empty:
                    break;
                case Level::player:
                    m_player = new Player(this, x, y);
                    m_actors.push_back(m_player);
                    break;
                case Level::wall:
                    m_actors.push_back(new Wall(this, x, y));
                    break;
                case Level::marble:
                    m_actors.push_back(new Marble(this, x, y));
                    break;
                case Level::pit:
                    m_actors.push_back(new Pit(this, x, y));
                    break;
                case Level::crystal:
                    m_actors.push_back(new Crystal(this, x, y));
                    m_crystals++;
                    break;
                case Level::exit:
                    m_actors.push_back(new Exit(this, x, y));
                    break;
                case Level::extra_life:
                    m_actors.push_back(new ExtraLifeGoodie(this, x, y));
                    break;
                case Level::restore_health:
                    m_actors.push_back(new RestoreHealthGoodie(this, x, y));
                    break;
                case Level::ammo:
                    m_actors.push_back(new AmmoGoodie(this, x, y));
                    break;
                case Level::vert_ragebot:
                    m_actors.push_back(new RageBot(this, x, y, GraphObject::up));
                    break;
                case Level::horiz_ragebot:
                    m_actors.push_back(new RageBot(this, x, y, GraphObject::right));
                    break;
                case Level::thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(this, x, y, false));
                    break;
                case Level::mean_thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(this, x, y, true));
                    break;
                default:
                    break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

//side note: why is every level after 1 so hard

int StudentWorld::move() 
{
    for (auto it = m_actors.begin(); !m_levelComplete && it != m_actors.end(); it++) //call actors' do something
    {
        (*it)->doSomething();
        if (m_player->getHitPoints() <= 0) //player died
        {
            playSound(SOUND_PLAYER_DIE);
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    for (auto it = m_actors.begin(); it != m_actors.end();)  // remove dead actors
    {
        if ((*it)->getHitPoints() <= 0) 
        {
            delete(*it);
            it = m_actors.erase(it);
        }
        else {it++;}
    }
    if (m_levelComplete)  // check to see if level was completed
    {
        increaseScore(m_bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }

    if (m_bonus > 0) { m_bonus--; } //update bonus
    m_ticks++;
    setDisplayText();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() 
{
    for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
    {
        delete* it; //delete all actors
    }
    m_actors.clear(); //clear list
}

// Helper Implementations
void StudentWorld::setDisplayText() 
{
    ostringstream displayText;
    displayText.fill('0');
    displayText << "Score: " << setw(7) << getScore() << "  ";
    displayText << "Level: " << setw(2) << getLevel() << "  ";
    displayText.fill(' ');
    displayText << "Lives: " << setw(2) << getLives() << "  ";
    displayText << "Health: " << setw(3) << round(m_player->getHitPoints() / 0.2) << "%  ";
    displayText << "Ammo: " << setw(2) << m_player->getPeas() << "  ";
    displayText << "Bonus: " << setw(4) << m_bonus;
    setGameStatText(displayText.str());
}

void StudentWorld::targetCoordinates(double& x, double& y, int dir) const //get coordinates
{
    switch (dir) {
    case GraphObject::up:
        y += 1;
        break;
    case GraphObject::down:
        y -= 1;
        break;
    case GraphObject::right:
        x += 1;
        break;
    case GraphObject::left:
        x -= 1;
        break;
    }
}

void StudentWorld::movePlayer() 
{
    // get target coordinates
    double x = m_player->getX();
    double y = m_player->getY();
    targetCoordinates(x, y, m_player->getDirection());

    for (auto it = m_actors.begin(); it != m_actors.end(); it++) // iterate through all actors
    {
        if ((*it)->getX() == x && (*it)->getY() == y)  // if an actor is here 
        {
            if ((*it)->playerVisibility()) continue; // if actor is transparent to players

            if ((*it)->isPushable()) // if actor is pushable
            {
                // get coords of space to push to
                double nextX = x;
                double nextY = y;
                targetCoordinates(nextX, nextY, m_player->getDirection());
                Actor* actorAtTarget = actorAtCoordinates(nullptr, nextX, nextY);
                if (actorAtTarget == nullptr || actorAtTarget->marbleVisibility()) // if nothing or transparent marble at target
                {
                    (*it)->moveTo(nextX, nextY);
                    m_player->moveTo(x, y);
                }
            }
            return;
        }
    }
    m_player->moveTo(x, y); //move player
}

void StudentWorld::movePea(Pea* pea) 
{
    // get current location
    double x = pea->getX();
    double y = pea->getY();

    for (auto it = m_actors.begin(); it != m_actors.end(); it++) //for all actors
    {
        if ((*it)->getX() == x && (*it)->getY() == y) 
        {
            if (!(*it)->peaVisibility()) //hit an actor
            {
                pea->setHitPoints(0);
                pea->setVisible(false);
                if ((*it)->canSpawn()) //if factory, don't do anything
                {
                    continue;
                }
                (*it)->decHitPoints();
                return;
            }
        }
    }

    targetCoordinates(x, y, pea->getDirection());
    pea->moveTo(x, y);

    for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
    {
        if ((*it)->getX() == x && (*it)->getY() == y) 
        {
            if (!(*it)->peaVisibility()) 
            {
                pea->setHitPoints(0);
                pea->setVisible(false);
                
                if ((*it)->canSpawn()) //if factory
                {
                    continue;
                }
                (*it)->decHitPoints();
                return;
            }
        }
    }
}

bool StudentWorld::enemyCanMoveHere(double x, double y) const 
{
    for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
    {
        if ((*it)->getX() == x && (*it)->getY() == y) // actor is here
        {
            if ((*it)->robotVisibility()) // actor is transparent to robots
            {
                continue;
            }
            else 
            {
                return false;
            }
        }
    }
    return true;
}

void StudentWorld::moveRageBot(RageBot* robot) 
{
    double x = robot->getX();
    double y = robot->getY();
    targetCoordinates(x, y, robot->getDirection());

    if (enemyCanMoveHere(x, y)) 
    {
        robot->moveTo(x, y);
    }
    else 
    {
        robot->setDirection((robot->getDirection() + 180) % 360);
    }
}

void StudentWorld::moveThiefBot(ThiefBot* robot) 
{
    if (robot->spacesMoved() >= robot->distanceBeforeTurning()) 
    {
        robot->turn();
        return;
    }
    double x = robot->getX();
    double y = robot->getY();

    targetCoordinates(x, y, robot->getDirection());

    if (enemyCanMoveHere(x, y)) {robot->moveTo(x, y);}
    else {robot->turn();}
}

bool StudentWorld::canShootAtPlayer(Enemy* robot) const 
{
    double x = robot->getX();
    double y = robot->getY();

    int lower, upper;
    switch (robot->getDirection()) 
    {
    case GraphObject::left:
        lower = m_player->getX();
        upper = x;
        if (y != m_player->getY() || x < lower) { return false; }
        for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
        {
            if ((*it)->getY() == y && (*it)->getX() > lower && (*it)->getX() < upper && !(*it)->peaVisibility()) 
            {
                return false;
            }
        }
        break;
    case GraphObject::right:
        lower = x;
        upper = m_player->getX();
        if (y != m_player->getY() || x > upper) return false;
        for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
        {
            if ((*it)->getY() == y && (*it)->getX() > lower && (*it)->getX() < upper && !(*it)->peaVisibility()) 
            {
                return false;
            }
        }
        break;
    case GraphObject::up:
        lower = y;
        upper = m_player->getY();
        if (x != m_player->getX() || y > upper) return false;
        Actor* cur;
        for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
        {
            cur = *it;
            if ((*it)->getX() == x && (*it)->getY() > lower && (*it)->getY() < upper && !(*it)->peaVisibility()) 
            {
                return false;
            }
        }
        break;
    case GraphObject::down:
        lower = m_player->getY();
        upper = y;
        if (x != m_player->getX() || y < lower) return false;
        for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
        {
            cur = *it;
            if ((*it)->getX() == x && (*it)->getY() > lower && (*it)->getY() < upper && !(*it)->peaVisibility()) 
            {
                return false;
            }
        }
    }
    return true;
}

Actor* StudentWorld::canStealGoodie(ThiefBot* bot) const 
{
    double x = bot->getX();
    double y = bot->getY();


    for (auto it = m_actors.begin(); it != m_actors.end(); it++) //loop through until stealable
    {
        if ((*it) != bot && (*it)->getX() == x && (*it)->getY() == y && (*it)->isStealable() && (*it)->isVisible()) 
        {
            return *it;
        }
    }
    return nullptr; //nothing stealable
}

int StudentWorld::countThiefBots(double x, double y) const 
{
    int count = 0;
    for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
    {
        if ((*it)->canSteal() && abs(x - (*it)->getX()) <= 3 && abs(y - (*it)->getY()) <= 3) 
        {
            count++;
        }
        if ((*it)->canSteal() && (*it)->getX() == x && (*it)->getY() == y) 
        {
            return 4;
        }
    }
    return count;
}

Actor* StudentWorld::actorAtCoordinates(Actor* caller, double x, double y) const 
{
    for (auto it = m_actors.begin(); it != m_actors.end(); it++) 
    {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != caller) 
        {
            return *it;
        }
    }
    return nullptr;
}