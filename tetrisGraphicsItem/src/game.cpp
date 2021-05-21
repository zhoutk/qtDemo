#include "game.h"
#include "common/Idb/DbBase.h"

extern int isRunning;
extern DbBase* db;
extern SnowFlake snowFlakeId;
QVector<int> SCORES = { 0, 1, 3, 7, 10 };

Game::Game():block(nullptr), blockNext(nullptr), autoTick(new QTimer(this)),
		tickVal(1000), tickInterval(100),speed(1),levels(0),scores(0), upScore(50),
		stepNum(0), gameID(QString())
{
	GameSpace space;
	connect(autoTick, &QTimer::timeout, this, &Game::slotAutoTick);
}

Game::~Game()
{
	if (autoTick->isActive())
		autoTick->stop();
	delete autoTick;
}

void Game::start()
{
	records.clear();
	gameID = snowFlakeId.nextid();
	stepNum = 0;
	isRunning = 1;
	curPos = QPoint(3, -1);
	speed = 1;
	levels = 0;
	scores = 0;
	qsrand(QTime::currentTime().msec());	
	block = new Tetris(curPos, qrand() % 7);
	blockNext = new Tetris(QPoint(11, 0), qrand() % 7);
	int rotateInit = qrand() % 4;
	for (int i = 0; i < rotateInit; i++)
		blockNext->rotate();
	autoTick->setInterval(tickVal);
	autoTick->start();
}

void Game::pause()
{
	isRunning = 5;
	autoTick->stop();
}

void Game::resume()
{
	isRunning = 1;
	autoTick->start();
}

void Game::rotateBlock()
{
	block->rotate();
}

void Game::moveBlockLeft()
{
	block->moveLeft();
}

void Game::moveBlockRight()
{
	block->moveRight();
}

void Game::moveBlockDown()
{
	if (!block->moveDown()) {
		generateNextBlock();
	}
}

void Game::moveBlockDownEnd()
{
	block->moveDownEnd();
	generateNextBlock();
}

void Game::generateNextBlock()
{
	if (isRunning == 1) {
		QPoint blockPos = block->getPos();
		Qjson params;
		params.AddValueBase("id", snowFlakeId.nextid());
		params.AddValueBase("gameId", gameID);
		params.AddValueBase("blockType", block->getBlockType());
		params.AddValueBase("rotateNumber", block->getRotateNum());
		params.AddValueBase("LocateX", blockPos.x());
		params.AddValueBase("LocateY", blockPos.y());
		params.AddValueBase("stepId", ++stepNum);
		//db->create("gameRecords", params);
		records.push_back(params);
	}
	int levelCount = block->cleanRow();
	levels += levelCount;
	scores += SCORES[levelCount];
	if (levelCount > 0) {
		if (scores / upScore >= speed) {
			int val = tickVal - tickInterval * speed;
			if(isRunning == 1)
				autoTick->setInterval( val > 50 ? val : 50);
			speed++;
		}
		emit signalUpdateScore();
	}
	delete block;
	blockNext->relocate(QPoint(3, -1));
	if (blockNext->canContinue()) {
		block = blockNext;
		int rotateInit = 0;
		if (isRunning == 1) {
			qsrand(QTime::currentTime().msec());
			blockNext = new Tetris(QPoint(11, 0), qrand() % 7);
			rotateInit = qrand() % 4;
		}
		else {
			Qjson cur = records[playbackIndex-1];
			Qjson next = records[playbackIndex++];
			curPos = QPoint(cur["LocateX"].toDouble(), -1);
			block->relocate(curPos);
			curPos.setY(cur["LocateY"].toDouble());
			blockNext = new Tetris(QPoint(11, 0), next["blockType"].toDouble());
			rotateInit = next["rotateNumber"].toDouble();
		}
		for (int i = 0; i < rotateInit; i++)
			blockNext->rotate();
	}
	else {
		autoTick->stop();
		emit signalGameOver();
		if (isRunning == 1) {
			QPoint blockPos = blockNext->getPos();
			Qjson params;
			params.AddValueBase("id", snowFlakeId.nextid());
			params.AddValueBase("gameId", gameID);
			params.AddValueBase("blockType", blockNext->getBlockType());
			params.AddValueBase("rotateNumber", blockNext->getRotateNum());
			params.AddValueBase("LocateX", blockPos.x());
			params.AddValueBase("LocateY", blockPos.y());
			params.AddValueBase("stepId", ++stepNum);
			//db->create("gameRecords", params);
			records.push_back(params);
			db->insertBatch("gameRecords", records);

			Qjson ps;
			ps.AddValueBase("id", gameID);
			ps.AddValueBase("speed", speed);
			ps.AddValueBase("levels", levels);
			ps.AddValueBase("scores", scores);
			ps.AddValueBase("steps", stepNum);
			db->create("gameLists", ps);
		}
		isRunning = 0;
	}
	
}

int Game::getSpeed()
{
	return speed;
}

int Game::getLevels()
{
	return levels;
}

int Game::getScores()
{
	return scores;
}

QString Game::getGameID()
{
	return gameID;
}

void Game::playback(QVector<Qjson> arr)
{
	speed = 1;
	levels = 0;
	scores = 0;
	curPos = QPoint(3, -1);

	records.clear();
	records = arr;
	isRunning = 2;
	playbackIndex = 2;
	int len = arr.size();
	if (len > 2) {
		Qjson num1 = arr[0];
		curPos.setX(num1["LocateX"].toDouble());
		block = new Tetris(curPos, num1["blockType"].toDouble());
		curPos.setY(num1["LocateY"].toDouble());
		int rotateCt = num1["rotateNumber"].toDouble();
		for (int k = 0; k < rotateCt; k++)
			block->rotate();

		Qjson num2 = arr[1];
		blockNext = new Tetris(QPoint(11, 0), num2["blockType"].toDouble());
		rotateCt = num2["rotateNumber"].toDouble();
		for (int k = 0; k < rotateCt; k++)
			blockNext->rotate();
	}
	autoTick->setInterval(100);
	autoTick->start();
}

void Game::slotAutoTick()
{
	if (isRunning == 1) {
		this->moveBlockDown();
		if (records.size() > 30) {
			db->insertBatch("gameRecords", records);
			records.clear();
		}
	}
	else if (isRunning == 2) {
		block->relocate(curPos);
		block->setBlockNotActive();
		generateNextBlock();
	}
}
