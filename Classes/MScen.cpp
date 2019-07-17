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

	// 原点的位置
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	// 屏幕的可见区域值
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	
	//俄罗斯方块的主要界面
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
	_tetrisLayer = TetrisLayer::create();
	log("1");
	_tetrisLayer->setPosition(Vec2(10, 10));
	_tetrisLayer->setScoreCallback(std::bind(&MScen::onGetScore, this, std::placeholders::_1));
	_tetrisLayer->setGameOverCallback(std::bind(&MScen::onGameOver, this));
	_tetrisLayer->setcheckCallback(std::bind(&MScen::onCheck, this));
	this->addChild(_tetrisLayer);

	// 文字按钮的添加
	//auto rotateLabel = Label::createWithTTF("rotate", "arial.ttf", 40);//新建一个标签
	//auto rotateMenuItem = MenuItemLabel::create(rotateLabel, std::bind(&HelloWorld::onRotateShape, this));  //通过标签新建一个按钮

	//下面是图片按钮
	auto rotateMenuItem = MenuItemImage::create("rotate.png", "rotate.png", std::bind(&MScen::onRotateShape, this));
	auto downMenuItem = MenuItemImage::create("down.png", "down.png", std::bind(&MScen::onMoveDownShape, this));
	auto leftMenuItem = MenuItemImage::create("left.png", "left.png", std::bind(&MScen::onMoveLeftShape, this));
	auto rightMenuItem = MenuItemImage::create("right.png", "right.png", std::bind(&MScen::onMoveRightShape, this));

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

	//创建词典类实例，将xml文件加载到词典中
	auto *chnStrings = Dictionary::createWithContentsOfFile("CHN_Strings.xml");
	//通过xml文件中的key获取value
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