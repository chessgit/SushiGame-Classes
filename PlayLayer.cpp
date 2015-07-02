#include "PlayLayer.h"
#include "SushiSprite.h"
#include "GameOverLayer.h"

#define MATRIX_WIDTH (6)
#define MATRIX_HEIGHT (8)

#define SUSHI_GAP (1)
typedef struct tagDIRECTION
{
	int x_offset;
	int y_offset;
}DIRECTION;

DIRECTION direction_4[] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };
PlayLayer::PlayLayer()
: spriteSheet(NULL)
, m_matrix(NULL)
, m_width(0)
, m_height(0)
, m_matrixLeftBottomX(0)
, m_matrixLeftBottomY(0)
, m_isAnimationing(true)//标志是否开始下落动画
, m_isTouchEnable(true)	// 标志知否处理触摸事件
, m_movingVertical(true)//true: 4消除产生纵向炸弹.  false: 4消除产生横向炸弹.
, m_iScore(0) //分数：0
, m_totalTime(0.0) //update中累计游戏时间
{
}

PlayLayer::~PlayLayer()
{
    if (m_matrix) {
        free(m_matrix);
    }
}

Scene *PlayLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = PlayLayer::create();
    scene->addChild(layer);
    return scene;
}

void PlayLayer::initBG()
{
	//Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto background = Sprite::create("mbackground.png");
	background->setAnchorPoint(Point(1, 1));
	background->setPosition(Point(visibleSize.width, visibleSize.height));
	this->addChild(background,0);
}

void PlayLayer::loadUI()
{
	auto UI = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("LitterRunnerUI_1.ExportJson");
	UI->setTouchEnabled(false);
	this->addChild(UI);

	//获取控件对象
	m_scoreLab = (Text*)Helper::seekWidgetByName(UI,"scoreLab");
	m_timeBar = (LoadingBar*)Helper::seekWidgetByName(UI,"hpProgress");
}

bool PlayLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
	// 创建游戏背景
	initBG();

	//加载UI
	loadUI();

	// 初始化寿司精灵表单
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sushi.plist");
    spriteSheet = SpriteBatchNode::create("sushi.pvr.ccz");
    addChild(spriteSheet);
    
	// 初始化矩阵的宽和高，即：寿司的长宽个数。MATRIX_WIDTH、MATRIX_HEIGHT通过宏定义
    m_width = MATRIX_WIDTH;
    m_height = MATRIX_HEIGHT;
    
	// 初始化寿司矩阵左下角的点
	Size visibleSize = Director::getInstance()->getVisibleSize();
	m_matrixLeftBottomX = (visibleSize.width - SushiSprite::getContentWidth() * m_width - (m_width - 1) * SUSHI_GAP) / 2;
	m_matrixLeftBottomY = (visibleSize.height - SushiSprite::getContentWidth() * m_height - (m_height - 1) * SUSHI_GAP) / 2;
    
	// 初始化数组
    int arraySize = sizeof(SushiSprite *) * m_width * m_height;//数组尺寸大小
	//为 m_matrix 分配内存，这里 m_matrix 是指向指针类型的指针，其定义为：SushiSprite **m_matrix。所以可以理解为 m_matrix 是一个指针数组
    m_matrix = (SushiSprite **)malloc(arraySize);
    memset((void*)m_matrix, 0, arraySize);//初始化数组 m_matrix
    
	/*  memset 补充

	void *memset(void *s, int ch, size_t n);
	函数解释：s 指向的某一块内存,将 s 中前 n 个字节 （typedef unsigned int size_t ）用 ch 指定的ASCII值 替换并返回 s 。
	memset：作用是在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法[1] 。

	*/
	//初始化寿司矩阵
    initMatrix();

	//调用定时器
    scheduleUpdate();
	this->schedule(schedule_selector(PlayLayer::timeUpdate), 1.0f);

	//创建并绑定触摸机制
	// 1. 创建一个事件监听器，Onebyone 为单点触摸
	auto touchListener = EventListenerTouchOneByOne::create();
	// 2. 绑定触摸事件
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this); // 触摸开始时触发
	touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this); // 触摸移动时触发
	// 3. 添加监听器
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);// addEventListenerWithSceneGraphPriority(EventListener *listener,Node *node); 事件监听器优先级为 0
	//而 _eventDispatcher->addEventListenerWithFixedPriority(EventListenr *listener,int fixedPriority)的优先级由我们设置，但不能为 0 
	/*		
		eventDispatcher事件分发器，
			1. eventDispatcher 是 Node 的属性，
			2. 通过它我们可以统一管理当前节点（如：场景、层、精灵等）的所有事件分发情况。
			3. 但是它本身是一个单例模式值的引用，在 Node 构造函数中，通过 “Director::getInstance()->getEventDispatcher();” 获取，有了这个属性，我们能更为方便的调用。
	*/
    return true;
}

