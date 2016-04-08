#include "StudentWorld.h"
#include <string>
#include <math.h>
#include <algorithm>
#include <random>
using namespace std;

int randInt(int min, int max)
{
    if (max < min)
        std::swap(max, min);
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
:GameWorld(assetDir)
{
    
};

StudentWorld::~StudentWorld() { };

int StudentWorld::init(){
    m_frackMan = new FrackMan(this);
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            if(j < 60 && !(j > 3 && i > 29 && i < 34)){
                m_dirtArr[i][j] = new Dirt(this, i, j);
            }
            else m_dirtArr[i][j] = nullptr;
        }
    }
    resetMap(m_MapToExit);
    resetMap(m_MapToFrackman);
    
    int level = getLevel(),
    nBoulders = min(level/2+2, 6), curBoulders = 0,
    nNuggets = max(5-level/2, 2), curNuggets = 0,
    nBarrels = min(2 + level, 20), curBarrels = 0;
    
    list<Actor*> :: iterator i;
    
    while(nBoulders > curBoulders){
        int x = randInt(0, 60), y = randInt(20, 56);
        Actor* a = new Boulder(this, x, y);
        for (i = m_actorList.begin(); i != m_actorList.end(); i++){
            if(distanceFrom(x, y, (**i)) <= 6 || (x > 25 && x < 35)){
                delete a;
                a = nullptr;
            }
        }
        if(a != nullptr){
            addActor(a);
            displaceDirt(x, y);
            curBoulders++;
        }
    }
    while(nNuggets > curNuggets){
        int x = randInt(0, 60), y = randInt(0, 56);
        Actor* a = new GoldNugget(this, x, y, false);
        for (i = m_actorList.begin(); i != m_actorList.end(); i++){
            if(distanceFrom(x, y, (**i)) < 6 || (x > 26 && x < 34)){
                delete a;
                a = nullptr;
            }
        }
        if(a){
            addActor(a);
            curNuggets++;
        }
    }    while(nBarrels > curBarrels){
        int x = randInt(0, 60), y = randInt(0, 56);
        Actor* a = new OilBarrel(this, x, y);
        for (i = m_actorList.begin(); i != m_actorList.end(); i++){
            if(distanceFrom(x, y, (**i)) < 6 || (x > 26 && x < 34)){
                delete a;
                a = nullptr;
            }
        }
        if(a){
            addActor(a);
            curBarrels++;
        }
    }
    
    ProtestCD = max(25, 200-level);
    curCD = ProtestCD;
    maxProtest = min(15, static_cast<int>(level*1.5) + 2);
    curProtest = 0;
    HCProb = min(90, level*10 + 30);
    GoodieProb = level*25 + 300;
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::nDigits(int n){
    if (n < 10) return 1;
    return 1 + nDigits(n/10);
}

void StudentWorld::updateDisplayText() {
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_frackMan->getHitPoints()*10;
    int squirts = m_frackMan->getWater();
    int gold = m_frackMan->getGold();
    int sonar = m_frackMan->getSonar();

    string newText = formatDisplayText(score, level, lives, health, squirts, gold, sonar);
    
    setGameStatText(newText);
}
string StudentWorld::formatDisplayText(int score, int level, int lives, int health, int squirts, int gold, int sonar){
    string scrStr = "Scr: ";
    for (int i = 0; i < 6 - nDigits(score); i++){
        scrStr += '0';
    }
    scrStr += to_string(score);
    
    string lvlStr = "Lvl: ";
    lvlStr += ((nDigits(level) == 1)? " ":"");
    lvlStr += to_string(level);
    
    string livesStr = "Lives: ";
    livesStr += to_string(lives);
    
    string healthStr = "Hlth: ";
    healthStr += ((nDigits(health) == 2)? " ":"");
    healthStr += to_string(health);
    healthStr += "%";
    
    string sqrtStr = "Wtr: ";
    sqrtStr += ((nDigits(squirts) == 1)? " ":"");
    sqrtStr += to_string(squirts);
    
    string goldStr = "";
    goldStr += "Gld: ";
    goldStr += ((nDigits(gold) == 1)? " ":"");
    goldStr += to_string(gold);
    
    string sonarStr = "Sonar: ";
    sonarStr += ((nDigits(sonar) == 1)? " ":"");
    sonarStr += to_string(sonar);
    
    string barrelStr = "Oil Left: ";
    barrelStr += ((nDigits(remainingBarrels) == 1)? " ":"");
    barrelStr += to_string(remainingBarrels);
    
    string out = scrStr + "  " + lvlStr + "  " + livesStr + "  " + healthStr + "  " + sqrtStr + "  " + goldStr + "  " + sonarStr + "  " + barrelStr;
    
    return out;
    
}

