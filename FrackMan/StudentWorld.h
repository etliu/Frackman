#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GraphObject.h"
#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>

int randInt(int min, int max);

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Add an actor to the world.
    void addActor(Actor* a);
    
    // Clear a 4x4 region of dirt.
    bool displaceDirt(int x, int y);
    
    //return true if coordinate (x,y) has dirt.
    bool dirtExists(int x, int y) const;
    
    // Can actor move to x,y?
    bool canActorMove(Actor* a, GraphObject::Direction dir) const;
    
    // Annoy all other actors within radius of annoyer.
    // Returns true if at least one actor was annoyed.
    bool annoyAllNearbyActors(Actor* annoyer, int amount, int radius);
    
    // Reveal all objects within radius of x,y.
    void revealAllNearbyObjects(int x, int y, int radius);
    
    // If the FrackMan is within radius of a, return a pointer to the
    // FrackMan, otherwise null.
    FrackMan* findNearbyFrackMan(Actor* a, int radius) const;
    
    // If at least one actor that can pick things up is within radius of a,
    // return a pointer to one of them, otherwise null.
    Actor* findNearbyProtester(Actor* a, int radius) const;
    
    // Give FrackMan some sonar charges.
    void giveFrackManSonar();
    
    // Give FrackMan some water.
    void giveFrackManWater();
    
    //Give Frackman a gold nugget.
    void giveFrackManGold();
    
    // If the Actor a has a clear line of sight to the FrackMan, return
    // the direction to the FrackMan, otherwise GraphObject::none.
    GraphObject::Direction lineOfSightToFrackMan(Actor* a) const;
    
    bool isFacingFrackMan(Actor* a) const;
    
    // Return whether the Actor a is within radius of FrackMan.
    bool isNearFrackMan(Actor* a, int radius) const;
    
    //Determine the direction of the first move a quitting protester
    //makes to leave the oil field.
    GraphObject::Direction determineFirstMoveToExit(Actor* a, int x, int y);
    
    //Determine the direction of the first move a hardcore protester
    //makes to approach the FrackMan.
    GraphObject::Direction determineFirstMoveToFrackMan(Actor* a,int x, int y);
    
    int curHeatVal(int x, int y) const;
    void updateFrackmanMap(int x, int y, int step = 0);
    void updateExitMap(int x = 60, int y = 60, int step = 0);
private:
    void resetMap(int map[64][64]);
    void updateDisplayText();
    std::string formatDisplayText(int score, int level, int lives, int health, int squirts, int gold, int sonar);
    int nDigits(int n);
    bool areObsticles(Actor* a, int x, int y) const;
    bool isClearPath(Actor* a, GraphObject::Direction dir, int n) const;
    int distanceFrom(int x, int y, const Actor& a) const;
    std::list<Actor*> m_actorList;
    Dirt* m_dirtArr[64][64];
    int m_MapToFrackman[64][64];
    int m_MapToExit[64][64];
    FrackMan* m_frackMan;
    
    int ProtestCD;
    int curCD;
    int maxProtest;
    int curProtest;
    int HCProb;
    int GoodieProb;
    int remainingBarrels;
};

#endif // STUDENTWORLD_H_