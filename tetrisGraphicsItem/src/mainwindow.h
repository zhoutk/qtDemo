#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "customGraphItem/customgraphtetrisblock.h"
#include "tetrisview.h"
#include "tetris.h"
#include "game.h"

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

public slots:
	void slotGameOver();
	void on_pushButton1_clicked();

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
