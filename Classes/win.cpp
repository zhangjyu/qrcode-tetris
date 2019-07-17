#include "win.h"
#include "HelloWorldScene.h"
Scene* Win::createScene() {
	auto scene = Scene::create();
	auto layer = Win::create();
	scene->addChild(layer);
	return scene;
}
bool Win::init() {
	if(!Layer::init()){
		return false;
	}
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto winSize = Director::getInstance()->getVisibleSize();
	auto layer1 = LayerColor::create(Color4B(128, 0, 128, 255));
	layer1->setContentSize(CCSizeMake(visibleSize.width, visibleSize.height));
	addChild(layer1);

	//背景图
	auto sprite_back = Sprite::create("win.png");
	sprite_back->setPosition(visibleSize.width / 2, visibleSize.height / 2);	//设置位置
	sprite_back->setAnchorPoint(Point(0.5, 0.5));	//设置锚点
													//根据缩放设置实际大小
	Size size_back = sprite_back->getContentSize();	//获取精灵大小(设置图片的时候，精灵大小为图片大小)
													//根据实际宽度和总宽度，计算期望缩放比率
	float scaleX = (float)visibleSize.width / (float)size_back.width;
	float scaleY = (float)visibleSize.height / (float)size_back.height;
	sprite_back->setScale(scaleX, scaleY);	//缩放

	addChild(sprite_back);
	/*
	MenuItemFont::setFontName("Marker Felt");//设置字体
	MenuItemFont *label = MenuItemFont::create("Shoot", this, menu_selector(Win::replay));//添加文字按钮  
	label->setScale(1.7);
	label->setColor(ccc3(255, 0, 0));//设置颜色  

	Menu *newmenu = Menu::create(label, NULL); // 此步很关键，否则只是菜单项，但不能点击  
	newmenu->setPosition(Vec2(visibleSize.width - 150,visibleSize.height - 80));
	this->addChild(newmenu, 2);*/
	//退出游戏按钮
	auto closeItem = MenuItemImage::create("CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(Win::onQuit, this));
	//精灵菜单类是MenuItemSprite，图片菜单类是MenuItemImage。由于MenuItemImage继承于MenuItemSprite，所以图片菜单也属于精灵菜单。
	//每个节点都有一个定位(锚)点，也就是贴图的中心点对应整个精灵的位置。默认情况下， anchorPoint 属性设置为（0.5,0.5）
	//另两个相关函数sprite.setScale(0.6f)设置缩放，sprite.setPosition设置精灵的左下角坐标
	closeItem->setAnchorPoint(Vec2(1, 1)); //仿佛是1,1在坐下，0,0在右上

										   // create menu, it's an autorelease object
	auto mainMenu = Menu::create(closeItem, NULL);
	mainMenu->setAnchorPoint(Vec2(0, 0));
	mainMenu->setPosition(Vec2(origin.x + visibleSize.width - 20, origin.y + visibleSize.height - 20));
	this->addChild(mainMenu, 10);
	return true;
}
void Win::replay(Ref* sender) {
	auto scene = HelloWorld::createScene();
	Director::getInstance()->replaceScene(scene);
}
void Win::onQuit(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}