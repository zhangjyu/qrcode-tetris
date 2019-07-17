#ifndef __SELECT_LAYER_H__
#define __SELECT_LAYER_H__

#include "cocos2d.h"

#include "ClientLayer.h"
#include "ServerLayer.h"

USING_NS_CC;

class SelectLayer : public Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(SelectLayer);
	void menuServerClick(Ref* sender); 
	void menuClientClick(Ref* sender);
	//char *FontToUTF8(const char* font);
	void onlog(std::string str);
private:
	void initMenu();
	ServerLayer *sl;
	ClientLayer *cl;
	//SocketServer *ss;
};

#endif 
