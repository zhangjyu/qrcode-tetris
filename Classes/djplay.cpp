#include <random>
#include "djplay.h"
#include "Block.h"
#include "HelloWorldScene.h"

#include "lose.h"
using namespace cocos2d;

const int MAX_ROW = 20;
const int MAX_COL = 12;
const float BLOCK_SIZE = 40.0;
const Vec2 NEXT_POS = Vec2(560, 700);  // next shape position
const Vec2 CUR_START_POS = Vec2(6, 18); // current shape start pos
const float START_SPEED = 1.0f;  // 2.0 second

bool Djplay::init()
{
	if (!Node::init()) return false;
	log("play!!!!!!!!!!!!!!!");
	_nRowNums = MAX_ROW;
	_nColNums = MAX_COL;
	_fBlockWid = BLOCK_SIZE;
	_curDownSpeed = START_SPEED;
	_score = 0;

	initBg();

	initBlock();

	return true;
}

Vec2 Djplay::getVec2Pos(int row, int col)
{
	return Vec2(_fBlockWid*(row + 0.5), _fBlockWid*(col + 0.5));
}

void Djplay::initBg()
{
	auto drawNode = DrawNode::create();
	drawNode->setLineWidth(1);
	this->addChild(drawNode);

	auto colorLine = Color4F(1.0, 1.0, 1.0, 0.4);

	for (int i = 0; i < _nColNums + 1; ++i)
	{
		drawNode->drawLine(Vec2(_fBlockWid*i, 0), Vec2(_fBlockWid*i, _fBlockWid*_nRowNums), colorLine);
	}
	for (int i = 0; i < _nRowNums + 1; ++i)
	{
		drawNode->drawLine(Vec2(0, _fBlockWid*i), Vec2(_fBlockWid*_nColNums, _fBlockWid*i), colorLine);
	}
}

void Djplay::initBlock()
{
	createNextShape();
	scheduleOnce((SEL_SCHEDULE)&Djplay::getNextShape, 0.5);
}

void Djplay::createNextShape()
{
	std::random_device rd;
	int randNumber = rd() % Shape::getShapeNumber();

	Shape *shape;
	switch (randNumber) {
	case 0:
		shape = IShape::create();
		break;
	case 1:
		shape = JShape::create();
		break;
	case 2:
		shape = LShape::create();
		break;
	case 3:
		shape = OShape::create();
		break;
	case 4:
		shape = SShape::create();
		break;
	case 5:
		shape = ZShape::create();
		break;
	case 6:
		shape = TShape::create();
		break;
	default:
		break;
	}

	Block::BlockColor color;
	randNumber = rd() % Block::getColorNumber();
	switch (randNumber) {
	case 0:
		color = Block::BlockColor::kBlue;
		break;
	case 1:
		color = Block::BlockColor::kOrange;
		break;
	case 2:
		color = Block::BlockColor::kPurple;
		break;
	case 3:
		color = Block::BlockColor::kRed;
		break;
	case 4:
		color = Block::BlockColor::kTeal;
		break;
	case 5:
		color = Block::BlockColor::kYellow;
		break;
	default:
		break;
	}

	shape->setOrientation(Shape::Orientation::kUp);
	shape->setBlockColor(color);
	shape->initBlocks();
	addChild(shape);
	shape->setPosition(NEXT_POS);

	_nextShape = shape;
}

void Djplay::putNextToCurrent()
{
	_curShape = _nextShape;

	_curShape->setPosition(getPositionByRowCol(CUR_START_POS.y, CUR_START_POS.x));
	_curShape->setRowInTetris(CUR_START_POS.y);
	_curShape->setColInTetris(CUR_START_POS.x);

	createNextShape();

	// check whether game over
	if (checkCover())
	{
		//_callGameOverFunc();
		auto scene = Lose::createScene();
		Director::getInstance()->replaceScene(scene);
		//Layer::onExit();
		log("sdfdaadsfsdafsadfsda");
	}

	// more diffcult
	if (_score > 100 && _score<500) _curDownSpeed = 0.8f;
	else if (_score >= 500 && _score < 1000) _curDownSpeed = 0.6f;
	else if (_score >= 1000 && _score < 2000) _curDownSpeed = 0.4f;
	else if (_score >= 2000) _curDownSpeed = 0.2f;

	schedule(CC_SCHEDULE_SELECTOR(Djplay::dropDown), _curDownSpeed);
}

