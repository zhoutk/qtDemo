#ifndef TETRISVIEW_H
#define TETRISVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QKeyEvent>
#include "game.h"

class TetrisView : public QGraphicsView
{
    Q_OBJECT
public:
    TetrisView(QWidget* parent = nullptr);

signals:

protected:
    void keyPressEvent(QKeyEvent* event) override;

};

#endif // TETRISVIEW_H
