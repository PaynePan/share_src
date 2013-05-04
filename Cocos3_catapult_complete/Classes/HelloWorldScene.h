#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D.h"
#include "MyContactListener.h"

#define PTM_RATIO 32.0

#define FLOOR_HEIGHT    62.0f
using namespace cocos2d;
class HelloWorld : public cocos2d::CCLayer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	virtual void onExit();
	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();
	
	// a selector callback
	void menuCloseCallback(CCObject* pSender);

	// implement the "static node()" method manually
	CREATE_FUNC(HelloWorld);
	

	b2World* m_world; 
	b2Body* m_groundBody; 
	
	b2Fixture *m_armFixture; 
	b2Body *m_armBody; 	
	b2RevoluteJoint *m_armJoint; 
	b2MouseJoint *m_mouseJoint; 	
	b2Vec2 m_bullet_position;
	std::vector<b2Body *> m_bullets; 
	int m_currentBullet; 
	b2Body *m_bulletBody; 
	b2WeldJoint *m_bulletJoint; 
	bool m_releasingArm; 
	
	
    std::vector<b2Body*> targets; 
    std::vector<b2Body*> enemies; 
	
	void tick(float dt); 
	
	void registerWithTouchDispatcher();
	virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event); 
	virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event); 
	virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event); 
	void createBullets(int count);
	bool attachBullet();
	void resetGame();
	
	void createTargets();
	void createTarget( const char *imageName,CCPoint position,float rotation, bool isCircle,bool isStatic,bool isEnemy);
	
	MyContactListener *contactListener;

};
#endif // __HELLOWORLD_SCENE_H__