Vec2 Djplay::getPositionByRowCol(int row, int col)
{
	return Vec2(20 + 40 * col, 20 + 40 * row);
}

void Djplay::dropDown(float dt)
{
	auto tempShapeRow = _curShape->getRowInTetris() - 1;

	if (checkCoverByPos(tempShapeRow, _curShape->getColInTetris()))
	{
		unschedule(CC_SCHEDULE_SELECTOR(Djplay::dropDown));

		removeCurShape();

		checkAndRemoveRows();

		scheduleOnce((SEL_SCHEDULE)&Djplay::getNextShape, 0.5);
	}
	else
		setCurShapePosition(tempShapeRow, _curShape->getColInTetris());
}

void Djplay::getNextShape(float dt)
{
	putNextToCurrent();
}

bool Djplay::checkCover()
{
	if (_curShape == nullptr) return true;

	// chect current shape each block
	auto rowCurShape = _curShape->getRowInTetris();
	auto colCurShape = _curShape->getColInTetris();
	return checkCoverByPos(rowCurShape, colCurShape);
}

bool Djplay::checkCoverByPos(int row, int col)
{
	if (_curShape == nullptr) return true;

	// chect current shape each block
	auto rowColBlocks = _curShape->getBlockPos();

	for (int i = 0; i < rowColBlocks.size(); i++)
	{
		auto tempRow = row + rowColBlocks[i].y;
		auto tempCol = col + rowColBlocks[i].x;

		if (tempRow < 0 || tempCol < 0 || tempCol > _nColNums - 1)
			return true;

		auto Id = getRowColumnID(tempRow, tempCol);
		if (hasBlockById(Id))
			return true;
	}

	return false;
}

void Djplay::removeCurShape()
{
	auto shapeBlocks = _curShape->getBlocks();
	auto shapeRow = _curShape->getRowInTetris();
	auto shapeCol = _curShape->getColInTetris();
	auto shapePosition = _curShape->getPosition();
	auto shapeBlockPos = _curShape->getBlockPos();

	int idx = 0;
	for (auto &&item : shapeBlocks)
	{
		auto block = Block::create();
		block->setBlockColor(item->getBlockColor());
		block->setPosition(item->getPosition() + shapePosition);
		addChild(block);

		auto col = shapeBlockPos[idx].x + shapeCol;
		auto row = shapeBlockPos[idx].y + shapeRow;
		block->setRow(row);
		block->setCol(col);
		auto id = getRowColumnID(row, col);
		block->setTag(id);

		idx++;
	}

	_curShape->removeFromParent();
	_curShape = nullptr;
}

bool Djplay::hasBlockById(int id)
{
	return (getChildByTag(id) != nullptr);
}

void Djplay::checkAndRemoveRows()
{
	int deleteRowNumber = 0;
	for (int row = 0; row < _nRowNums; row++)
	{
		int col = 0;
		while (col < _nColNums)
		{
			auto id = getRowColumnID(row, col);
			if (!hasBlockById(id))
				break;
			col++;
		}
		if (col == _nColNums)  // need delete
		{
			//_callcheckFunc();
			auto particleSystem = ParticleSystemQuad::create("fire.plist");
			auto pos = getPositionByRowCol(row, _nRowNums / 2);
			particleSystem->setPosition(pos);
			this->addChild(particleSystem);
		}
	}
	for (int row = 0; row < _nRowNums;)
	{
		int col = 0;
		while (col < _nColNums)
		{
			auto id = getRowColumnID(row, col);
			if (!hasBlockById(id))
				break;
			col++;
		}
		if (col == _nColNums)  // need delete
		{
			deleteRowNumber++;
			for (int col = 0; col < _nColNums; ++col)
			{
				auto id = getRowColumnID(row, col);
				// log("delete %d %d", row, col);
				removeChildByTag(id);
			}
			for (int moveRow = row + 1; moveRow < _nRowNums; ++moveRow)
				for (int moveCol = 0; moveCol < _nColNums; ++moveCol)
				{
					//log("move %d %d", moveRow, moveCol);
					auto id = getRowColumnID(moveRow, moveCol);
					auto block = dynamic_cast<Block*>(getChildByTag(id));
					if (block)
					{
						auto newId = getRowColumnID(moveRow - 1, moveCol);
						block->setTag(newId);
						block->setRow(moveRow - 1);
						auto pos = getPositionByRowCol(moveRow - 1, moveCol);
						block->setPosition(pos);
					}
				}
		}
		else
		{
			row++;
		}
	}
	addScoreByCount(deleteRowNumber);
}

