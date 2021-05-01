#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "customGraphItem/customgraphtetrisblock.h"
#include "tetrisview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
	void on_pushButton1_clicked();
	void on_pushButton2_clicked();
	void on_pushButton3_clicked();
	void on_pushButton4_clicked();
	void on_pushButton5_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* MainScene;
	CustomGraphTetrisBlock* block;
	CustomGraphTetrisBlock* blockNext;

    int index;
	TetrisView* gameView;
	TetrisView* blockView;

	QPoint curPos;
};
#endif // MAINWINDOW_H
