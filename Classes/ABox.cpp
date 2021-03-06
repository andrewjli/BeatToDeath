#include "ABox.h"

USING_NS_CC;
#define PTM_RATIO 30


ABox::ABox(BoxType t, b2World* world){
	myWorld = world;
	type = t;
	oldSprite = nullptr;

	//TODO vary based on type
	sprite = Sprite::create();
	addChild(sprite);

	if (type == Player) {
		sprite->initWithFile("player.png");
		Size screenSize = Director::getInstance()->getWinSize();
		boxBodyDef.type = b2_dynamicBody;
		boxBodyDef.position.Set(100,100);
		boxBodyDef.angle = 0;

		boxBodyDef.userData = this;

		boxBody = world->CreateBody(&boxBodyDef);

		boxShape.SetAsBox(getSprite()->getContentSize().width / PTM_RATIO / 2,
						  getSprite()->getContentSize().height / PTM_RATIO / 2,
						  b2Vec2(0.85, 0.85), 0); // need to divide by 2 for some reason

		boxShapeDef.shape = &boxShape;
		boxShapeDef.density = 10.0f;
		boxShapeDef.friction = 0.0f;
		boxShapeDef.restitution = 0.0f;
		boxBody->CreateFixture(&boxShapeDef);
		boxBody->SetFixedRotation(true);

		// Create sensor thing
		boxShape.SetAsBox((getSprite()->getContentSize().width*0.8) / PTM_RATIO / 2,
						  0.5 / PTM_RATIO / 2,
						  b2Vec2(0.85, -0.15), 0);
		boxShapeDef.isSensor = true;
		boxShapeDef.userData = boxBody;
		b2Fixture* footSensorFixture = boxBody->CreateFixture(&boxShapeDef);
	}
	else
	{
		if (type == Static) {
			sprite->initWithFile("inactive.png");
			boxBodyDef.type = b2_staticBody;
		}

		else if (type == Dead) {
			sprite->initWithFile("dead.png");
			boxBodyDef.type = b2_staticBody;
		}
		else if (type == Goal) {
			sprite->initWithFile("goal.png");
			boxBodyDef.type = b2_staticBody;
		}
		else if (type == Kill) {
			sprite->initWithFile("kill.png");
			boxBodyDef.type = b2_staticBody;
		}

		boxBodyDef.userData = this;

		boxBody = world->CreateBody(&boxBodyDef);

		boxShape.SetAsBox(getSprite()->getContentSize().width / PTM_RATIO / 2,
						  getSprite()->getContentSize().height / PTM_RATIO / 2,
						  b2Vec2(0.85, 0.85), 0); // need to divide by 2 for some reason

		boxShapeDef.shape = &boxShape;
		boxShapeDef.density = 10.0f;
		boxShapeDef.friction = 0.0f;
		boxShapeDef.restitution = 0.0f;
		boxBody->CreateFixture(&boxShapeDef);
	}
}

void ABox::setPosition(const Point &point){
	Node::setPosition(point);
	boxBody->SetTransform(b2Vec2(point.x / PTM_RATIO, point.y / PTM_RATIO), 0);
}


void ABox::visit(){
	Node::setPosition(ccp(boxBody->GetPosition().x*PTM_RATIO, boxBody->GetPosition().y*PTM_RATIO));

	sprite->setPosition(getPosition());
	sprite->setAnchorPoint(getAnchorPoint());
	sprite->setZOrder(getZOrder());
	sprite->visit();
	if (oldSprite != nullptr)
	{
		oldSprite->setPosition(getPosition());
		oldSprite->setAnchorPoint(getAnchorPoint());
		oldSprite->setZOrder(getZOrder());
		oldSprite->visit();
	}
}

void ABox::kill() {
	oldSprite = sprite;

	sprite = Sprite::create();
	sprite->initWithFile("dead.png");
	addChild(sprite);

	type = Dead;

	CCFadeOut* fade = CCFadeOut::create(0.2f);
	oldSprite->runAction(fade);

	boxBody->SetType(b2_staticBody);
}

ABox::~ABox(){
	myWorld->DestroyBody(boxBody);
	removeChild(sprite);
	if (oldSprite) {
		removeChild(oldSprite);
	}
}