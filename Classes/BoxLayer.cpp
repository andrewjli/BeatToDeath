#include "BoxLayer.h"
#include "B2DebugDrawLayer.h"
USING_NS_CC;

#define PTM_RATIO 30

bool BoxLayer::init()
{
	killPlayerNextLoop = false;
	canJump = false;
	Size screenSize = Director::getInstance()->getWinSize();

	// box2d shit
	b2Vec2 gravity;
	gravity.Set(0.0f, -30.0f);
	boolean doSleep = true;
	_world = new b2World(gravity);
	_world->SetAllowSleeping(true);
	_world->SetContinuousPhysics(false);
	_world->SetContactListener(this);
	//addChild(B2DebugDrawLayer::create(_world, PTM_RATIO), 9999);

	/* Ground stuff
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);

	_groundBody = _world->CreateBody(&groundBodyDef);

	b2EdgeShape groundShape;
	groundShape.Set(b2Vec2(0, 0), b2Vec2(screenSize.width / PTM_RATIO, 0));
	_groundBody->CreateFixture(&groundShape, 0);
	*/

	spawnPlayer();
	
	scheduleUpdate();
	return true;
}

void BoxLayer::initFixedBoxes(std::vector<std::pair<Point, BoxType>> boxInput){
	for (int i = 0; i < boxInput.size(); i++) {
		std::pair<Point, BoxType> p = boxInput[i];
		ABox* box = new ABox(p.second, _world);
		box->setPosition(p.first);

		boxes.push_back(box);
		addChild(box);
	}
}

void BoxLayer::update(float delta){
	_world->Step(delta, 8, 1);
	
	ABox* player = getPlayer();
	Point playerPosition = player->getPosition();
	Point goalPosition = getGoal()->getPosition();

	if (killPlayerNextLoop)
	{
		killPlayer(true);
		killPlayerNextLoop = false;
	}

	if (playerPosition.y < 0) {
		killPlayer(false);
	}

	if (!toDelete.empty()) {
		for (int i = toDelete.size() - 1; i >= 0; i--) {
			removeChild(toDelete[i]);
			toDelete[i]->release();
		}
		toDelete.clear();
	}
}

ABox* BoxLayer::getPlayer() {
	for (int i = boxes.size()-1; i >= 0; i--) {
		if (boxes[i]->getType() == Player) {
			return boxes[i];
		}
	}
}

ABox* BoxLayer::getGoal() {
	for (int i = boxes.size() - 1; i >= 0; i--) {
		if (boxes[i]->getType() == Goal) {
			return boxes[i];
		}
	}
}

void BoxLayer::spawnPlayer() {
	canJump = true;
	ABox* player = new ABox(Player, _world);
	player->setPosition(ccp(100, 150));
	boxes.push_back(player);
	addChild(player);
}

void BoxLayer::killPlayer(bool newBody) {
	for (int i = boxes.size() - 1; i >= 0; i--) {
		ABox* player = boxes[i];
		if (player->getType() == Player) {
			if (newBody) {
				player->kill();
			} else {
				toDelete.push_back(player);
				boxes.erase(boxes.begin() + i);
			}
		}
	}
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
		"boom.wav");
	spawnPlayer();
}
bool BoxLayer::canPlayerBeKilled(){
	return getPlayer()->getPosition().x > 151;
}

void BoxLayer::resetBodies(){
	for (int i = boxes.size() - 1; i >= 0; i--) {
		ABox* dead = boxes[i];
		if (dead->getType() == Dead) {
			toDelete.push_back(dead);
			boxes.erase(boxes.begin() + i);
		}
	}
}

void BoxLayer::movePlayer(InputDirection direction){
	ABox* player = getPlayer();
	b2Vec2 vel = player->getBoxBody()->GetLinearVelocity();
	if (direction == UP) {
		if (canJump) {
			canJump = false;
			vel.y = 15;//upwards - don't change x velocity
			player->getBoxBody()->SetLinearVelocity(vel);
		}
	}
	else if (direction == LEFT) {
		vel.x = -8;
		player->getBoxBody()->SetLinearVelocity(vel);
	}
	else if (direction == RIGHT) {
		vel.x = 8;
		player->getBoxBody()->SetLinearVelocity(vel);
	}
	else if (direction == DOWN) {
		killPlayer(true);
	}
}

void BoxLayer::stopHorizontalMovement(){
	ABox* player = getPlayer();
	player->getBoxBody()->SetLinearVelocity(b2Vec2(0, player->getBoxBody()->GetLinearVelocity().y));
}

void BoxLayer::BeginContact(b2Contact *contact) {
	ABox* box1 = static_cast<ABox*>(contact->GetFixtureA()->GetBody()->GetUserData());
	ABox* box2 = static_cast<ABox*>(contact->GetFixtureB()->GetBody()->GetUserData());

	b2Body* test1 = static_cast<b2Body*>(contact->GetFixtureA()->GetUserData());
	b2Body* test2 = static_cast<b2Body*>(contact->GetFixtureB()->GetUserData());
	for (int i = 0; i < 2; i++)
	{
		if (test1 != nullptr || test2 != nullptr) {
			canJump = true;
		}
		if (box1->getType() == Player && box2->getType() == Goal)
		{
			//resetBodies();
			Size winSize = CCEGLView::sharedOpenGLView()->getFrameSize();
			CCSprite* s = CCSprite::create();
			s->initWithFile("youwin.png");
			s->setScale(0.8);
			s->setPosition(ccp(winSize.width / 2, winSize.height / 2));
			addChild(s);
		}
		else if (box1->getType() == Player && box2->getType() == Kill)
		{
			killPlayerNextLoop = true;
		}
		ABox* box3 = box2;
		box2 = box1;
		box1 = box3;
	}
}

void BoxLayer::EndContact(b2Contact *contact) {
	b2Body* test1 = static_cast<b2Body*>(contact->GetFixtureA()->GetUserData());
	b2Body* test2 = static_cast<b2Body*>(contact->GetFixtureB()->GetUserData());
	for (int i = 0; i < 2; i++)
	{
		if (test1 != nullptr || test2 != nullptr) {
			canJump = false;
		}
	}
}


BoxLayer::~BoxLayer(){
	//TODO: remove all boxes
}