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
, m_isAnimationing(true)//��־�Ƿ�ʼ���䶯��
, m_isTouchEnable(true)	// ��־֪�������¼�
, m_movingVertical(true)//true: 4������������ը��.  false: 4������������ը��.
, m_iScore(0) //������0
, m_totalTime(0.0) //update���ۼ���Ϸʱ��
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

	//��ȡ�ؼ�����
	m_scoreLab = (Text*)Helper::seekWidgetByName(UI,"scoreLab");
	m_timeBar = (LoadingBar*)Helper::seekWidgetByName(UI,"hpProgress");
}

bool PlayLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
	// ������Ϸ����
	initBG();

	//����UI
	loadUI();

	// ��ʼ����˾�����
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sushi.plist");
    spriteSheet = SpriteBatchNode::create("sushi.pvr.ccz");
    addChild(spriteSheet);
    
	// ��ʼ������Ŀ�͸ߣ�������˾�ĳ��������MATRIX_WIDTH��MATRIX_HEIGHTͨ���궨��
    m_width = MATRIX_WIDTH;
    m_height = MATRIX_HEIGHT;
    
	// ��ʼ����˾�������½ǵĵ�
	Size visibleSize = Director::getInstance()->getVisibleSize();
	m_matrixLeftBottomX = (visibleSize.width - SushiSprite::getContentWidth() * m_width - (m_width - 1) * SUSHI_GAP) / 2;
	m_matrixLeftBottomY = (visibleSize.height - SushiSprite::getContentWidth() * m_height - (m_height - 1) * SUSHI_GAP) / 2;
    
	// ��ʼ������
    int arraySize = sizeof(SushiSprite *) * m_width * m_height;//����ߴ��С
	//Ϊ m_matrix �����ڴ棬���� m_matrix ��ָ��ָ�����͵�ָ�룬�䶨��Ϊ��SushiSprite **m_matrix�����Կ������Ϊ m_matrix ��һ��ָ������
    m_matrix = (SushiSprite **)malloc(arraySize);
    memset((void*)m_matrix, 0, arraySize);//��ʼ������ m_matrix
    
	/*  memset ����

	void *memset(void *s, int ch, size_t n);
	�������ͣ�s ָ���ĳһ���ڴ�,�� s ��ǰ n ���ֽ� ��typedef unsigned int size_t ���� ch ָ����ASCIIֵ �滻������ s ��
	memset����������һ���ڴ�������ĳ��������ֵ�����ǶԽϴ�Ľṹ�������������������һ����췽��[1] ��

	*/
	//��ʼ����˾����
    initMatrix();

	//���ö�ʱ��
    scheduleUpdate();
	this->schedule(schedule_selector(PlayLayer::timeUpdate), 1.0f);

	//�������󶨴�������
	// 1. ����һ���¼���������Onebyone Ϊ���㴥��
	auto touchListener = EventListenerTouchOneByOne::create();
	// 2. �󶨴����¼�
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this); // ������ʼʱ����
	touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this); // �����ƶ�ʱ����
	// 3. ��Ӽ�����
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);// addEventListenerWithSceneGraphPriority(EventListener *listener,Node *node); �¼����������ȼ�Ϊ 0
	//�� _eventDispatcher->addEventListenerWithFixedPriority(EventListenr *listener,int fixedPriority)�����ȼ����������ã�������Ϊ 0 
	/*		
		eventDispatcher�¼��ַ�����
			1. eventDispatcher �� Node �����ԣ�
			2. ͨ�������ǿ���ͳһ����ǰ�ڵ㣨�磺�������㡢����ȣ��������¼��ַ������
			3. ������������һ������ģʽֵ�����ã��� Node ���캯���У�ͨ�� ��Director::getInstance()->getEventDispatcher();�� ��ȡ������������ԣ������ܸ�Ϊ����ĵ��á�
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
			{//getNumberOfRunningActions()�������Ի�ȡsushi����ִ���еĶ�������
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
		m_srcSushi = nullptr;//��Ȼ���ϸ�Դ����bug ����Ū���µĹ���
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
{//������ÿһ��
	
	chainList.push_back(sushi);// �����һ����˾����

	//��ǰ�����ͬͼ��ֵ��ImgIndex������˾
	int neighborCol = sushi->getCol() - 1;//sushiǰһ����˾��������ֵ
	while (neighborCol >= 0) {
		SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
		if (neighborSushi && (neighborSushi->getImgIndex() == sushi->getImgIndex())) {
			chainList.push_back(neighborSushi);//����á��ھӡ���˾
			neighborCol--;//������ǰ
		}
		else {
			break;
		}
	}
	//�������ͬͼ��ֵ��ImgIndex������˾
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

	//�����ӷ�
	setScore();
}

//���õ÷�
void PlayLayer::setScore()
{
	m_iScore += 100;
	m_scoreLab->setText(Value(m_iScore).asString());
}
//����ĳ�ʼ��
void PlayLayer::initMatrix()
{//row ���� ,col ����
    for (int row = 0; row < m_height; row++) {
		for (int col = 0; col < m_width; col++) {
            createAndDropSushi(row, col);
        }
    }
}

//����SushiSprite�������䵽ָ��λ��
void PlayLayer::createAndDropSushi(int row, int col)
{
    Size size = Director::getInstance()->getWinSize();
    
    SushiSprite *sushi = SushiSprite::create(row, col);
    
	// ������ִ�����䶯��
    Point endPosition = positionOfItem(row, col);
    Point startPosition = Point(endPosition.x, endPosition.y + size.height /2);
    sushi->setPosition(startPosition);
	float speed = startPosition.y / (2 * size.height);
    sushi->runAction(MoveTo::create(speed, endPosition));

	//����˾��ӵ�������ע�⣬���û����ӵ�������������ӵ�����Ļ����Ͳ���õ����ܵ��Ż���
    spriteSheet->addChild(sushi);

	//��ָ��λ�õ����鸳ֵ
    m_matrix[row * m_width + col] = sushi;
}

//�õ���Ӧ���о��������ֵ
Point PlayLayer::positionOfItem(int row, int col)
{
    float x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
    float y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
    return Point(x, y);
}
/*//��ȡ���������ڵ���˾,�õ� m_srcSushi
SushiSprite *PlayLayer::sushiOfPoint(Point *point)//���Ը� &point
{
	SushiSprite *sushi = nullptr;
	Rect rect = Rect(0, 0, 0, 0); // rect��������������洢�ɶԳ��ֵĲ��������磬һ�����ο�����Ͻ����ꡢ��Ⱥ͸߶�
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
				//����������, ת��Ϊ��Խڵ�sushi��, �������
				auto point = sushi->convertTouchToNodeSpace(touch);

				//����sushi�ĳߴ����
				Size size = sushi->getContentSize();
				Rect rect = Rect(0, 0, size.width, size.height);

				//�жϴ����Ƿ�����sp�ڲ�
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
//�� m_srcSushi ��Դ��˾����� m_destSushi
void PlayLayer::onTouchMoved(Touch *touch, Event *unused)
{
	//log("onTouchMoved");
	if (!m_srcSushi || !m_isTouchEnable)
		return;
	// ����һЩ������ʡЩ����
	auto row = m_srcSushi->getRow();
	auto col = m_srcSushi->getCol();
	auto location = touch->getLocation();
	auto halfSushiWidth = m_srcSushi->getContentSize().width / 2;
	auto halfSushiHeight = m_srcSushi->getContentSize().height / 2;


	// ����Դ��˾���� rect
	// Դ��˾�ϱ���˾���
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
		m_movingVertical = true;//�������������������˾
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
		m_movingVertical = false;//�������ɺ����������˾
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

	// 1.���� m_srcSushi �� m_destSushi ����˾��������к�, �� m_srcSushi �� m_destSushi ��������
	m_matrix[m_srcSushi->getRow()*m_width + m_srcSushi->getCol()] = m_destSushi;
	m_matrix[m_destSushi->getRow()*m_width + m_destSushi->getCol()] = m_srcSushi;
	int tmpRow = m_srcSushi->getRow();
	int tmpCol = m_srcSushi->getCol();
	m_srcSushi->setRow(m_destSushi->getRow());
	m_srcSushi->setCol(m_destSushi->getCol());
	m_destSushi->setRow(tmpRow);
	m_destSushi->setCol(tmpCol);

	// 2.��⽻����� m_srcSushi �� m_destSushi �ں��ݷ������Ƿ�������������
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