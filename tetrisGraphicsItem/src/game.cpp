#include "game.h"

extern bool isRunning;

Game::Game():block(nullptr), blockNext(nullptr)
{
	GameSpace space;
}

void Game::start()
{
	isRunning = true;
	curPos = QPoint(3, -1);
	qsrand(QTime::currentTime().msec());	
	block = new Tetris(curPos, qrand() % 7);
	blockNext = new Tetris(QPoint(11, 0), qrand() % 7);
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
	delete block;
	blockNext->relocate(QPoint(3, -1));
	if (blockNext->canContinue()) {
		block = blockNext;
		qsrand(QTime::currentTime().msec());
		blockNext = new Tetris(QPoint(11, 0), qrand() % 7);
	}
	else {
		isRunning = false;
		emit signalGameOver();
	}
	
}
