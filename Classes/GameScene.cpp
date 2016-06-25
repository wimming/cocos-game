#include "GameScene.h"
#include "Definitions.h"
#include "GameOverScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics( );
    scene->getPhysicsWorld( )->setDebugDrawMask( PhysicsWorld::DEBUGDRAW_ALL );
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();
    layer->SetPhysicsWorld(scene->getPhysicsWorld());

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    auto backgroundSprite = Sprite::create( "Background.png" );
    backgroundSprite->setPosition( Point( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ) );
    
    this->addChild( backgroundSprite );
    
    auto edgeBody = PhysicsBody::createEdgeBox( visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3 );
    edgeBody->setCollisionBitmask( OBSTACLE_COLLISION_BITMASK );
    edgeBody->setContactTestBitmask( true );
    
    auto edgeNode = Node::create();
    edgeNode->setPosition( Point( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ) );
    
    edgeNode->setPhysicsBody( edgeBody );
    
    this->addChild( edgeNode );
    
    this->schedule( schedule_selector( GameScene::newEnemy), PIPE_SPAWN_FREQUENCY * visibleSize.width );
    
    player = Player::create();
	addChild(player);
    
    auto contactListener = EventListenerPhysicsContact::create( );
    contactListener->onContactBegin = CC_CALLBACK_1( GameScene::onContactBegin, this );
    Director::getInstance( )->getEventDispatcher( )->addEventListenerWithSceneGraphPriority( contactListener, this );
    
    auto touchListener = EventListenerTouchOneByOne::create( );
    touchListener->setSwallowTouches( true );
    touchListener->onTouchBegan = CC_CALLBACK_2( GameScene::onTouchBegan, this );
    Director::getInstance( )->getEventDispatcher( )->addEventListenerWithSceneGraphPriority( touchListener, this );

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    
    score = 0;
	scoreLock = 10;
    
    __String *tempScore = __String::createWithFormat( "%i", score );
    
    scoreLabel = Label::createWithTTF( tempScore->getCString( ), "fonts/Marker Felt.ttf", visibleSize.height * SCORE_FONT_SIZE );
    scoreLabel->setColor( Color3B::WHITE );
    scoreLabel->setPosition( Point( visibleSize.width / 2 + origin.x, visibleSize.height * 0.9 + origin.y ) );
    
    this->addChild( scoreLabel, 10000 );
    
    this->scheduleUpdate( );
    
    return true;
}

void GameScene::newEnemy( float dt )
{
	this->addChild(EnemyGenerator::getInstance()->GenerateEnemy());
}

bool GameScene::onContactBegin( cocos2d::PhysicsContact &contact )
{
	Node * a = contact.getShapeA()->getBody()->getNode();
    Node * b = contact.getShapeB()->getBody()->getNode();
    
    if (a->getTag() == ENEMY_TAG && b->getTag() == PLAYER_TAG || b->getTag() == ENEMY_TAG && a->getTag() == PLAYER_TAG)
    {        
        auto scene = GameOverScene::createScene( score );
        Director::getInstance( )->replaceScene( TransitionFade::create( TRANSITION_TIME, scene ) );
    }
    
    return true;
}

bool GameScene::onTouchBegan( cocos2d::Touch *touch, cocos2d::Event *event )
{
	player->Fly();
    return true;
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
		player->Fly();
	}
}

void GameScene::update( float dt )
{
	if (scoreLock <= 0) {
		scoreLock = 10;
		score += 1;
		scoreLabel->setString(String::createWithFormat("%i", score)->_string);
	}
	else {
		scoreLock -= dt*100;
	}

	player->Fall( );
	EnemyGenerator::getInstance()->removeEnemys();

	if (player->getPosition().y < 0 || player->getPosition().y > visibleSize.height) {
		auto scene = GameOverScene::createScene(score);
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
	}

}