void PlayLayer::timeUpdate(float dt)
{
	m_totalTime += 2.0f;
	m_timeBar->setPercent(100.0 - m_totalTime);
}

void PlayLayer::update(float dt)
{
	if (m_timeBar->getPercent() <= 0)
	{
		sendMsg();
		Director::getInstance()->replaceScene(GameOverLayer::createScene());
	}
    // check if animationing,m_isAnimationing(true)
    if (m_isAnimationing) 
	{
        // init with false
        m_isAnimationing = false;
        for (int i = 0; i < m_height * m_width; i++) 
		{
            SushiSprite *sushi = m_matrix[i];
            if (sushi && sushi->getNumberOfRunningActions() > 0) 
			{//getNumberOfRunningActions()方法可以获取sushi正在执行中的动作个数
                m_isAnimationing = true;
                break;
            }
        }
    }

	// if sushi is moving, ignore use touch event
	m_isTouchEnable = !m_isAnimationing;

    if (!m_isAnimationing) {
      //  checkAndRemoveChain();
    }
}
/*void PlayLayer::checkAndRemoveChain()
{
	//log("checkAndRemoveChain");
	for (int i = 0; i < m_height * m_width; i++) 
	{
		SushiSprite *sushi = m_matrix[i];
		if (!sushi) 
		{
			continue;
		}

		// start count chain
		std::list<SushiSprite *> colChainList;
		getColChain(sushi, colChainList);

		std::list<SushiSprite *> rowChainList;
		getRowChain(sushi, rowChainList);

		std::list<SushiSprite *> &longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
		if (longerList.size() == 3) 
		{
			removeSushi(longerList);
			return;
		}
		if (longerList.size() > 3) 
		{
			//TODO: make a special sushi can clean a line, and remove others
			removeSushi(longerList);
			return;
		}
	}
}*/
void PlayLayer::FloodSeedFill(int x, int y, SushiSprite* sushi)
{
	if (x >= 0 && x < m_height && y >= 0 && y < m_width)
	{
		SushiSprite *neighborSushi = m_matrix[x * m_width + y];
		if (neighborSushi->getImgIndex() == sushi->getImgIndex())
		{
			if (neighborSushi->getIgnoreCheck())return;
			for (int i = 0; i < 4; i++)
			{
				neighborSushi->setIgnoreCheck(true);
				FloodSeedFill(x + direction_4[i].x_offset, y + direction_4[i].y_offset, sushi);
			}
		}
	}

}
void PlayLayer::checkAndRemoveChain()
{
	//log("checkAndRemoveChain");
		SushiSprite *sushi = m_srcSushi;
		if (!sushi)
		{
			return;
		}
		std::list<SushiSprite*> sameList;
		FloodSeedFill(sushi->getRow(),sushi->getCol(),sushi);

		for (int i = 0; i < m_width*m_height; i++)
		{
			if (m_matrix[i]->getIgnoreCheck())
			{
				sameList.push_back(m_matrix[i]);
			}
		}
		m_srcSushi = nullptr;//不然有上个源结点的bug 可以弄个新的功能
		if (sameList.size()>2)
		{

			removeSushi(sameList);
		}
		return;
		/*		// start count chain
		std::list<SushiSprite *> colChainList;
		getColChain(sushi, colChainList);

		std::list<SushiSprite *> rowChainList;
		getRowChain(sushi, rowChainList);

		std::list<SushiSprite *> &longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
		if (longerList.size() == 3)
		{
			removeSushi(longerList);
			return;
		}
		if (longerList.size() > 3)
		{
			//TODO: make a special sushi can clean a line, and remove others
			removeSushi(longerList);
			return;
		}*/

	
}



