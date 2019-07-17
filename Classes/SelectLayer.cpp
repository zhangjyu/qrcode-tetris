#include "SelectLayer.h"
#include "ServerLayer.h"
#include "ClientLayer.h"
#include "TetrisLayer.h"
Scene* SelectLayer::createScene()
{
	auto scene = Scene::createWithPhysics();	
	auto layer = SelectLayer::create();

	scene->addChild(layer);
	return scene;
}

bool SelectLayer::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	this->initMenu();

	return true;
}

void SelectLayer::initMenu()
{
	auto winSize = Director::getInstance()->getVisibleSize();
	auto sprite_back = Sprite::create("vs.png");
	sprite_back->setPosition(winSize.width / 2, winSize.height / 2);	//设置位置
	sprite_back->setAnchorPoint(Point(0.5, 0.5));	//设置锚点
													//根据缩放设置实际大小
	Size size_back = sprite_back->getContentSize();	//获取精灵大小(设置图片的时候，精灵大小为图片大小)
													//根据实际宽度和总宽度，计算期望缩放比率
	float scaleX = (float)winSize.width / (float)size_back.width;
	float scaleY = (float)winSize.height / (float)size_back.height;
	sprite_back->setScale(scaleX, scaleY);	//缩放

	addChild(sprite_back);
//	auto winSize = Director::getInstance()->getVisibleSize();
	auto lblServer = Label::createWithSystemFont("create room", "Arial", 36);
	auto menuServer = MenuItemLabel::create(lblServer, CC_CALLBACK_1(SelectLayer::menuServerClick, this));
	menuServer->setPosition(Vec2(winSize.width * 0.5f, winSize.height * 0.6f));

	auto lblClient = Label::createWithSystemFont("join room", "Arial", 36);
	auto menuClient = MenuItemLabel::create(lblClient, CC_CALLBACK_1(SelectLayer::menuClientClick, this));
	menuClient->setPosition(Vec2(winSize.width * 0.5f, winSize.height * 0.5f));

	auto menu = Menu::create(menuServer, menuClient, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);

}

void SelectLayer::menuClientClick(Ref* sender)
{
	auto scene=cl->createScene();
	//auto scene = ClientLayer::createScene();
	
	Director::getInstance()->replaceScene(scene);
}

void SelectLayer::menuServerClick(Ref* sender)
{
	auto scene=sl->createScene();
	//sl->setlogCallback(std::bind(&SelectLayer::onlog, this, std::placeholders::_1));
	//auto scene = ServerLayer::createScene();
	
	Director::getInstance()->replaceScene(scene);
}
/*
char *SelectLayer::FontToUTF8(const char* font) {
	int len = MultiByteToWideChar(CP_ACP, 0, font, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, font, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char *str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr)delete[] wstr;
	return str;
}*/
