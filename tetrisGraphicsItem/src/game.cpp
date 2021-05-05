#include "game.h"

extern bool isRunning;
QVector<int> SCORES = { 0, 1, 3, 7, 10 };

Game::Game():block(nullptr), blockNext(nullptr), autoTick(new QTimer(this)),
		tickVal(1000), tickInterval(100),speed(1),levels(0),scores(0), upScore(50)
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
	isRunning = true;
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
	int levelCount = block->cleanRow();
	levels += levelCount;
	scores += SCORES[levelCount];
	if (levelCount > 0) {
		if (scores / upScore >= speed) {
			int val = tickVal - tickInterval * speed;
			autoTick->setInterval( val > 50 ? val : 50);
			speed++;
		}
		emit signalUpdateScore();
	}
	delete block;
	blockNext->relocate(QPoint(3, -1));
	if (blockNext->canContinue()) {
		block = blockNext;
		qsrand(QTime::currentTime().msec());
		blockNext = new Tetris(QPoint(11, 0), qrand() % 7);
		int rotateInit = qrand() % 4;
		for (int i = 0; i < rotateInit; i++)
			blockNext->rotate();
	}
	else {
		isRunning = false;
		autoTick->stop();
		emit signalGameOver();
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

void Game::slotAutoTick()
{
	if (isRunning) {
		this->moveBlockDown();
	}
}