void PlayLayer::getColChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{//横向检测每一列
	
	chainList.push_back(sushi);// 插入第一个寿司精灵

	//向前检测相同图标值（ImgIndex）的寿司
	int neighborCol = sushi->getCol() - 1;//sushi前一列寿司所在列数值
	while (neighborCol >= 0) {
		SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
			chainList.push_back(neighborSushi);//插入该“邻居”寿司
			neighborCol--;//继续向前
		}
		else {
			break;
		}
	}
	//向后检测相同图标值（ImgIndex）的寿司
	neighborCol = sushi->getCol() + 1;
	while (neighborCol < m_width) {
		SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
			chainList.push_back(neighborSushi);
			neighborCol++;
		}
		else {
			break;
		}
	}
}
void PlayLayer::getRowChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{
	
    chainList.push_back(sushi);// add first sushi
    
    int neighborRow = sushi->getRow() - 1;
    while (neighborRow >= 0) {
        SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
            chainList.push_back(neighborSushi);
            neighborRow--;
        } else {
            break;
        }
    }
    
    neighborRow = sushi->getRow() + 1;
    while (neighborRow < m_height) {
        SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
        if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
            chainList.push_back(neighborSushi);
            neighborRow++;
        } else {
            break;
        }
    }
}

void PlayLayer::fillVacancies()
{
    Size size = CCDirector::getInstance()->getWinSize();
    int *colEmptyInfo = (int *)malloc(sizeof(int) * m_width);
    memset((void *)colEmptyInfo, 0, sizeof(int) * m_width);
    
    // 1. drop exist sushi
    SushiSprite *sushi = NULL;
    for (int col = 0; col < m_width; col++) {
        int removedSushiOfCol = 0;
        // from buttom to top
        for (int row = 0; row < m_height; row++) {
            sushi = m_matrix[row * m_width + col];
            if (NULL == sushi) {
                removedSushiOfCol++;
            } else {
                if (removedSushiOfCol > 0) {
                    // evey item have its own drop distance
                    int newRow = row - removedSushiOfCol;
                    // switch in matrix
                    m_matrix[newRow * m_width + col] = sushi;
                    m_matrix[row * m_width + col] = NULL;
                    // move to new position
                    Point startPosition = sushi->getPosition();
                    Point endPosition = positionOfItem(newRow, col);
                    float speed = (startPosition.y - endPosition.y) / size.height;
                    sushi->stopAllActions();// must stop pre drop action
                    sushi->runAction(CCMoveTo::create(speed, endPosition));
                    // set the new row to item
                    sushi->setRow(newRow);
                }
            }
        }
        
        // record empty info
        colEmptyInfo[col] = removedSushiOfCol;
    }
    
    // 2. create new item and drop down.
    for (int col = 0; col < m_width; col++) {
        for (int row = m_height - colEmptyInfo[col]; row < m_height; row++) {
            createAndDropSushi(row, col);
        }
    }
    
    free(colEmptyInfo);
}

void PlayLayer::removeSushi(std::list<SushiSprite *> &sushiList)
{
	
    // make sequence remove
    m_isAnimationing = true;
    
    std::list<SushiSprite *>::iterator itList;
    for (itList = sushiList.begin(); itList != sushiList.end(); itList++) {
        SushiSprite *sushi = (SushiSprite *)*itList;
        // remove sushi from the metrix
        m_matrix[sushi->getRow() * m_width + sushi->getCol()] = NULL;
        explodeSushi(sushi);
    }
    
    // drop to fill empty
    fillVacancies();
}

void PlayLayer::explodeSushi(SushiSprite *sushi)
{
	//log("explodeSushi ture");
    float time = 0.3;
    
    // 1. action for sushi
    sushi->runAction(Sequence::create(
                                      ScaleTo::create(time, 0.0),
                                      CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sushi)),
                                      NULL));
	
    // 2. action for circle
    auto circleSprite = Sprite::create("circle.png");
	addChild(circleSprite, 10);
	circleSprite->setPosition(sushi->getPosition());
	circleSprite->setScale(0);// start size
    circleSprite->runAction(Sequence::create(ScaleTo::create(time, 1.0),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite)),
                                             NULL));

    // 3. particle effect
	auto particleStars = ParticleSystemQuad::create("stars.plist");
    particleStars->setAutoRemoveOnFinish(true);
	particleStars->setBlendAdditive(false);
	particleStars->setPosition(sushi->getPosition());
	particleStars->setScale(0.3);
    addChild(particleStars, 20);

	//消除加分
	setScore();
}