int StudentWorld::move(){
    
    updateDisplayText();
    
    if(m_frackMan->isAlive()) m_frackMan->doSomething();
    else {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    int nRemainingBarrels = 0;
    list<Actor*> :: iterator i;
    for(i = m_actorList.begin(); i!= m_actorList.end(); i++){
        if((*i)->isAlive()) (*i)->doSomething();
        if(!(*i)->isAlive()) {
            delete (*i);
            i = m_actorList.erase(i);
            i--;
            continue;
        }
        
        if((*i)->isWinCondition()) nRemainingBarrels++;
    }
    
    if(curCD == ProtestCD && curProtest != maxProtest){
        Actor* a;
        if(randInt(1, 100) <= HCProb){
            a = new HardcoreProtester(this);
        } else {
            a = new RegularProtester(this);
        }
        addActor(a);
        curCD = 0;
        curProtest++;
    } else {
        curCD++;
    }
    
    if(randInt(1, GoodieProb) == 1){
        Actor* a = nullptr;
        if(randInt(1, 5) == 1){
            a = new SonarKit(this);
        } else {
            do{
                int x = randInt(0, 60);
                int y = randInt(0, 60);
                if(dirtExists(x, y)) continue;
                
                a = new WaterPool(this, x, y);
            } while (!a);
        }
        addActor(a);
    }
    
    if(nRemainingBarrels == 0){
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }  else {
        remainingBarrels = nRemainingBarrels;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp(){
    delete m_frackMan;
    list<Actor*> :: iterator i;
    for(i = m_actorList.begin(); i!=m_actorList.end(); i++){
        delete (*i);
    }
    m_actorList.clear();
    for (int i = 0; i< 64; i++){
        for (int j = 0; j < 64; j++){
            if(m_dirtArr[i][j]) delete m_dirtArr[i][j];
            m_dirtArr[i][j] = nullptr;
        }
    }
}

void StudentWorld::addActor(Actor* a){
    m_actorList.push_back(a);
}

bool StudentWorld::displaceDirt(int x, int y){
    bool dirtcleared = false;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(m_dirtArr[x+i][y+j]){
                delete m_dirtArr[x+i][y+j];
                m_dirtArr[x+i][y+j] = nullptr;
                dirtcleared = true;

            }
        }
    }
    if(dirtcleared) {
        updateExitMap();
    }
    return dirtcleared;
}

void StudentWorld::resetMap(int map[64][64]) {
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            if (dirtExists(i, j)){
                map[i][j] = -1;
            } else {
                map[i][j] = INT_MAX;
            }
        }
    }
}

void StudentWorld::updateFrackmanMap(int x, int y, int step) {
    if(step == 0) resetMap(m_MapToFrackman);
    if(x < 0 || y < 0) return;
    if(x > 60 || y > 60) return;
    if(step < m_MapToFrackman[x][y]) m_MapToFrackman[x][y] = step;
    else return;
    updateFrackmanMap(x + 1, y, step + 1);
    updateFrackmanMap(x - 1, y, step + 1);
    updateFrackmanMap(x, y + 1, step + 1);
    updateFrackmanMap(x, y - 1, step + 1);
}

