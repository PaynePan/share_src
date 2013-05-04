#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GameOverScene.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();
	
	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void HelloWorld::onExit() {
	//CCLog("------------ HelloWorld::onExit");
	delete m_world;
	delete contactListener;
	//m_world = NULL;
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);

}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}
	
	/*
	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
	"CloseNormal.png",
	"CloseSelected.png",
	this,
	menu_selector(HelloWorld::menuCloseCallback) );
	pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
	pMenu->setPosition( CCPointZero );
	this->addChild(pMenu, 15);
	*/
	
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	
	CCSprite *sprite = CCSprite::create("bg1.png");  //背景图 
	sprite->setAnchorPoint(CCPointZero); 
	this->addChild(sprite, -1); 
	
	sprite = CCSprite::create("catapult_base_2.png"); //投射器底部后面那块 
	sprite->setAnchorPoint(CCPointZero); 
	sprite->setPosition(CCPointMake(181.0, FLOOR_HEIGHT)); 
	this->addChild(sprite, 0); 
	
	sprite = CCSprite::create("squirrel_1.png");        //左边松鼠 
	sprite->setAnchorPoint(CCPointZero); 
	sprite->setPosition(CCPointMake(11.0, FLOOR_HEIGHT)); 
	this->addChild(sprite, 0); 
	
	sprite = CCSprite::create("catapult_base_1.png");   //投射器底部前面那块 
	sprite->setAnchorPoint(CCPointZero); 
	sprite->setPosition(CCPointMake(181.0, FLOOR_HEIGHT)); 
	this->addChild(sprite, 9); 
	
	sprite = CCSprite::create("squirrel_2.png");    //右边松鼠 
	sprite->setAnchorPoint(CCPointZero); 
	sprite->setPosition(CCPointMake(240.0, FLOOR_HEIGHT)); 
	this->addChild(sprite, 9); 
	
	sprite = CCSprite::create("fg.png");    //带冰的地面 
	sprite->setAnchorPoint(CCPointZero); 
	this->addChild(sprite, 10); 	
	
	b2Vec2 gravity; 
	gravity.Set(0.0f, -10.0f); 
	bool doSleep = true; 
	m_world = new b2World(gravity); 
	m_world->SetAllowSleeping(doSleep); 
	m_world->SetContinuousPhysics(true); 	
	
	b2BodyDef groundBodyDef; 
	groundBodyDef.position.Set(0, 0); // bottom-left corner 
	m_groundBody = m_world->CreateBody(&groundBodyDef); 
	
	b2EdgeShape groundBox; 
	// bottom 
	groundBox.Set(b2Vec2(0,FLOOR_HEIGHT/PTM_RATIO), b2Vec2(screenSize.width*2.0f/PTM_RATIO,FLOOR_HEIGHT/PTM_RATIO)); 
	m_groundBody->CreateFixture(&groundBox, 0); 
	
	// top 
	groundBox.Set(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width*2.0f/PTM_RATIO,screenSize.height/PTM_RATIO)); 
	m_groundBody->CreateFixture(&groundBox, 0); 
	
	// left 
	groundBox.Set(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(0,0)); 
	m_groundBody->CreateFixture(&groundBox, 0); 
	
	// right 
	groundBox.Set(b2Vec2(screenSize.width*1.5f/PTM_RATIO,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width*1.5f/PTM_RATIO,0)); 
	m_groundBody->CreateFixture(&groundBox, 0); 

	
	// Create the catapult's arm 
	CCSprite *arm = CCSprite::create("catapult_arm.png"); 
	this->addChild(arm, 1); 
	
	b2BodyDef armBodyDef; 
	armBodyDef.type = b2_dynamicBody; 
	armBodyDef.linearDamping = 1; 
	armBodyDef.angularDamping = 1; 
	armBodyDef.position.Set(230.0f/PTM_RATIO, (FLOOR_HEIGHT+91.0f)/PTM_RATIO); 
	armBodyDef.userData = arm; 
	m_armBody = m_world->CreateBody(&armBodyDef); 
	
	b2PolygonShape armBox; 
	b2FixtureDef armBoxDef; 
	armBoxDef.shape = &armBox; 
	armBoxDef.density = 0.3F; 
	armBox.SetAsBox(11.0f/PTM_RATIO, 91.0f/PTM_RATIO); 
	m_armFixture = m_armBody->CreateFixture(&armBoxDef);

	
	
	b2RevoluteJointDef armJointDef; 
	armJointDef.Initialize(m_groundBody, m_armBody, b2Vec2(233.0f/PTM_RATIO, FLOOR_HEIGHT/PTM_RATIO)); 
	armJointDef.enableMotor = true; 
	armJointDef.enableLimit = true; 
	armJointDef.motorSpeed  = -10; //-1260; 
	armJointDef.lowerAngle  = CC_DEGREES_TO_RADIANS(9); 
	armJointDef.upperAngle  = CC_DEGREES_TO_RADIANS(75); 
	armJointDef.maxMotorTorque = 700; 
	m_armJoint = (b2RevoluteJoint*)m_world->CreateJoint(&armJointDef); 	
	
	
	
	m_mouseJoint = NULL; 
	m_releasingArm = false; 
	
	
	contactListener = new MyContactListener(); 
	m_world->SetContactListener(contactListener);
	
	this->setTouchEnabled(true);
	schedule(schedule_selector(HelloWorld::tick)); 	
	
	scheduleOnce(schedule_selector(HelloWorld::resetGame),0.2f); 	
	/*	
	CCDelayTime *delayAction = CCDelayTime::actionWithDuration(0.2f); 
		CCCallFunc *callSelectorAction = CCCallFunc::actionWithTarget(this, callfunc_selector(HelloWorld::resetGame)); 
		this->runAction(CCSequence::actions(delayAction, 
											callSelectorAction, 
											NULL)); 	
*/											
	//CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("background-music-aac.wav", true);
	return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();
}