bool Djplay::rotateShape()
{
	if (_curShape == nullptr) return false;

	// next orientation
	auto tempOrient = _curShape->getNextOrientation();
	auto tempBlockPos = _curShape->getBlockPosByOrientation(tempOrient);
	auto row = _curShape->getRowInTetris();
	auto col = _curShape->getColInTetris();

	// check cover about next orientation
	for (int i = 0; i < tempBlockPos.size(); i++)
	{
		auto tempRow = row + tempBlockPos[i].y;
		auto tempCol = col + tempBlockPos[i].x;

		if (tempRow < 0 || tempCol < 0 || tempCol > _nColNums - 1)
			return false;

		auto Id = getRowColumnID(tempRow, tempCol);
		if (hasBlockById(Id))
			return false;
	}

	// rotate the shape
	_curShape->rotate();

	return true;
}

bool Djplay::moveShapeLeft()
{
	if (_curShape == nullptr) return false;

	auto tempShapeCol = _curShape->getColInTetris() - 1;

	if (checkCoverByPos(_curShape->getRowInTetris(), tempShapeCol))
		return false;
	else
		setCurShapePosition(_curShape->getRowInTetris(), tempShapeCol);
	return true;
}

bool Djplay::moveShapeRight()
{
	if (_curShape == nullptr) return false;

	auto tempShapeCol = _curShape->getColInTetris() + 1;

	if (checkCoverByPos(_curShape->getRowInTetris(), tempShapeCol))
		return false;
	else
		setCurShapePosition(_curShape->getRowInTetris(), tempShapeCol);

	return true;
}

void Djplay::moveShapeDown()
{
	if (_curShape == nullptr) return;

	auto curShapeRow = _curShape->getRowInTetris();
	auto curShapeCol = _curShape->getColInTetris();
	auto rowColBlocks = _curShape->getBlockPos();

	auto minDiff = 1000;

	for (auto &&item : rowColBlocks)
	{
		auto tempRow = item.y + curShapeRow;
		auto tempCol = item.x + curShapeCol;

		auto tempMaxRowCanPut = getMaxRowInBlocks(tempRow, tempCol) + 1;
		if (minDiff > tempRow - tempMaxRowCanPut)
			minDiff = tempRow - tempMaxRowCanPut;
	}

	setCurShapePosition(curShapeRow - minDiff, curShapeCol);
}

void Djplay::setCurShapePosition(int row, int col)
{
	if (_curShape == nullptr) return;

	_curShape->setRowInTetris(row);
	_curShape->setColInTetris(col);
	auto pos = getPositionByRowCol(row, col);
	_curShape->setPosition(pos);
}

int Djplay::getMaxRowInBlocks(int row, int col)
{
	int tempRow = row;
	while (!hasBlockById(getRowColumnID(tempRow, col)))
	{
		tempRow--;
		if (tempRow < 0)
			break;
	}
	return tempRow;
}

void Djplay::addScoreByCount(int count)
{
	auto *chnStrings = Dictionary::createWithContentsOfFile("CHN_Strings.xml");
	//通过xml文件中的key获取value
	const char *strScore = ((String*)chnStrings->objectForKey("Score"))->getCString();

	if (count <= 0) return;
	int getScore = count * 50;
	_score += getScore;

	if (_callScoreFunc)
		_callScoreFunc(StringUtils::format("%s%d", strScore, _score));
}
void Djplay::upline() {
	log("%d", _nRowNums);
	for (int moveRow = _nRowNums; moveRow >= 0; --moveRow) {
		log("%d", moveRow);
		for (int moveCol = 0; moveCol < _nColNums; ++moveCol) {
			auto id = getRowColumnID(moveRow, moveCol);
			auto block = dynamic_cast<Block*>(getChildByTag(id));
			if (block) {
				auto newId = getRowColumnID(moveRow + 1, moveCol);
				block->setTag(newId);
				block->setRow(moveRow + 1);
				auto pos = getPositionByRowCol(moveRow + 1, moveCol);
				block->setPosition(pos);
			}/*
			 for (int col = 0; col < _nColNums; ++col) {
			 auto id = getRowColumnID(moveRow, col);
			 removeChildByTag(id);
			 }*/
		}
	}
}