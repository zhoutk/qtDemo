#include "game.h"

Game::Game():block(nullptr), blockNext(nullptr)
{
	GameSpace space;
}

void Game::start()
{
	curPos = QPoint(3, -1);
	block = new Tetris(curPos, 0);
	blockNext = new Tetris(QPoint(11, 0), 6);
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
	block->moveDown();
}

void Game::moveBlockDownEnd()
{
	block->moveDownEnd();
}