void HelloWorld::tick(float dt) 
{ 

	CCLog("++++++++tick  %f", dt);

	int velocityIterations = 8; 
	int positionIterations = 1; 

	m_world->Step(dt, velocityIterations, positionIterations); 
	for(b2Body *b = m_world->GetBodyList(); b; b=b->GetNext()) {    
		if (b->GetUserData() != NULL) {
			//Synchronize the AtlasSprites position and rotation with the corresponding body 
			CCSprite* myActor = (CCSprite*)b->GetUserData(); 
			myActor->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) ); 
			myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) ); 		}        
	}
	
	if (m_releasingArm && m_bulletJoint != NULL) 
	{ 
		// Check if the arm reached the end so we can return the limits 
		if (m_armJoint->GetJointAngle() <= CC_DEGREES_TO_RADIANS(10)) 
		{ 
			m_releasingArm = false; 

			// Destroy joint so the bullet will be free 
			m_world->DestroyJoint(m_bulletJoint); 
			m_bulletJoint = NULL; 

		} 
	}

	// Bullet is moving. 
	if (m_bulletBody && m_bulletJoint == NULL) 
	{ 
		b2Vec2 position = m_bulletBody->GetPosition(); 
		CCPoint myPosition = this->getPosition(); 
		CCSize screenSize = CCDirector::sharedDirector()->getWinSize(); 
		
		static int count = 0;
		if ( ++count % 60 ==0) {
			
			//CCLog("++++++++after  x:%f, y:%f", position.x, position.y);
			if ( abs(m_bullet_position.x-position.x) < 1.5f &&
					abs(m_bullet_position.y-position.y) < 1.5f ) {
				attachBullet();
				return;
			}
			m_bullet_position = position;
		}
		
		// Move the camera. 
		if (position.x > screenSize.width / 2.0f / PTM_RATIO) 
		{ 
			myPosition.x = -MIN(screenSize.width * 2.0f - screenSize.width, position.x * PTM_RATIO - screenSize.width / 2.0f); 
			this->setPosition(myPosition); 
		} 
	} 
	
	std::set<b2Body*>::iterator pos; 
	for(pos = contactListener->contacts.begin(); pos != contactListener->contacts.end(); ++pos) 
	{ 
		b2Body *body = *pos; 
		for (vector<b2Body*>::iterator iter = targets.begin(); iter !=targets.end(); ++iter) 
		{ 
			if (body == *iter)
			{ 
				iter = targets.erase(iter); 
				break; 
			} 
		} 
		for (vector<b2Body*>::iterator iter = enemies.begin(); iter !=enemies.end(); ++iter) 
		{ 
			if (body == *iter) 
			{ 
				iter = enemies.erase(iter); 
				break; 
			} 
		} 

		CCNode *contactNode = (CCNode*)body->GetUserData(); 
		CCPoint position = contactNode->getPosition(); 
		removeChild(contactNode, true); 
		m_world->DestroyBody(body);
		
		CCParticleSun* explosion = CCParticleSun::create();
		explosion->retain(); 
		explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png")); 
		explosion->initWithTotalParticles(100); 
		explosion->setAutoRemoveOnFinish(true); 
		explosion->setStartSizeVar(10.0f); 
		explosion->setSpeed(70.0f); 
		explosion->setAnchorPoint(ccp(0.5f, 0.5f)); 
		explosion->setPosition(position); 
		explosion->setDuration(1.0f); 
		addChild(explosion, 11); 
		explosion->release(); 		
		
	}
	contactListener->contacts.clear();		
}	