//设置得分
void PlayLayer::setScore()
{
	m_iScore += 100;
	m_scoreLab->setText(Value(m_iScore).asString());
}
//矩阵的初始化
void PlayLayer::initMatrix()
{//row 是行 ,col 是列
    for (int row = 0; row < m_height; row++) {
		for (int col = 0; col < m_width; col++) {
            createAndDropSushi(row, col);
        }
    }
}

//创建SushiSprite，并下落到指定位置
void PlayLayer::createAndDropSushi(int row, int col)
{
    Size size = Director::getInstance()->getWinSize();
    
    SushiSprite *sushi = SushiSprite::create(row, col);
    
	// 创建并执行下落动画
    Point endPosition = positionOfItem(row, col);
    Point startPosition = Point(endPosition.x, endPosition.y + size.height /2);
    sushi->setPosition(startPosition);
	float speed = startPosition.y / (2 * size.height);
    sushi->runAction(MoveTo::create(speed, endPosition));

	//将寿司添加到精灵表单里。注意，如果没有添加到精灵表单里，而是添加到层里的话，就不会得到性能的优化。
    spriteSheet->addChild(sushi);

	//给指定位置的数组赋值
    m_matrix[row * m_width + col] = sushi;
}

//得到对应行列精灵的坐标值
Point PlayLayer::positionOfItem(int row, int col)
{
    float x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
    float y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
    return Point(x, y);
}
/*//获取触屏点所在的寿司,得到 m_srcSushi
SushiSprite *PlayLayer::sushiOfPoint(Point *point)//试试该 &point
{
	SushiSprite *sushi = nullptr;
	Rect rect = Rect(0, 0, 0, 0); // rect这个对象是用来存储成对出现的参数，比如，一个矩形框的左上角坐标、宽度和高度
	for (int i = 0; i < m_width*m_height; i++)
	{
		sushi = m_matrix[i];
		if (sushi)
		{
			rect.origin.x = sushi->getPositionX() - (sushi->getContentSize().width / 2);
			rect.origin.y = sushi->getPositionY() - (sushi->getContentSize().height / 2);
			rect.size = sushi->getContentSize();
			if (rect.containsPoint(*point))
				return sushi;
		}
	}
	return nullptr;
}

bool PlayLayer::onTouchBegan(Touch *touch, Event *unused)
{
	//log("onTouchBegan");
	m_srcSushi = nullptr;
	m_destSushi = nullptr;
	if (m_isTouchEnable)
	{
		auto location = touch->getLocation();
		m_srcSushi = sushiOfPoint(&location);
	}
	return m_isTouchEnable;
}*/

//
bool PlayLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	if (m_isTouchEnable)
	{
		SushiSprite *sushi = nullptr;
		for (int i = 0; i < m_width*m_height; i++)
		{
			sushi = m_matrix[i];
			if (sushi)
			{
				//将触点坐标, 转换为相对节点sushi的, 相对坐标
				auto point = sushi->convertTouchToNodeSpace(touch);

				//构造sushi的尺寸矩形
				Size size = sushi->getContentSize();
				Rect rect = Rect(0, 0, size.width, size.height);

				//判断触点是否触摸到sp内部
				if (rect.containsPoint(point)) {
					m_srcSushi = sushi;
				}
			}
		}
	}
	checkAndRemoveChain();
	return m_isTouchEnable;
}
//
//用 m_srcSushi （源寿司）求得 m_destSushi
void PlayLayer::onTouchMoved(Touch *touch, Event *unused)
{
	//log("onTouchMoved");
	if (!m_srcSushi || !m_isTouchEnable)
		return;
	// 定义一些变量节省些代码
	auto row = m_srcSushi->getRow();
	auto col = m_srcSushi->getCol();
	auto location = touch->getLocation();
	auto halfSushiWidth = m_srcSushi->getContentSize().width / 2;
	auto halfSushiHeight = m_srcSushi->getContentSize().height / 2;


	// 计算源寿司四周 rect
	// 源寿司上边寿司面积
	auto upRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
					   m_srcSushi->getPositionY() + halfSushiHeight,
					   m_srcSushi->getContentSize().width,
					   m_srcSushi->getContentSize().height);
	if (upRect.containsPoint(location))
	{
		row++;
		if (row < m_height)
		{
			m_destSushi = m_matrix[row*m_width + col];
		}
		m_movingVertical = true;//可能生成纵向的特殊寿司
		swapSushi();
		return;
	}

	auto downRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
		m_srcSushi->getPositionY() - (halfSushiHeight*3),
		m_srcSushi->getContentSize().width,
		m_srcSushi->getContentSize().height
		);
	if (downRect.containsPoint(location))
	{
		row--;
		if (row >= 0)
		{
			m_destSushi = m_matrix[row*m_width + col];
		}
		m_movingVertical = true;
		swapSushi();
		return;
	}

	auto leftRect = Rect(m_srcSushi->getPositionX() - (halfSushiWidth * 3),
		m_srcSushi->getPositionY() - halfSushiHeight,
		m_srcSushi->getContentSize().width,
		m_srcSushi->getContentSize().height
		);
	if (leftRect.containsPoint(location))
	{
		col--;
		if (col >= 0)
		{
			m_destSushi = m_matrix[row*m_width + col];
		}
		m_movingVertical = false;//可能生成横向的特殊寿司
		swapSushi();
		return;
	}

	auto rightRect = Rect(m_srcSushi->getPositionX() + halfSushiWidth,
		m_srcSushi->getPositionY() - halfSushiHeight,
		m_srcSushi->getContentSize().width,
		m_srcSushi->getContentSize().height
		);
	
	if (rightRect.containsPoint(location))
	{
		col++;
		if (col < m_width)
		{
			m_destSushi = m_matrix[row*m_width + col];
		}
		m_movingVertical = false;
		swapSushi();
		return;
	}
}

