	#include "MyContactListener.h" 
	#include "cocos2d.h"
	using namespace cocos2d;	
	MyContactListener::MyContactListener() : contacts() 
	{ 
	} 
	
	MyContactListener::~MyContactListener() 
	{ 
	} 
	
	
	void MyContactListener::BeginContact(b2Contact* contact) 
	{ 
	} 
	
	void MyContactListener::EndContact(b2Contact* contact) 
	{ 
	} 
	
	void MyContactListener::PreSolve(b2Contact* contact, 
	const b2Manifold* oldManifold) 
	{ 
	} 
	
	void MyContactListener::PostSolve(b2Contact* contact, 
	const b2ContactImpulse* impulse) 
	{ 
		//CCLog("1--------- contact->GetFixtureA() is %d, user_data is", contact->GetFixtureA(),contact->GetFixtureA()->GetUserData() );
		
		bool isAEnemy = (contact->GetFixtureA()->GetUserData() != NULL); 
		bool isBEnemy = (contact->GetFixtureB()->GetUserData() != NULL); 
		if (isAEnemy || isBEnemy)
		{ 
			// Should the body break? 
			int32 count = contact->GetManifold()->pointCount; 
			
			float32 maxImpulse = 0.0f; 
			for (int32 i = 0; i < count; ++i) 
			{ 
				maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]); 
			} 
			
			//CCLog("--------- maxImpulse is %d", maxImpulse);
			if (maxImpulse > 1.0f) 
			{ 
				// Flag the enemy(ies) for breaking. 
				if (isAEnemy) 
				contacts.insert(contact->GetFixtureA()->GetBody()); 
				if (isBEnemy) 
				contacts.insert(contact->GetFixtureB()->GetBody()); 
			} 
		} 
	} 
