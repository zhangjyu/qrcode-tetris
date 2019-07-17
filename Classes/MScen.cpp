#include "MScen.h"
#include "TetrisLayer.h"

#include "lose.h"
using namespace cocos2d;

Scene* MScen::createScene() 
{
	auto scene = Scene::create();
	auto Layer = MScen::create();

	scene->addChild(Layer);
	return scene;
}
bool MScen::init() {
	if (!Layer::init()) {
		return false;
	}

	// ԭ���λ��
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	// ��Ļ�Ŀɼ�����ֵ
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	
	//����˹�������Ҫ����
	//����ͼ
	auto sprite_back = Sprite::create("b.png");
	sprite_back->setPosition(visibleSize.width / 2, visibleSize.height / 2);	//����λ��
	sprite_back->setAnchorPoint(Point(0.5, 0.5));	//����ê��
													//������������ʵ�ʴ�С
	Size size_back = sprite_back->getContentSize();	//��ȡ�����С(����ͼƬ��ʱ�򣬾����СΪͼƬ��С)
													//����ʵ�ʿ�Ⱥ��ܿ�ȣ������������ű���
	float scaleX = (float)visibleSize.width / (float)size_back.width;
	float scaleY = (float)visibleSize.height / (float)size_back.height;
	sprite_back->setScale(scaleX, scaleY);	//����

	addChild(sprite_back);
	_tetrisLayer = TetrisLayer::create();
	log("1");
	_tetrisLayer->setPosition(Vec2(10, 10));
	_tetrisLayer->setScoreCallback(std::bind(&MScen::onGetScore, this, std::placeholders::_1));
	_tetrisLayer->setGameOverCallback(std::bind(&MScen::onGameOver, this));
	_tetrisLayer->setcheckCallback(std::bind(&MScen::onCheck, this));
	this->addChild(_tetrisLayer);

	// ���ְ�ť�����
	//auto rotateLabel = Label::createWithTTF("rotate", "arial.ttf", 40);//�½�һ����ǩ
	//auto rotateMenuItem = MenuItemLabel::create(rotateLabel, std::bind(&HelloWorld::onRotateShape, this));  //ͨ����ǩ�½�һ����ť

	//������ͼƬ��ť
	auto rotateMenuItem = MenuItemImage::create("rotate.png", "rotate.png", std::bind(&MScen::onRotateShape, this));
	auto downMenuItem = MenuItemImage::create("down.png", "down.png", std::bind(&MScen::onMoveDownShape, this));
	auto leftMenuItem = MenuItemImage::create("left.png", "left.png", std::bind(&MScen::onMoveLeftShape, this));
	auto rightMenuItem = MenuItemImage::create("right.png", "right.png", std::bind(&MScen::onMoveRightShape, this));

	//���ð�ť��λ�ã������λ�ã����滹�в˵���λ��
	rotateMenuItem->setPosition(Vec2(0, 70));
	downMenuItem->setPosition(Vec2(0, -70));
	leftMenuItem->setPosition(Vec2(-90, 0));
	rightMenuItem->setPosition(Vec2(90, 0));

	auto menu = Menu::create(rotateMenuItem, downMenuItem, leftMenuItem, rightMenuItem, nullptr); //һ���Ե���Ӷ���˵���ť��һ���˵���
	menu->setPosition(Vec2(origin.x + visibleSize.width - 150, origin.y + 200));  //���ò˵���λ��
	this->addChild(menu); //���˰����뵽������

						  //��ʾ�����ı�ǩ
						  //�����ʵ���ʵ������xml�ļ����ص��ʵ���
	auto *chnStrings = Dictionary::createWithContentsOfFile("CHN_Strings.xml");
	//ͨ��xml�ļ��е�key��ȡvalue
	const char *strScore = ((String*)chnStrings->objectForKey("Score"))->getCString();
	_scoreLabel = Label::createWithTTF(StringUtils::format("%s%d", strScore, 0), "AdobeHeitiStd-Regular.otf", 40);
	_scoreLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 130, origin.y + visibleSize.height - 50));
	_scoreLabel->setAnchorPoint(Vec2(0, 0.5));
	this->addChild(_scoreLabel);
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MScen::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MScen::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	return true;
}


void MScen::onRotateShape()
{
	_tetrisLayer->rotateShape();
}

void MScen::onMoveLeftShape()
{
	_tetrisLayer->moveShapeLeft();
}

void MScen::onMoveRightShape()
{
	_tetrisLayer->moveShapeRight();
}

void MScen::onMoveDownShape()
{
	_tetrisLayer->moveShapeDown();
}

void MScen::onQuit(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void MScen::onGetScore(std::string score)
{
	if (score == "") return;
	log("onGetScore %s", score.c_str());
	_scoreLabel->setString(score);
}

void MScen::onGameOver()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	//�����ʵ���ʵ������xml�ļ����ص��ʵ���
	auto *chnStrings = Dictionary::createWithContentsOfFile("CHN_Strings.xml");
	//ͨ��xml�ļ��е�key��ȡvalue
	const char *strGameOver = ((String*)chnStrings->objectForKey("Score"))->getCString();
	auto label = Label::createWithTTF(strGameOver, "AdobeHeitiStd-Regular.otf", 60);
	label->setColor(Color3B(255, 0, 0));
	label->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	addChild(label);
	removeChild(_tetrisLayer);
	_callendFunc();
}
void MScen::onCheck() {
	del = _tetrisLayer->del;
	_callcheckFunc();
}
void MScen::upline(int line) {
	_tetrisLayer->upline(line);
}

bool MScen::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
	Point touchPO = touch->getLocation();

	firstX = touchPO.x;
	firstY = touchPO.y;

	return true;
}
void MScen::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
	Point touchPO = touch->getLocation();
	endX = firstX - touchPO.x;
	endY = firstY - touchPO.y;

	if (abs(endX)>abs(endY)) {
		//����
		if (endX + 5>0) {
			onMoveLeftShape();
		}
		else {
			onMoveRightShape();
		}
	}
	else {
		//����
		if (endY - 5>0) {
			onMoveDownShape();

		}
		else {
			onRotateShape();
		}
	}
}