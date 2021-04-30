#ifndef TETRISVIEW_H
#define TETRISVIEW_H

#include <QObject>
#include <QGraphicsView>

class TetrisView : public QGraphicsView
{
    Q_OBJECT
public:
    TetrisView(QWidget* parent = nullptr);

signals:

};

#endif // TETRISVIEW_H
