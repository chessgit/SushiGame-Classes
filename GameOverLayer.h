#ifndef GameOverLayer_H_
#define GameOverLayer_H_

#include "cocos2d.h"
USING_NS_CC;

class GameOverLayer :public Layer
{
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameOverLayer);
	void menuCloseCallback(cocos2d::Ref* pSender);
private:
	//接受PlayLayer score 的消息的回调函数
	void scoreMsg(Ref* pData);
};
#endif
