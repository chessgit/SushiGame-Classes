/*
�� SushiSprite �ṩ�Ĺ��ܣ�

1.�����У�row�����У�col�����������ͬ�������˾����
create(int row, int col)
2.�õ�һ����˾������
getContentWidth

*/
#ifndef __SushiSprite_H__
#define __SushiSprite_H__

#include "cocos2d.h"

USING_NS_CC;

// DisplayMode ����˾����ʾģʽ
typedef enum{
	DISPLAY_MODE_NORMAL = 0,//�������
	DISPLAY_MODE_HORIZONTAL,//���� 4 ��
	DISPLAY_MODE_VERTICAL,  //���� 4 ��
} DisplayMode;


class SushiSprite :  public Sprite
{
public:
	//������˾����
	static SushiSprite *create(int row, int col);

	//�õ���˾������
    static float getContentWidth();
    
	//  CC_SYNTHESIZE(int, m_row, Row)  ������: ����һ�� int �͵� m_row ������һ��������ΪgetRow�Լ�setRow�ĺ�����
    CC_SYNTHESIZE(int, m_row, Row);//��
    CC_SYNTHESIZE(int, m_col, Col);//��
    CC_SYNTHESIZE(int, m_imgIndex, ImgIndex);
	CC_SYNTHESIZE(bool, m_isNeedRemove, IsNeedRemove);//�����Ҫ����������˾
	CC_SYNTHESIZE(bool, m_ignoreCheck, IgnoreCheck);//����²�����4����˾����Ϊtrueʱ��ʾ���Ա��ֶ���˾�ļ��
	CC_SYNTHESIZE_READONLY(DisplayMode, m_displayMode, DisplayMode);//ֻ������ getDisplayMode �ķ���
	void setDisplayMode(DisplayMode mode);
};

#endif /* defined(__SushiSprite_H__) */
