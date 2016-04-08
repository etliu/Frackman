#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
#include <random>

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF ACTOR FUNCTIONS                                           ///////
//////////////////////////////////////////////////////////////////////////////////////////////

Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction startDir, float size, int depth)
        :GraphObject(imageID, startX, startY, startDir, size, depth), m_isAlive(true)
{
    setVisible(true);
    wld = world;
}

Actor::~Actor() { setVisible(false); };


void Actor::move(Direction dir){
    switch(dir){
        case left:
            if(getWorld()->canActorMove(this, dir)) moveTo(getX() - 1, getY());
            break;
        case right:
            if(getWorld()->canActorMove(this, dir)) moveTo(getX() + 1, getY());
            break;
        case down:
            if(getWorld()->canActorMove(this, dir)) moveTo(getX(), getY() - 1);
            break;
        case up:
            if(getWorld()->canActorMove(this, dir)) moveTo(getX(), getY() + 1);
    }
}



bool Actor::isAlive() const { return m_isAlive; }

void Actor::setDead() {
    m_isAlive = false;
}

StudentWorld* Actor::getWorld() const { return wld; };

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF AGENT FUNCTIONS                                           ///////
//////////////////////////////////////////////////////////////////////////////////////////////
Agent::Agent(StudentWorld* world, int imageID, int startX, int startY, Direction startDir,
      unsigned int hitPoints)
:Actor(world, imageID, startX, startY, startDir), m_health(hitPoints)
{ };

Agent::~Agent() { };

unsigned int Agent::getHitPoints() const { return m_health; };

