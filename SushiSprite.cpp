#include "SushiSprite.h"

USING_NS_CC;

#define TOTAL_SUSHI (6)

static const std::string sushiNormal[TOTAL_SUSHI] = {
	"sushi_1n.png",
	"sushi_2n.png",
	"sushi_3n.png",
	"sushi_4n.png",
	"sushi_5n.png",
    "sushi_6n.png"
};

//得到寿司精灵宽度
float SushiSprite::getContentWidth()
{
    static float itemWidth = 0;
    if (0 == itemWidth) {
        Sprite *sprite = CCSprite::createWithSpriteFrameName(sushiNormal[0]);
        itemWidth = sprite->getContentSize().width;
    }
    return itemWidth;
}

//创建寿司精灵
SushiSprite *SushiSprite::create(int row, int col)
{
	SushiSprite *sushi = new SushiSprite();
	sushi->m_row = row;
	sushi->m_col = col;
	//srand((unsigned)time(NULL));//srand()函数产生一个以当前时间开始的随机种子
	//sushi->m_imgIndex = rand() % TOTAL_SUSHI;//MAX为最大值，其随机域为0~MAX-1
	sushi->m_imgIndex = CCRANDOM_0_1() * TOTAL_SUSHI;
    sushi->initWithSpriteFrameName(sushiNormal[sushi->m_imgIndex]);
	sushi->autorelease();
	return sushi;
}