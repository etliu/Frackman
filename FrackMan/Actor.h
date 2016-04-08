// Possible interfaces for actors.  You may use all, some, or none
// of this, as you wish.

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction startDir,
          float size = 1.0, int depth = 0);
    
    virtual ~Actor();
    
    //move actor in direction dir
    void move(Direction dir);
    
    // Is this actor alive?
    bool isAlive() const;
    
    // Mark this actor as dead.
    void setDead();
    
    // Annoy this actor.
    virtual bool annoy(unsigned int amt) { return true; };
    
    // Get this actor's world
    StudentWorld* getWorld() const;
    
    // Can other actors pass through this actor?
    virtual bool canActorsPassThroughMe() const { return true; };
    
    virtual bool canBeAnnoyed() const { return false; };
    // Can this actor dig through dirt?
    virtual bool canDigThroughDirt() const { return false; };
    
    // Does this actor hunt the FrackMan?
    virtual bool huntsFrackMan() const { return false; };
    
    // Do you need to collect this actor to win the game?
    virtual bool isWinCondition() const { return false; };
    
    virtual void doSomething() = 0;
    
    virtual void addGold() { };
    
    virtual bool annoysFrackman() { return false; };
    
private:
    StudentWorld* wld;
    bool m_isAlive;
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* world, int imageID, int startX, int startY, Direction startDir,
          unsigned int hitPoints);
    virtual ~Agent();
    
    // Pick up a gold nugget.
    virtual void addGold() = 0;
    
    // How many hit points does this actor have left?
    unsigned int getHitPoints() const;
    
    virtual void doSomething() = 0;
    virtual bool annoy(unsigned int amount);
    
    virtual bool canBeAnnoyed() const { return true; };
private:
    int m_health;
};

class FrackMan : public Agent
{
public:
    FrackMan(StudentWorld* world);
    virtual ~FrackMan();
    virtual void doSomething();
    virtual bool annoy(unsigned int amount);
    
    virtual bool canDigThroughDirt() const { return true; };
    
    // Pick up a gold nugget
    virtual void addGold() { m_nNuggets++; };
    
    // Pick up a sonar kit.
    void addSonar() { m_nCharges++; };
    
    // Pick up water.
    void addWater() { m_uWater+=5; };
    
    // Get amount of gold
    unsigned int getGold() const { return m_nNuggets; };
    
    // Get amount of sonar charges
    unsigned int getSonar() const { return m_nCharges; };
    
    // Get amount of water
    unsigned int getWater() const { return m_uWater; };
    
private:
    int m_nCharges;
    int m_nNuggets;
    int m_uWater;
};

class Protester : public Agent
{
public:
    Protester(StudentWorld* world, int imageID,
              unsigned int hitPoints, unsigned int score);
    virtual ~Protester();
    virtual void doSomething();
    virtual bool annoy(unsigned int amount);
    virtual void addGold() = 0;
    
    virtual bool huntsFrackMan() const { return true; };
    
    void setTicksToNextMove(int n);
    
    void setInactive() { isActive = false; };
    bool isActiveP() const { return isActive; };
    
    bool isWaiting();
private:
    bool wasPTurn(Direction di, Direction df);
    bool isActive;
    int score;
    int ticksToWait;
    int curWait;
    int numMovesInCurDir;
    int shoutWait;
    int ticksSincePTurn;
};

class RegularProtester : public Protester
{
public:
    RegularProtester(StudentWorld* world);
    virtual ~RegularProtester();
    virtual void doSomething();
    virtual void addGold();
};

class HardcoreProtester : public Protester
{
public:
    HardcoreProtester(StudentWorld* world);
    virtual ~HardcoreProtester();
    virtual void doSomething();
    virtual void addGold();
};

class Dirt : public Actor
{
public:
    Dirt(StudentWorld* world, int startX, int startY);
    virtual ~Dirt();
    virtual void doSomething() {};
};

class Boulder : public Actor
{
public:
    Boulder(StudentWorld* world, int startX, int startY);
    virtual ~Boulder();
    virtual void doSomething();
    virtual bool canActorsPassThroughMe() const { return false; };
    virtual bool annoysFrackman() { return true; };
private:
    bool m_isWaiting;
    int m_waitCount;
};

class Squirt : public Actor
{
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction startDir);
    virtual ~Squirt();
    virtual void doSomething();
private:
    int travelDistance;
};

class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
                     int soundToPlay, int ptValue);
    virtual ~ActivatingObject();
    virtual void doSomething();
    virtual bool isWinCondition() const { return false; };
    // Set number of ticks until this object dies
    void setTicksToLive(int n);
    //returns lifetime reminaing
    int getRemainingLifetime();
    
    //checks if object is activated. if so, performs appropriate actions on object
    //returns true if object was activated by frackman
    bool ActivateIfNecessary();
    
    
    
private:
    int ptValue;
    int remainingLifetime;
    int sound;
};

class OilBarrel : public ActivatingObject
{
public:
    OilBarrel(StudentWorld* world, int startX, int startY);
    virtual ~OilBarrel();
    virtual bool isWinCondition() const { return true; };
};

class GoldNugget : public ActivatingObject
{
public:
    GoldNugget(StudentWorld* world, int startX, int startY, bool temporary);
    virtual ~GoldNugget();
    virtual void doSomething();
private:
    bool wasDropped;
};

class SonarKit : public ActivatingObject
{
public:
    SonarKit(StudentWorld* world);
    virtual ~SonarKit();
    virtual void doSomething();
};

class WaterPool : public ActivatingObject
{
public:
    WaterPool(StudentWorld* world, int startX, int startY);
    virtual ~WaterPool();
    virtual void doSomething();
};

#endif // ACTOR_H_