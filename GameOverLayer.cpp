#include "GameOverLayer.h"
#include "FirstLayer.h"
#include "PlayLayer.h"
Scene* GameOverLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameOverLayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameOverLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = (*Director::getInstance()).getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(GameOverLayer::menuCloseCallback, this));

	closeItem->setPosition(Point(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 3));

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 1);

	//订阅消息类型为score的消息，不传递数据
	 int i;
	NotificationCenter::getInstance()->addObserver(
		this,
		callfuncO_selector(GameOverLayer::scoreMsg),
		"score",
		NULL
		);

	auto label = LabelTTF::create(" game over\nplay again?", "Arial", 24);


	// position the label on the center of the screen
	label->setPosition(Point(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));

	// add the label as a child to this layer
	this->addChild(label, 1);
	auto background = Sprite::create("mbackground.png");
	background->setAnchorPoint(Point(0, 0));
	background->setPosition(Point(0, 0));
	this->addChild(background);

	return true;
}

void GameOverLayer::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->replaceScene(FirstLayer::createScene());

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void GameOverLayer::scoreMsg(Ref* pData)
{
	log("pData:%d",pData);
	/*	Text* finalScore; 
	finalScore->setText(Value(pData).asString());
	this->addChild(finalScore);*/

}
