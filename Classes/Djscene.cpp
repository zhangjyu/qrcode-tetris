#include "Djscene.h"
#include "djplay.h"

#include "lose.h"
using namespace cocos2d;

Scene* Djscene::createScene()
{
	auto scene = Scene::create();
	auto Layer = Djscene::create();

	scene->addChild(Layer);
	return scene;
}
bool Djscene::init() {
	if (!Layer::init()) {
		return false;
	}
	log("safdojdsaigjisdofjisdo");
	// 原点的位置
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	// 屏幕的可见区域值
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//退出游戏按钮
	auto closeItem = MenuItemImage::create("CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(Djscene::onQuit, this));
	//精灵菜单类是MenuItemSprite，图片菜单类是MenuItemImage。由于MenuItemImage继承于MenuItemSprite，所以图片菜单也属于精灵菜单。
	//每个节点都有一个定位(锚)点，也就是贴图的中心点对应整个精灵的位置。默认情况下， anchorPoint 属性设置为（0.5,0.5）
	//另两个相关函数sprite.setScale(0.6f)设置缩放，sprite.setPosition设置精灵的左下角坐标
	closeItem->setAnchorPoint(Vec2(1, 1)); //仿佛是1,1在坐下，0,0在右上

										   // create menu, it's an autorelease object
	auto mainMenu = Menu::create(closeItem, NULL);
	mainMenu->setAnchorPoint(Vec2(0, 0));
	mainMenu->setPosition(Vec2(origin.x + visibleSize.width - 20, origin.y + visibleSize.height - 20));
	this->addChild(mainMenu, 10);
	//背景图
	auto sprite_back = Sprite::create("b.png");
	sprite_back->setPosition(visibleSize.width / 2, visibleSize.height / 2);	//设置位置
	sprite_back->setAnchorPoint(Point(0.5, 0.5));	//设置锚点
													//根据缩放设置实际大小
	Size size_back = sprite_back->getContentSize();	//获取精灵大小(设置图片的时候，精灵大小为图片大小)
													//根据实际宽度和总宽度，计算期望缩放比率
	float scaleX = (float)visibleSize.width / (float)size_back.width;
	float scaleY = (float)visibleSize.height / (float)size_back.height;
	sprite_back->setScale(scaleX, scaleY);	//缩放

	addChild(sprite_back);

	djplay = Djplay::create();
	log("1");
	djplay->setPosition(Vec2(10, 10));
	djplay->setScoreCallback(std::bind(&Djscene::onGetScore, this, std::placeholders::_1));
	this->addChild(djplay);
	//下面是图片按钮
	auto rotateMenuItem = MenuItemImage::create("rotate.png", "rotate.png", std::bind(&Djscene::onRotateShape, this));
	auto downMenuItem = MenuItemImage::create("down.png", "down.png", std::bind(&Djscene::onMoveDownShape, this));
	auto leftMenuItem = MenuItemImage::create("left.png", "left.png", std::bind(&Djscene::onMoveLeftShape, this));
	auto rightMenuItem = MenuItemImage::create("right.png", "right.png", std::bind(&Djscene::onMoveRightShape, this));
	//设置按钮的位置，是相对位置，后面还有菜单的位置
	rotateMenuItem->setPosition(Vec2(0, 70));
	downMenuItem->setPosition(Vec2(0, -70));
	leftMenuItem->setPosition(Vec2(-90, 0));
	rightMenuItem->setPosition(Vec2(90, 0));
	auto menu = Menu::create(rotateMenuItem, downMenuItem, leftMenuItem, rightMenuItem, nullptr); //一次性的添加多个菜单按钮到一个菜单中
	menu->setPosition(Vec2(origin.x + visibleSize.width - 150, origin.y + 200));  //设置菜单的位置
	this->addChild(menu); //将菜案加入到本层中

						  //显示分数的标签
						  //创建词典类实例，将xml文件加载到词典中
	auto *chnStrings = Dictionary::createWithContentsOfFile("CHN_Strings.xml");
	//通过xml文件中的key获取value
	const char *strScore = ((String*)chnStrings->objectForKey("Score"))->getCString();
	_scoreLabel = Label::createWithTTF(StringUtils::format("%s%d", strScore, 0), "AdobeHeitiStd-Regular.otf", 40);
	_scoreLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 - 130, origin.y + visibleSize.height - 50));
	_scoreLabel->setAnchorPoint(Vec2(0, 0.5));
	this->addChild(_scoreLabel);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Djscene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Djscene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}


void Djscene::onRotateShape()
{
	djplay->rotateShape();
}

void Djscene::onMoveLeftShape()
{
	djplay->moveShapeLeft();
}

void Djscene::onMoveRightShape()
{
	djplay->moveShapeRight();
}

void Djscene::onMoveDownShape()
{
	djplay->moveShapeDown();
}

void Djscene::onQuit(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void Djscene::onGetScore(std::string score)
{
	if (score == "") return;
	log("onGetScore %s", score.c_str());
	_scoreLabel->setString(score);
}

void Djscene::onGameOver()
{
	//_callendFunc();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	//创建词典类实例，将xml文件加载到词典中
	auto *chnStrings = Dictionary::createWithContentsOfFile("CHN_Strings.xml");
	//通过xml文件中的key获取value
	const char *strGameOver = ((String*)chnStrings->objectForKey("Score"))->getCString();
	auto label = Label::createWithTTF(strGameOver, "AdobeHeitiStd-Regular.otf", 60);
	label->setColor(Color3B(255, 0, 0));
	label->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	addChild(label);
	auto lo = Lose::create();
	this->addChild(lo);
}


bool Djscene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
	Point touchPO = touch->getLocation();

	firstX = touchPO.x;
	firstY = touchPO.y;

	return true;
}
void Djscene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
	Point touchPO = touch->getLocation();
	endX = firstX - touchPO.x;
	endY = firstY - touchPO.y;

	if (abs(endX)>abs(endY)) {
		//左右
		if (endX + 5>0) {
			onMoveLeftShape();
		}
		else {
			onMoveRightShape();
		}
	}
	else {
		//上下
		if (endY - 5>0) {
			onMoveDownShape();

		}
		else {
			onRotateShape();
		}
	}
}