void StudentWorld::updateExitMap(int x, int y, int step) {
    if(step == 0) resetMap(m_MapToExit);
    if(x < 0 || y < 0) return;
    if(x > 63 || y > 63) return;
    if(m_MapToExit[x][y] > step) m_MapToExit[x][y] = step;
    else return;
    updateExitMap(x + 1, y, step + 1);
    updateExitMap(x - 1, y, step + 1);
    updateExitMap(x, y + 1, step + 1);
    updateExitMap(x, y - 1, step + 1);
}

int StudentWorld::curHeatVal(int x, int y) const { return m_MapToFrackman[x][y]; }

bool StudentWorld::dirtExists(int x, int y) const {
    for(int i = 0; i < 4; i++){
        for(int j = 0; j<4; j++){
            if(x+i > 63 || y+j > 63) return true;
            if(m_dirtArr[x+i][y+j]) return true;
        }
    }
    return false;
}
GraphObject::Direction StudentWorld::determineFirstMoveToExit(Actor* a, int x, int y) {
    GraphObject::Direction out = GraphObject::none;
    int minHeat = m_MapToExit[x][y];
    
    if (canActorMove(a, GraphObject::right) && m_MapToExit[x+1][y] < minHeat){
        minHeat = m_MapToExit[x+1][y];
        out = GraphObject::right;
    }
    if (canActorMove(a, GraphObject::left) && m_MapToExit[x-1][y] < minHeat){
        minHeat = m_MapToExit[x-1][y];
        out = GraphObject::left;
    }
    if (canActorMove(a, GraphObject::up) && m_MapToExit[x][y+1] < minHeat){
        minHeat = m_MapToExit[x][y+1];
        out = GraphObject::up;
    }
    if (canActorMove(a, GraphObject::down) && m_MapToExit[x][y-1] < minHeat){
        minHeat = m_MapToExit[x][y-1];
        out = GraphObject::down;
    }
    return out;
}

GraphObject::Direction StudentWorld::determineFirstMoveToFrackMan(Actor* a, int x, int y) {
    GraphObject::Direction out = GraphObject::none;
    int minHeat = m_MapToFrackman[x][y];
    if ( canActorMove(a, GraphObject::right) && m_MapToFrackman[x+1][y] <= minHeat){
        minHeat = m_MapToFrackman[x+1][y];
        out = GraphObject::right;
    }
    if ( canActorMove(a, GraphObject::left) && m_MapToFrackman[x-1][y] <= minHeat){
        minHeat = m_MapToFrackman[x-1][y];
        out = GraphObject::left;
    }
    if ( canActorMove(a, GraphObject::up) && m_MapToFrackman[x][y+1] <= minHeat){
        minHeat = m_MapToFrackman[x][y+1];
        out = GraphObject::up;
    }
    if ( canActorMove(a, GraphObject::down) && m_MapToFrackman[x][y-1] <= minHeat){
        minHeat = m_MapToFrackman[x][y-1];
        out = GraphObject::down;
    }
    return out;
}
bool StudentWorld::canActorMove(Actor* a, GraphObject::Direction dir) const {
    int newX = a->getX();
    int newY = a->getY();
    switch(dir){
        case GraphObject::up: newY++;
            break;
        case GraphObject::down: newY--;
            break;
        case GraphObject::right: newX++;
            break;
        case GraphObject::left: newX--;
    }
    return !areObsticles(a, newX, newY);
}

bool StudentWorld::areObsticles(Actor *a, int x, int y) const {
    if(x > 60 || x < 0 || y > 60 || y < 0) return true;
    if(!a->canDigThroughDirt() && dirtExists(x, y)) return true;
    
    list<Actor*> :: const_iterator i;
    for (i = m_actorList.begin(); i != m_actorList.end(); i++){
        if(!(*i)->canActorsPassThroughMe() && distanceFrom(x, y, (**i)) < 4 && (*i) != a) return true;
    }
    return false;
}

