#ifndef GAMESPACE_H
#define GAMESPACE_H

#include <QObject>
#include "QGraphicsScene"

class GameSpace : public QObject
{
    Q_OBJECT
public:
    GameSpace();

private:
    QGraphicsScene* scene;
};

#endif // GAMESPACE_H
