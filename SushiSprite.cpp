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

//�õ���˾�������
float SushiSprite::getContentWidth()
{
    static float itemWidth = 0;
    if (0 == itemWidth) {
        Sprite *sprite = CCSprite::createWithSpriteFrameName(sushiNormal[0]);
        itemWidth = sprite->getContentSize().width;
    }
    return itemWidth;
}

//������˾����
SushiSprite *SushiSprite::create(int row, int col)
{
	SushiSprite *sushi = new SushiSprite();
	sushi->m_row = row;
	sushi->m_col = col;
	//srand((unsigned)time(NULL));//srand()��������һ���Ե�ǰʱ�俪ʼ���������
	//sushi->m_imgIndex = rand() % TOTAL_SUSHI;//MAXΪ���ֵ���������Ϊ0~MAX-1
	sushi->m_imgIndex = CCRANDOM_0_1() * TOTAL_SUSHI;
    sushi->initWithSpriteFrameName(sushiNormal[sushi->m_imgIndex]);
	sushi->autorelease();
	return sushi;
}