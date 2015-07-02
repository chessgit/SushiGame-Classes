#ifndef FirstLayer_H_
#define FirstLayer_H_

#include "cocos2d.h"
USING_NS_CC;

class FirstLayer:public Layer
{
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(FirstLayer);
	void menuCloseCallback(cocos2d::Ref* pSender);
private:

};



#endif