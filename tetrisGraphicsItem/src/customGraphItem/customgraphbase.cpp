#include "customgraphbase.h"

CustomGraphBase::CustomGraphBase() : id(QString::number(snowFlakeId.nextid())),
	lineStyle(0), lineWidth(1), colorValue(0), fillStyle(0)
{

}

CustomGraphBase::CustomGraphBase(QString id) : id(id)
{

}

CustomGraphBase::~CustomGraphBase()
{
}