bool Agent::annoy(unsigned int amount) {
    m_health -= amount;
    return m_health <= 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF FRACKMAN FUNCTIONS                                        ///////
//////////////////////////////////////////////////////////////////////////////////////////////

FrackMan::FrackMan(StudentWorld* world)
        :Agent(world, IID_PLAYER, 30, 60, right, 10),
         m_nCharges(1), m_nNuggets(0), m_uWater(5)
{};

FrackMan::~FrackMan() {};

void FrackMan::doSomething(){
    if(!isAlive()) return;
    int key;
    if (getWorld()->getKey(key)){
        switch(key){
            case KEY_PRESS_UP:
                if(getDirection() == up) {
                    move(up);
                }
                else setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                if(getDirection() ==  down) {
                    move(down);
                }
                else setDirection(down);
                break;
            case KEY_PRESS_LEFT:
                if(getDirection() == left) {
                    move(left);
                }
                else setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                if(getDirection() == right) {
                    move(right);
                }
                else setDirection(right);
                break;
            case KEY_PRESS_ESCAPE: setDead();
                break;
            case KEY_PRESS_SPACE:
            {
                if(m_uWater != 0){
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    Actor* a = new Squirt(getWorld(), getX(), getY(), getDirection());
                    getWorld()->addActor(a);
                    m_uWater--;
                }
                break;
            }
            case KEY_PRESS_TAB:
            {
                if(m_nNuggets != 0){
                    m_nNuggets--;
                    Actor* a = new GoldNugget(getWorld(), getX(), getY(), true);
                    getWorld()->addActor(a);
                }
                break;
            }
            case 'z':
            case 'Z':
            {
                if(m_nCharges !=0){
                    getWorld()->playSound(SOUND_SONAR);
                    getWorld()->revealAllNearbyObjects(getX(), getY(), 12);
                    m_nCharges--;
                }
                break;
            }
        }
        getWorld()->updateFrackmanMap(getX(), getY());
        if(getWorld()->displaceDirt(getX(), getY())) getWorld()->playSound(SOUND_DIG);
    }
}

bool FrackMan::annoy(unsigned int amount){
    if(Agent::annoy(amount)){
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
        setDead();
        return true;
    }
    else{
        getWorld()->playSound(SOUND_PLAYER_ANNOYED);
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF PROTESTER FUNCTIONS                                       ///////
//////////////////////////////////////////////////////////////////////////////////////////////

Protester::Protester(StudentWorld* world, int imageID, unsigned int hitpoints, unsigned int score)
:Agent(world, imageID, 60, 60, left, hitpoints), score(score), ticksSincePTurn(0)
{
    isActive = true;
    int i = 3 -(getWorld()->getLevel())/4;
    ticksToWait = std::max(0, i);
    curWait = ticksToWait;
    numMovesInCurDir = randInt(8, 60);
    shoutWait = 0;
}

Protester::~Protester() { };

bool Protester::isWaiting() {
    if(curWait != 0){
        curWait--;
        return true;
    }
    curWait = ticksToWait;
    return false;
};

void Protester::setTicksToNextMove(int n){
    curWait = n;
}

void Protester::doSomething(){
    if(!isAlive()) return;
    ticksSincePTurn++;
    if(!isActive){
        if(getX() == 60 && getY() == 60) setDead();
        Direction dir = getWorld()->determineFirstMoveToExit(this, getX(), getY());
        if(dir != none) setDirection(dir);
        move(getDirection());
        return;
    }
    if(shoutWait != 0) shoutWait--;
    FrackMan* a = getWorld()->findNearbyFrackMan(this, 4);
    if(a && getWorld()->isFacingFrackMan(this)){
        if(shoutWait == 0){
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            a->annoy(2);
            shoutWait = 15;
            return;
        }
    }
    if(getWorld()->lineOfSightToFrackMan(this) != none){
        setDirection(getWorld()->lineOfSightToFrackMan(this));
        move(getDirection());
        numMovesInCurDir = 0;
        return;
    }
    numMovesInCurDir--;
    if(numMovesInCurDir <= 0){
        for(;;){
            int dir = randInt(0, 3);
            if(dir == 0 && getWorld()->canActorMove(this, up)) {
                setDirection(up);
                break;
            }
            else if(dir == 1 && getWorld()->canActorMove(this, down)) {
                setDirection(down);
                break;
            }
            else if(dir == 2 && getWorld()->canActorMove(this, left)) {
                setDirection(left);
                break;
            }
            else if(dir == 3 && getWorld()->canActorMove(this, right)) {
                setDirection(right);
                break;
            }
        }
        numMovesInCurDir = randInt(8, 60);
    }
    else if ( ticksSincePTurn >= 200 ) {
        Direction dir = getDirection();
        if(wasPTurn(dir, up)){
            if(getWorld()->canActorMove(this, up)) setDirection(up);
        }
        if(wasPTurn(dir, down)){
            if(getWorld()->canActorMove(this, down)){
                if(getDirection() == up){
                    if(randInt(0,1) == 1)setDirection(down);
                } else {
                    setDirection(down);
                }
            }
        }
        if(wasPTurn(dir, left)){
            if(getWorld()->canActorMove(this, left)) setDirection(left);
        }
        if(wasPTurn(dir, right)){
            if(getWorld()->canActorMove(this, right)){
                if(getDirection() == left){
                    if(randInt(0,1) == 1)setDirection(right);
                } else {
                    setDirection(right);
                }
            }
        }
        if(dir != getDirection()){
            ticksSincePTurn = 0;
            numMovesInCurDir = randInt(8, 60);
        }
    }
    if(getWorld()->canActorMove(this, getDirection())) {
        move(getDirection());
    }
    else numMovesInCurDir = 0;
}

bool Protester::annoy(unsigned int amount){
    if(!isActive) return false;
    if(Agent::annoy(amount)){
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        isActive = false;
        curWait = 0;
        (amount == 2)?getWorld()->increaseScore(score):getWorld()->increaseScore(500);
        return true;
    } else {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        int i = 100-(getWorld()->getLevel())*10;
        curWait = std::min(50,i);
        return false;
    }
}

bool Protester::wasPTurn(Direction di, Direction df){
    if(di == right || di == left){
        return (df == up || df == down);
    }
    return (df == right || df == left);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF REGULAR PROTESTER FUNCTIONS                               ///////
//////////////////////////////////////////////////////////////////////////////////////////////

RegularProtester::RegularProtester(StudentWorld* world)
:Protester(world, IID_PROTESTER, 5, 100)
{ };

RegularProtester::~RegularProtester() { };

void RegularProtester::doSomething() {
    if(!isWaiting()) { Protester::doSomething(); };
}

void RegularProtester::addGold() {
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(25);
    setInactive();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF HARDCORE PROTESTER FUNCTIONS                              ///////
//////////////////////////////////////////////////////////////////////////////////////////////

HardcoreProtester::HardcoreProtester(StudentWorld* world)
:Protester(world, IID_HARD_CORE_PROTESTER, 20, 250)
{ };

HardcoreProtester::~HardcoreProtester() { };

void HardcoreProtester::doSomething(){
    if(!isAlive()) return;
    if(isWaiting()) return;
    if(isActiveP() && !getWorld()->isNearFrackMan(this, 4)){
        if(getWorld()->curHeatVal(getX(), getY()) < 16 + getWorld()->getLevel()*2){
            Direction dir = getWorld()->determineFirstMoveToFrackMan(this, getX(), getY());
            if(dir != none) setDirection(dir);
            move(getDirection());
            return;
        }
    }
    Protester::doSomething();
}

void HardcoreProtester::addGold(){
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(50);
    int i = 100-getWorld()->getLevel()*10;
    setTicksToNextMove(std::min(50, i));
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF DIRT FUNCTIONS                                            ///////
//////////////////////////////////////////////////////////////////////////////////////////////

Dirt::Dirt(StudentWorld* world, int startX, int startY)
:Actor(world, IID_DIRT, startX, startY, right, .25, 3)
{ };

Dirt::~Dirt() {};

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF BOULDER FUNCTIONS                                         ///////
//////////////////////////////////////////////////////////////////////////////////////////////
Boulder::Boulder(StudentWorld* world, int startX, int startY)
       :Actor(world, IID_BOULDER, startX, startY, down, 1.0, 1), m_isWaiting(false), m_waitCount(30)
{ };

Boulder::~Boulder() { };

void Boulder::doSomething(){
    if(!isAlive()) return;
    if(m_waitCount == 0){
        m_isWaiting = false;
        if(getWorld()->canActorMove(this, down)){
            move(down);
            if(getWorld()->annoyAllNearbyActors(this, 100, 3)) setDead();
        } else setDead();
    }
    if(m_isWaiting){
        m_waitCount--;
        return;
    }
    else if (getWorld()->canActorMove(this, down)){
        m_isWaiting = true;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF SQUIRT FUNCTIONS                                          ///////
//////////////////////////////////////////////////////////////////////////////////////////////
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction startDir)
      :Actor(world, IID_WATER_SPURT, startX, startY, startDir, 1.0, 1), travelDistance(4)
{ };

Squirt::~Squirt() { };

void Squirt::doSomething(){
    if(travelDistance == 0 || getWorld()->annoyAllNearbyActors(this, 2, 3)) {
        setDead();
        return;
    }
    travelDistance--;
    move(getDirection());
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF ACTIVATING OBJECT FUNCTIONS                               ///////
//////////////////////////////////////////////////////////////////////////////////////////////
ActivatingObject::ActivatingObject(StudentWorld* world, int startX, int startY, int imageID, int soundToPlay, int ptValue)
:Actor(world,imageID, startX, startY, right, 1.0, 2), ptValue(ptValue), remainingLifetime(-1), sound(soundToPlay)
{ };

ActivatingObject::~ActivatingObject() { };

bool ActivatingObject::ActivateIfNecessary() {
    FrackMan* a = getWorld()->findNearbyFrackMan(this, 3);
    if(getWorld()->isNearFrackMan(this, 3)){
        setDead();
        getWorld()->increaseScore(ptValue);
        getWorld()->playSound(sound);
    }
    return a;
}

void ActivatingObject::doSomething() {
    if(!isAlive()) return;
    if(!ActivateIfNecessary() && getWorld()->isNearFrackMan(this, 4)){
        setVisible(true);
    }
};

void ActivatingObject::setTicksToLive(int n){
    remainingLifetime = n;
}
int ActivatingObject::getRemainingLifetime() { return remainingLifetime; };

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF OIL BARREL FUNCTIONS                                      ///////
//////////////////////////////////////////////////////////////////////////////////////////////
OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY)
:ActivatingObject(world, startX, startY, IID_BARREL, SOUND_FOUND_OIL, 1000)
{ setVisible(false); };

OilBarrel::~OilBarrel() { };

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF GOLD NUGGET FUNCTIONS                                     ///////
//////////////////////////////////////////////////////////////////////////////////////////////
GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary)
:ActivatingObject(world, startX, startY, IID_GOLD, SOUND_GOT_GOODIE, 10), wasDropped(temporary)
{
    if(!temporary) setVisible(false);
    else setTicksToLive(100);
};

GoldNugget::~GoldNugget() { };

void GoldNugget::doSomething(){
    if(!wasDropped){
        ActivatingObject::doSomething();
        if(!isAlive()) getWorld()->giveFrackManGold();
    } else {
        if(getRemainingLifetime() == 0){
            setDead();
            return;
        }
        setTicksToLive(getRemainingLifetime()-1);
        Actor* a = getWorld()->findNearbyProtester(this, 3);
        if(a){
            a->addGold();
            getWorld()->increaseScore(25);
            setDead();
        }
    }
    
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF SONAR KIT FUNCTIONS                                       ///////
//////////////////////////////////////////////////////////////////////////////////////////////
SonarKit::SonarKit(StudentWorld* world)
:ActivatingObject(world, 0, 60, IID_SONAR, SOUND_GOT_GOODIE, 75)
{
    int i = 300 - 10*(getWorld()->getLevel());
    int initialLifetime = std::min(100, i);
    setTicksToLive(initialLifetime);
};

SonarKit::~SonarKit() { };

void SonarKit::doSomething(){
    if(ActivateIfNecessary()) getWorld()->giveFrackManSonar();
    setTicksToLive(getRemainingLifetime()-1);
    if(getRemainingLifetime() == 0) setDead();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////     IMPLEMENTATION OF WATER POOL FUNCTIONS                                      ///////
//////////////////////////////////////////////////////////////////////////////////////////////
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
:ActivatingObject(world, startX, startY, IID_WATER_POOL, SOUND_GOT_GOODIE, 100)
{
    int i = 300 - 10*(getWorld()->getLevel());
    int initialLifeTime = std::min(100, i);
    setTicksToLive(initialLifeTime);
}

WaterPool::~WaterPool() { };

void WaterPool::doSomething(){
    if(ActivateIfNecessary()) getWorld()->giveFrackManWater();
    setTicksToLive(getRemainingLifetime()-1);
    if(getRemainingLifetime() == 0) setDead();
}








