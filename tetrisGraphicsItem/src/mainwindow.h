#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QDir>
#include "customGraphItem/customgraphtetrisblock.h"
#include "tetrisview.h"
#include "tetris.h"
#include "game.h"
#include "common/Idb/DbBase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	static MainWindow* GetApp();
    ~MainWindow();
	QGraphicsScene* GetScene();
	Game* GetGame();
	void updateScore();

	void initDatabase();

public slots:
	void slotGameOver();
	void slotUpdateScore();
	void on_pushButtonStart_clicked();
	void on_pushButtonPlayback_clicked();

private:
	MainWindow(QWidget* parent = nullptr);
	Ui::MainWindow* ui;
	static MainWindow* APP;
	
	QGraphicsScene* MainScene;

	TetrisView* gameView;
	TetrisView* blockView;

	Game* game;

};
#endif // MAINWINDOW_H