void HelloWorld::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this,0);
}


void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) 
{ 
	if (m_mouseJoint != NULL)   {       return;     } 

	CCTouch* touch = (CCTouch*)( touches->anyObject() );
	CCPoint location = touch->getLocation();
	
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO); 
	
	if (locationWorld.x < m_armBody->GetWorldCenter().x + 150.0/PTM_RATIO) 
	{ 
		b2MouseJointDef md; 
		md.bodyA = m_groundBody; 
		md.bodyB = m_armBody; 
		md.target = locationWorld; 
		md.maxForce = 2000; 
		
		m_mouseJoint = (b2MouseJoint *)m_world->CreateJoint(&md); 
	} 
} 

void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) 
{ 
	if (m_mouseJoint == NULL)   {   return; } 
	
	CCTouch* touch = (CCTouch*)( touches->anyObject() );
	CCPoint location = touch->getLocation();
	
	
	b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO); 
	
	m_mouseJoint->SetTarget(locationWorld); 
} 

void HelloWorld::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) 
{ 
	if (m_mouseJoint != NULL) 
	{ 
		
		if (m_armJoint->GetJointAngle() >= CC_DEGREES_TO_RADIANS(5)) 
		{ 
			m_releasingArm = true; 
		} 		
		
		m_world->DestroyJoint(m_mouseJoint); 
		m_mouseJoint = NULL; 
		return; 
	} 
} 


void HelloWorld::createBullets(int count) 
{ 
	m_currentBullet = 0; 
	float pos = 62.0f; 
	
	if (count > 0) 
	{ 
		// delta is the spacing between corns 
		// 62 is the position o the screen where we want the corns to start appearing 
		// 165 is the position on the screen where we want the corns to stop appearing 
		// 30 is the size of the corn 
		
		float delta = (count > 1)?((165.0f - 62.0f - 30.0f) / (count - 1)):0.0f; 
		
		for (int i=0; i<count; i++, pos += delta) 
		{ 
			// Create the bullet 
			CCSprite *sprite = CCSprite::create("acorn.png"); 
			this->addChild(sprite, 1); 
			
			b2BodyDef bulletBodyDef; 
			bulletBodyDef.type = b2_dynamicBody; 
			bulletBodyDef.bullet = true; 

			bulletBodyDef.position.Set(pos/PTM_RATIO,(FLOOR_HEIGHT+15.0f)/PTM_RATIO); 
			
			m_bullet_position = bulletBodyDef.position;

			bulletBodyDef.userData = sprite; 
			b2Body *bullet = m_world->CreateBody(&bulletBodyDef); 
			bullet->SetActive(false); 
			
			b2CircleShape circle; 
			circle.m_radius = 15.0/PTM_RATIO; 
			
			b2FixtureDef ballShapeDef; 
			ballShapeDef.shape = &circle; 
			ballShapeDef.density = 0.8f; 
			ballShapeDef.restitution = 0.2f; 
			ballShapeDef.friction = 10.99f; 
			bullet->CreateFixture(&ballShapeDef); 
			
			m_bullets.push_back(bullet); 
		} 
	} 
} 

bool HelloWorld::attachBullet() 
{ 
	if (m_currentBullet < m_bullets.size()) 
	{ 
		m_bulletBody = (b2Body*)m_bullets.at(m_currentBullet++); 
		m_bulletBody->SetTransform(b2Vec2(230.0f/PTM_RATIO, (155.0f+FLOOR_HEIGHT)/PTM_RATIO), 0.0f); 
		m_bulletBody->SetActive(true); 
		
		b2WeldJointDef weldJointDef; 
		weldJointDef.Initialize(m_bulletBody, m_armBody, b2Vec2(230.0f/PTM_RATIO,(155.0f+FLOOR_HEIGHT)/PTM_RATIO)); 
		weldJointDef.collideConnected = false; 
		
		m_bulletJoint = (b2WeldJoint*)m_world->CreateJoint(&weldJointDef); 
		this->setPosition(0,0); 
		return true; 
	}
	
	CCDirector::sharedDirector()->replaceScene(GameOver::scene());
	return false; 
} 

void HelloWorld::resetGame( ) 
{ 
	this->createBullets(3); 
	this->attachBullet(); 
	this->createTargets(); 

} 


