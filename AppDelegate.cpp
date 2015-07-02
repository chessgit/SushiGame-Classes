#include "AppDelegate.h"
#include "PlayLayer.h"
#include "FirstLayer.h"
#include "HelloWorldScene.h"
USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
		glview = GLView::create("Jing's game");
		director->setOpenGLView(glview);
		glview->setFrameSize(320.0, 480.0);
		//glview->setFrameSize(480.0, 480.0);
	}

	std::vector<std::string> searchPath;
	searchPath.push_back("w640");
	FileUtils::getInstance()->setSearchPaths(searchPath);
	director->setContentScaleFactor(640.0 / 320.0);
	glview->setDesignResolutionSize(320.0, 480.0, ResolutionPolicy::FIXED_WIDTH);

	//director->setContentScaleFactor(640.0 / 320.0);
	/*	glview->setDesignResolutionSize(480.0f, 320.0f, ResolutionPolicy::SHOW_ALL);
	std::vector<std::string> searchPath;
	searchPath.push_back("height_864");
	CCFileUtils::getInstance()->setSearchPaths(searchPath);
	director->setContentScaleFactor(864.0f / 320.0f);*/


	// turn on display FPS
	director->setDisplayStats(false);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

	// create a scene. it's an autorelease object
	auto scene = FirstLayer::createScene();

	// run
	director->runWithScene(scene);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();

	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
