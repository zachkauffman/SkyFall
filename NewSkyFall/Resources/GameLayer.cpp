/*

Background music:
Blipotron by Kevin MacLeod (incompetech.com)

*/


#include "GameLayer.h"

GameLayer::~GameLayer () {
    
    //release all retained actions
    CC_SAFE_RELEASE(_growBomb);
    CC_SAFE_RELEASE(_rotateSprite);
    CC_SAFE_RELEASE(_shockwaveSequence);
    CC_SAFE_RELEASE(_swingHealth);
    CC_SAFE_RELEASE(_groundHit);
    CC_SAFE_RELEASE(_explosion);
    
    //release all retained arrays
    CC_SAFE_RELEASE(_clouds);
    CC_SAFE_RELEASE(_meteorPool);
    CC_SAFE_RELEASE(_healthPool);
    CC_SAFE_RELEASE(_fallingObjects);
	
}

CCScene* GameLayer::scene() {
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameLayer *layer = GameLayer::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameLayer::init() {
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() ) {
        return false;
    }
    
    //get screen size
	_screenSize = CCDirector::sharedDirector()->getWinSize();
	
    _running = false;
    
    //create game screen elements
    createGameScreen();
    
    //create object pools
    createPools();
    //create CCActions
    createActions();
    
    //create array to store all falling objects (will use it in collision check)
    _fallingObjects = CCArray::createWithCapacity(40);
    _fallingObjects->retain();
    
    
	//listen for touches
    this->setTouchEnabled(true);
    
    //create main loop
    this->schedule(schedule_selector(GameLayer::update));
    
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("background.mp3", true);
    return true;
}


void GameLayer::update (float dt) {
    
    if (!_running) return;
    
    
    //update timers
    
    _meteorTimer += dt;
    if (_meteorTimer > _meteorInterval) {
        _meteorTimer = 0;
        this->resetMeteor();
    }
    
    _healthTimer += dt;
    if (_healthTimer > _healthInterval) {
        _healthTimer = 0;
        this->resetHealth();
    }
    
    _difficultyTimer += dt;
    if (_difficultyTimer > _difficultyInterval) {
        _difficultyTimer = 0;
        this->increaseDifficulty();
    }

}

void GameLayer::ccTouchesBegan(CCSet* pTouches, CCEvent* event) {
    
    
}

//call back for when falling object reaches its target
void GameLayer::fallingObjectDone (CCNode* pSender) {
    
    //remove it from array
    _fallingObjects->removeObject(pSender);
    pSender->stopAllActions();
    pSender->setRotation(0);
    
    //if object is a meteor...
    if (pSender->getTag() == kSpriteMeteor) {
        _energy -= 15;
        //show explosion animation
        pSender->runAction((CCAction*)_groundHit->copy()->autorelease());
        //play explosion sound
        SimpleAudioEngine::sharedEngine()->playEffect("boom.wav");

    //if object is a health drop...
    } else {
        pSender->setVisible(false);

        //if energy is full, score points from falling drop
        if (_energy == 100) {
            
            _score += 25;
            char score[100] = {0};
            sprintf(score, "%i", _score);
            _scoreDisplay->setString(score);
            
        } else {
            _energy+= 10;
            if (_energy > 100) _energy = 100;
        }
        
        //play health bonus sound
        SimpleAudioEngine::sharedEngine()->playEffect("health.wav");
    }
    
    //if energy is less or equal 0, game over
    if (_energy <= 0) {
        _energy = 0;
        this->stopGame();
        SimpleAudioEngine::sharedEngine()->playEffect("fire_truck.wav");
        //show GameOver
        _gameOverMessage->setVisible(true);
    }
    
    char szValue[100] = {0};
    sprintf(szValue, "%i%s", _energy, "%");
    _energyDisplay->setString(szValue);

}

//call back for animation done (hide animated object)
void GameLayer::animationDone (CCNode* pSender) {
    pSender->setVisible(false);
}

//call back for shockwave done, hide shockwave
void GameLayer::shockwaveDone() {
    _shockWave->setVisible(false);
}

//use a new meteor from the pool
void GameLayer::resetMeteor(void) {
    
}

//use a new health drop from the pool
void GameLayer::resetHealth(void) {
    if (_fallingObjects->count() > 30) return;
    
    CCSprite * health = (CCSprite *) _healthPool->objectAtIndex(_healthPoolIndex);
	_healthPoolIndex++;
	if (_healthPoolIndex == _healthPool->count()) _healthPoolIndex = 0;
    
	int health_x = rand() % (int) (_screenSize.width * 0.8f) + _screenSize.width * 0.1f;
    int health_target_x = rand() % (int) (_screenSize.width * 0.8f) + _screenSize.width * 0.1f;
    
    health->stopAllActions();
    health->setPosition(ccp(health_x, _screenSize.height + health->boundingBox().size.height * 0.5));
	
    //create action (swing, move to target, and call function when done)
    CCFiniteTimeAction*  sequence = CCSequence::create(
       CCMoveTo::create(_healthSpeed, ccp(health_target_x, _screenSize.height * 0.15f)),
       CCCallFuncN::create(this, callfuncN_selector(GameLayer::fallingObjectDone)),
       NULL);
    
    health->setVisible ( true );
    health->runAction((CCAction *)_swingHealth->copy()->autorelease());
    health->runAction(sequence);
    _fallingObjects->addObject(health);
}

//start
void GameLayer::resetGame(void) {
    
}

//this function is called at regular intervals
void GameLayer::increaseDifficulty () {

    _meteorInterval -= 0.2f;
    if (_meteorInterval < 0.25f) _meteorInterval = 0.25f;
    _meteorSpeed -= 1;
    if (_meteorSpeed < 4) _meteorSpeed = 4;
    _healthSpeed -= 1;
    if (_healthSpeed < 8) _healthSpeed = 8;
    
}

//if player is killed, stop all actions, clear screen
void GameLayer::stopGame() {
    
}


void GameLayer::createGameScreen() {

    //add bg
    CCSprite * bg = CCSprite::create("bg.png");
    bg->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg);


}

void GameLayer::createPools() {

}

void GameLayer::createActions() {

}