void StudentWorld::revealAllNearbyObjects(int x, int y, int radius){
    list<Actor*> :: iterator i;
    for (i = m_actorList.begin(); i != m_actorList.end(); i++){
        if(radius >= distanceFrom(x, y, (**i))) (*i)->setVisible(true);
    }
}

bool StudentWorld::annoyAllNearbyActors(Actor *annoyer, int amount, int radius){
    list<Actor*> :: iterator i;
    bool actorsAnnoyed = false;
    if(annoyer->annoysFrackman() && radius >= distanceFrom(annoyer->getX(), annoyer->getY(), *m_frackMan)){
        m_frackMan->annoy(amount);
        actorsAnnoyed = true;
    }
    for (i = m_actorList.begin(); i!= m_actorList.end(); i++){
        if(radius >= distanceFrom(annoyer->getX(), annoyer->getY(), (**i))) {
            if((*i)->canBeAnnoyed()){
                (*i)->annoy(amount);
                actorsAnnoyed = true;
            }
        }
    }
    return actorsAnnoyed;
}

int StudentWorld::distanceFrom(int x, int y, const Actor& a) const {
    int xDist = a.getX()-x;
    int yDist = a.getY()-y;
    return sqrt(pow(xDist, 2) + pow(yDist, 2));
}

FrackMan* StudentWorld::findNearbyFrackMan(Actor *a, int radius) const {
    if(isNearFrackMan(a, radius)) return m_frackMan;
    return nullptr;
}

Actor* StudentWorld::findNearbyProtester(Actor *a, int radius) const {
    list<Actor*> :: const_iterator i;
    for (i = m_actorList.begin(); i != m_actorList.end(); i++){
        if((*i)->huntsFrackMan() && distanceFrom(a->getX(), a->getY(), (**i)) < radius) return (*i);
    }
    return nullptr;
}

bool StudentWorld::isNearFrackMan(Actor* a, int radius) const {
    return (distanceFrom(a->getX(), a->getY(), *m_frackMan) <= radius);
}

void StudentWorld::giveFrackManGold(){
    m_frackMan->addGold();
}

void StudentWorld::giveFrackManSonar(){
    m_frackMan->addSonar();
}

void StudentWorld::giveFrackManWater(){
    m_frackMan->addWater();
}

bool StudentWorld::isFacingFrackMan(Actor* a) const {
    int dX = m_frackMan->getX() - a->getX();
    int dY = m_frackMan->getY() - a->getY();
    
    switch(a->getDirection()){
        case GraphObject::right:
            if(dX >= 0) return true;
        case GraphObject::left:
            if(dX <= 0) return true;
        case GraphObject::up:
            if(dY >= 0) return true;
        case GraphObject::down:
            if(dY <= 0) return true;
    }
    return false;
}

GraphObject::Direction StudentWorld::lineOfSightToFrackMan(Actor *a) const {
    int dX = m_frackMan->getX() - a->getX();
    int dY = m_frackMan->getY() - a->getY();
    if (abs(dX) > 4 && abs(dY) > 4) return GraphObject::none;
    else if (dX > 4 && isClearPath(a, GraphObject::right, dX)){
        return GraphObject::right;
    }
    else if (dX < -4 && isClearPath(a, GraphObject::left, -dX)){
        return GraphObject::left;
    }
    else if (dY > 4 && isClearPath(a, GraphObject::up, dY)){
        return GraphObject::up;
    }
    else if (dY <-4 && isClearPath(a, GraphObject::down, -dY)){
        return GraphObject::down;
    }
    else return GraphObject::none;
}

bool StudentWorld::isClearPath(Actor* a, GraphObject::Direction dir, int n) const {
    int incrX = 0, incrY = 0;
    switch (dir){
        case GraphObject::up: incrY++;
            break;
        case GraphObject::down: incrY--;
            break;
        case GraphObject::right: incrX++;
            break;
        case GraphObject::left: incrX--;
            break;
    }
    for(int i = 1; i != n; i++) {
        if(areObsticles(a, a->getX() + incrX*i, a->getY() + incrY*i)) return false;
    }
    return true;
}

