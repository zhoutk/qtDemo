#include "customgraphbase.h"

CustomGraphBase::CustomGraphBase() : id(QString::number(snowFlakeId.nextid()))
{

}

CustomGraphBase::CustomGraphBase(QString id) : id(id)
{

}

CustomGraphBase::~CustomGraphBase()
{
}