void HelloWorld::createTarget(const char *imageName,CCPoint position,float rotation, bool isCircle,bool isStatic,bool isEnemy) 
{ 
	CCSprite *sprite = CCSprite::create(imageName); 
	this->addChild(sprite, 1); 
	
	b2BodyDef bodyDef; 
	bodyDef.type = isStatic ? b2_staticBody : b2_dynamicBody; 
	bodyDef.position.Set((position.x+sprite->getContentSize().width/2.0f)/PTM_RATIO, 
	(position.y+sprite->getContentSize().height/2.0f)/PTM_RATIO); 
	bodyDef.angle = CC_DEGREES_TO_RADIANS(rotation); 
	bodyDef.userData = sprite; 
	
	b2Body *body = m_world->CreateBody(&bodyDef); 
	
	b2FixtureDef boxDef;
	boxDef.density = 0.5f;

	if (isEnemy){
		boxDef.userData = (void *)1;
		enemies.push_back(body); 
	} 
	
	
	if (isCircle){ 
		b2CircleShape circle; 
		circle.m_radius = sprite->getContentSize().width/2.0f/PTM_RATIO; 
		boxDef.shape = &circle; 
		body->CreateFixture(&boxDef); 		
	} 
	else{ 
		b2PolygonShape box; 
		box.SetAsBox(sprite->getContentSize().width/2.0f/PTM_RATIO, sprite->getContentSize().height/2.0f/PTM_RATIO); 
		boxDef.shape = &box;
		body->CreateFixture(&boxDef);
	}
	
	targets.push_back(body); 
} 


void HelloWorld::createTargets() 
{ 
	createTarget("brick_2.png", CCPointMake(675.0, FLOOR_HEIGHT), 0.0f, false, false, false); 
	createTarget("brick_1.png", CCPointMake(741.0, FLOOR_HEIGHT), 0.0f, false, false, false); 
	createTarget("brick_1.png", CCPointMake(741.0, FLOOR_HEIGHT+23.0), 0.0f, false, false, false); 
	createTarget("brick_3.png", CCPointMake(673.0, FLOOR_HEIGHT+46.0), 0.0f, false, false, false); 
	createTarget("brick_1.png", CCPointMake(707.0, FLOOR_HEIGHT+58.0), 0.0f, false, false, false); 
	createTarget("brick_1.png", CCPointMake(707.0, FLOOR_HEIGHT+81.0), 0.0f, false, false, false); 
	
	createTarget("head_dog.png", CCPointMake(702.0, FLOOR_HEIGHT), 0.0f, true, false, true); 
	createTarget("head_cat.png", CCPointMake(680.0, FLOOR_HEIGHT+58.0), 0.0f, true, false, true); 
	createTarget("head_dog.png", CCPointMake(740.0, FLOOR_HEIGHT+58.0), 0.0f, true, false, true); 
	
	// 2 bricks at the right of the first block 
	createTarget("brick_2.png", CCPointMake(770.0, FLOOR_HEIGHT), 0.0f, false, false, false); 
	createTarget("brick_2.png", CCPointMake(770.0, FLOOR_HEIGHT+46.0), 0.0f, false, false, false); 
	
	// The dog between the blocks 
	createTarget("head_dog.png", CCPointMake(830.0, FLOOR_HEIGHT), 0.0f, true, false, true); 
	
	// Second block 
	createTarget("brick_platform.png", CCPointMake(839.0, FLOOR_HEIGHT), 0.0f, false, true, false); 
	createTarget("brick_2.png", CCPointMake(854.0, FLOOR_HEIGHT+28.0), 0.0f, false, false, false); 
	createTarget("brick_2.png", CCPointMake(854.0, FLOOR_HEIGHT+28.0+46.0), 0.0f, false, false, false); 
	createTarget("head_cat.png", CCPointMake(881.0, FLOOR_HEIGHT+28.0), 0.0f, true, false, true); 
	createTarget("brick_2.png", CCPointMake(909.0, FLOOR_HEIGHT+28.0), 0.0f, false, false, false); 
	createTarget("brick_1.png", CCPointMake(909.0, FLOOR_HEIGHT+28.0+46.0), 0.0f, false, false, false); 
	createTarget("brick_1.png", CCPointMake(909.0, FLOOR_HEIGHT+28.0+46.0+23.0), 0.0f, false, false, false); 
	createTarget("brick_2.png", CCPointMake(882.0, FLOOR_HEIGHT+108.0), 90.0f, false, false, false); 
} 