void PlayLayer::swapSushi()
{
	//log("swapSushi");
	m_isAnimationing = true;
	m_isTouchEnable = false;
	if (!m_srcSushi || !m_destSushi)
	{
		m_movingVertical = true;
		return;
	}
	Point posOfSrc = m_srcSushi->getPosition();
	Point posOfDest = m_destSushi->getPosition();
	float time = 0.2;

	// 1.交换 m_srcSushi 与 m_destSushi 在寿司矩阵的行列号, 和 m_srcSushi 与 m_destSushi 本身内容
	m_matrix[m_srcSushi->getRow()*m_width + m_srcSushi->getCol()] = m_destSushi;
	m_matrix[m_destSushi->getRow()*m_width + m_destSushi->getCol()] = m_srcSushi;
	int tmpRow = m_srcSushi->getRow();
	int tmpCol = m_srcSushi->getCol();
	m_srcSushi->setRow(m_destSushi->getRow());
	m_srcSushi->setCol(m_destSushi->getCol());
	m_destSushi->setRow(tmpRow);
	m_destSushi->setCol(tmpCol);

	// 2.检测交换后的 m_srcSushi 与 m_destSushi 在横纵方向上是否满足消除条件
	std::list<SushiSprite*> ColChainListOfFirst;
	getColChain(m_srcSushi, ColChainListOfFirst);

	std::list<SushiSprite*> RowChainListOfFirst;
	getRowChain(m_srcSushi, RowChainListOfFirst);

	std::list<SushiSprite*> ColChainListOfSecond;
	getColChain(m_destSushi, ColChainListOfSecond);

	std::list<SushiSprite*> RowChainListOfSecond;
	getRowChain(m_destSushi, RowChainListOfSecond);

	if (
		ColChainListOfFirst.size() >= 3
		|| RowChainListOfFirst.size() >= 3
		|| ColChainListOfSecond.size() >= 3
		|| RowChainListOfSecond.size() >= 3
		)
	{
		m_srcSushi->runAction(MoveTo::create(time, posOfDest));
		m_destSushi->runAction(MoveTo::create(time, posOfSrc));
		return;
	}
	m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
	m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
	tmpRow = m_srcSushi->getRow();
	tmpCol = m_srcSushi->getCol();
	m_srcSushi->setRow(m_destSushi->getRow());
	m_srcSushi->setCol(m_destSushi->getCol());
	m_destSushi->setRow(tmpRow);
	m_destSushi->setCol(tmpCol);

	m_srcSushi->runAction(Sequence::create(
		MoveTo::create(time, posOfDest),
		MoveTo::create(time, posOfSrc),
		NULL));
	m_destSushi->runAction(Sequence::create(
		MoveTo::create(time, posOfSrc),
		MoveTo::create(time, posOfDest),
		NULL));

}

void PlayLayer::sendMsg()
{
	NotificationCenter::getInstance()->postNotification("score");
}