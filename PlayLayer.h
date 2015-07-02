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
	bool m_isTouchEnable;		// 标志知否处理触摸事件
	bool m_isNeedFillVacancies;	 // 标志是否有空缺需要填补
	bool m_movingVertical;		// true: 4消除产生纵向炸弹.  false: 4消除产生横向炸弹.
	bool m_isAnimationing;		 // 标志是否在移动
	

	int m_width; //初始化矩阵的宽和高，如 8 行（height）， 6列 （width）
	int m_height;
	float m_matrixLeftBottomX; // 单个寿司左下角的点
	float m_matrixLeftBottomY;
	float m_totalTime;
	SushiSprite *m_srcSushi;	// 存源寿司
	SushiSprite *m_destSushi;	// 存目的寿司
	SushiSprite **m_matrix;		// 二维数组存寿司 (SushiSprite *)，存一系列寿司
	Text* m_scoreLab;	//分数标签
	LoadingBar* m_timeBar;	//时间条
    SpriteBatchNode *spriteSheet;	// 寿司列表 
    
    void initMatrix();//初始化寿司矩阵
	void loadUI(); //加载计分板UI
	void initBG();	//初始化背景
    void createAndDropSushi(int row, int col);//创建下落寿司
    void checkAndRemoveChain(); // 检测是否在移动
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
	CC_SYNTHESIZE_READONLY(int, m_iScore, Score); //声明了int m_iScore 和 int getScore
//	SushiSprite *sushiOfPoint(Point *point);
	Point positionOfItem(int row, int col);	//得到对应行列精灵的坐标值


};

#endif /* defined(__PlayLayer_H__) */
