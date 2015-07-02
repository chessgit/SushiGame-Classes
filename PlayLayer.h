#ifndef __PlayLayer_H__
#define __PlayLayer_H__

#include "cocos2d.h"
#include "editor-support\cocostudio\CCSGUIReader.h"
#include "ui\CocosGUI.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;
using namespace cocos2d::ui;
using namespace cocostudio;
USING_NS_CC;

class SushiSprite;

class PlayLayer : public Layer
{
public:
    PlayLayer();
    ~PlayLayer();
    static Scene* createScene();
    CREATE_FUNC(PlayLayer);
    
    virtual bool init() override;
    virtual void update(float dt) override;
	virtual bool onTouchBegan(Touch *touch, Event *unused) override;
	virtual void onTouchMoved(Touch *touch, Event *unused) override;
private:
	bool m_isTouchEnable;		// ��־֪�������¼�
	bool m_isNeedFillVacancies;	 // ��־�Ƿ��п�ȱ��Ҫ�
	bool m_movingVertical;		// true: 4������������ը��.  false: 4������������ը��.
	bool m_isAnimationing;		 // ��־�Ƿ����ƶ�
	

	int m_width; //��ʼ������Ŀ�͸ߣ��� 8 �У�height���� 6�� ��width��
	int m_height;
	float m_matrixLeftBottomX; // ������˾���½ǵĵ�
	float m_matrixLeftBottomY;
	float m_totalTime;
	SushiSprite *m_srcSushi;	// ��Դ��˾
	SushiSprite *m_destSushi;	// ��Ŀ����˾
	SushiSprite **m_matrix;		// ��ά�������˾ (SushiSprite *)����һϵ����˾
	Text* m_scoreLab;	//������ǩ
	LoadingBar* m_timeBar;	//ʱ����
    SpriteBatchNode *spriteSheet;	// ��˾�б� 
    
    void initMatrix();//��ʼ����˾����
	void loadUI(); //���ؼƷְ�UI
	void initBG();	//��ʼ������
    void createAndDropSushi(int row, int col);//����������˾
    void checkAndRemoveChain(); // ����Ƿ����ƶ�
	void getColChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList); 
    void getRowChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList);
    void removeSushi(std::list<SushiSprite *> &sushiList); 
    void explodeSushi(SushiSprite *sushi);
    void fillVacancies();
	void FloodSeedFill(int x, int y, SushiSprite* sushi);
	void swapSushi();
	void timeUpdate(float dt);
	void setScore();
	void sendMsg();
	CC_SYNTHESIZE_READONLY(int, m_iScore, Score); //������int m_iScore �� int getScore
//	SushiSprite *sushiOfPoint(Point *point);
	Point positionOfItem(int row, int col);	//�õ���Ӧ���о��������ֵ


};

#endif /* defined(__PlayLayer_H__) */
