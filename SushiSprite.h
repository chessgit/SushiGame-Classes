/*
类 SushiSprite 提供的功能：

1.输入行（row），列（col）随机创建不同种类的寿司精灵
create(int row, int col)
2.得到一个寿司精灵宽度
getContentWidth

*/
#ifndef __SushiSprite_H__
#define __SushiSprite_H__

#include "cocos2d.h"

USING_NS_CC;

// DisplayMode 是寿司的显示模式
typedef enum{
	DISPLAY_MODE_NORMAL = 0,//正常情况
	DISPLAY_MODE_HORIZONTAL,//横向 4 个
	DISPLAY_MODE_VERTICAL,  //纵向 4 个
} DisplayMode;


class SushiSprite :  public Sprite
{
public:
	//创建寿司精灵
	static SushiSprite *create(int row, int col);

	//得到寿司精灵宽度
    static float getContentWidth();
    
	//  CC_SYNTHESIZE(int, m_row, Row)  的作用: 声明一个 int 型的 m_row 变量和一个函数名为getRow以及setRow的函数。
    CC_SYNTHESIZE(int, m_row, Row);//行
    CC_SYNTHESIZE(int, m_col, Col);//列
    CC_SYNTHESIZE(int, m_imgIndex, ImgIndex);
	CC_SYNTHESIZE(bool, m_isNeedRemove, IsNeedRemove);//标记需要被消除的寿司
	CC_SYNTHESIZE(bool, m_ignoreCheck, IgnoreCheck);//标记新产生的4消寿司，当为true时表示忽略本轮对寿司的检测
	CC_SYNTHESIZE_READONLY(DisplayMode, m_displayMode, DisplayMode);//只设置了 getDisplayMode 的方法
	void setDisplayMode(DisplayMode mode);
};

#endif /* defined(__SushiSprite_H__) */
