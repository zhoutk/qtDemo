# QTetris -- Qt5之QGraphicsItem编写Tetris俄罗斯方块游戏

## 背景

使用Qt5.12.9的QGraphicsItem来实现俄罗斯方块，现在是C++版本，下来还会有python版本，以及方便的接口，来接入算法，由机器人玩俄罗斯方块。

## 思路

- CustomGraphBase类继承自QGraphicsObject，提供必要的虚函数。
- CustomGraphTetrisBlock类继承自CustomGraphBase，实现最小方块，分边框类型（0）与方块类型（1）。
- CustomGraphTetrisText类继承自CustomGraphBase，显示文字，类型为5。
- Tetris类组合CustomGraphTetrisBlock，显示俄罗斯方块。
- Game类为游戏逻辑控制类。

    该游戏传统的编程方式，是用一个二维数组来控制游戏空间，类似迷宫的方式。其实选择QGraphicsItem来实现就是一种很另类的选择，其实用gdi来做更方便，这种规模，QGraphicsItem没有优势，只是个人学习探索的选择。  
    我没有用二维数组来控制游戏空间，而是在边沿上用了一圏CustomGraphTetrisBlock来定义游戏空间，因为所有的items都能方便的在scene上检索到，所以看一个方块是否能移动，就需要检索自己的周围是否已经被其它方块占据。这里有一点，在方块进行旋转的时候，就要判断区分组成自己的block和别人的方块。

## 效果图
![输入图片说明](https://images.gitee.com/uploads/images/2021/0506/162358_07e427e8_332944.png "tetris.png")

##关键代码分析

功能尽量内聚，类CustomGraphTetrisBlock封装小方块，Tetris类组合了Block，封装了俄罗斯方块的绝大部分操作，类Game游戏的整体流程。
### CustomGraphBase自定义图元基类
```
class CustomGraphBase : public QGraphicsObject
{
    Q_OBJECT
public:
	CustomGraphBase();
public:
	virtual QRectF boundingRect() const = 0;  //占位区域，必须准确，才能很好的显示与清除
	virtual int type() const = 0;             
	virtual void relocate() = 0;              //移动，重定位
	virtual bool isActive() { return false; };//未落地的方块
	virtual int getBlockType() { return 0; }; //方块类型，主要区别边沿方块
};
```
### CustomGraphTetrisBlock 最小方块，组成俄罗斯方块的基本元素
paint 重绘操作，需要操作边沿方块，边沿方块只占位，不显示。要注意prepareGeometryChange()函数的使用，不能放在这个函数中，不然会不停的重绘，占用大量CPU资源。具体原理我还没研究透，我将其放到relocateb函数中了。
```
void CustomGraphTetrisBlock::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
{
	if (blockType) {
		painter->drawRoundedRect(
			0,
			0,
			BLOCKSIDEWIDTH,
			BLOCKSIDEWIDTH,
			2, 2
		);
	}
	//prepareGeometryChange();
}
```
relocate元素重定位，只需将其放到scene上正确的坐标
```
void CustomGraphTetrisBlock::relocate()
{
	this->setPos(pos * BLOCKSIDEWIDTH);
	prepareGeometryChange();
}
```
### Tetris类，俄罗斯方块类
七类方块的定义
```
QVector<QVector<int>> SHAPES = {
	{1, 1, 1, 1},
	{0, 1, 1, 1, 0 , 1},
	{1, 1, 1, 0, 0, 0, 1},
	{0, 1, 1, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 1, 1},
	{0, 1, 1, 0, 1, 1},
	{0, 1, 0, 0, 1, 1, 1} 
};
```
俄罗斯方块的构建
```
QVector<int> curShape = SHAPES[shape % SHAPES.size()];
	for (int i = 0; i < curShape.size(); i++) {
		if (curShape[i]) {
			data[1 + i / sideLen][i % sideLen] = true;
			CustomGraphTetrisBlock* block = new CustomGraphTetrisBlock(pos + QPoint(i % sideLen, 1 + i / sideLen), 2, shape);
			blocks.push_back(block);   //存储组成该方块的所有元素，在落到底之前需要由Tetris类控制其运动
			MainWindow::GetApp()->GetScene()->addItem(block);                                   //加入block到scene，显示方块
		}
	}
```
hasTetrisBlock函数检测位置上是否有方块
```
CustomGraphTetrisBlock* Tetris::hasTetrisBlock(int x, int y)
{
	auto items = MainWindow::GetApp()->GetScene()->items(QPointF((x + 0.5) * BLOCKSIDEWIDTH, (y + 0.5) * BLOCKSIDEWIDTH));
	foreach (auto al , items)
	{
		if (!(((CustomGraphBase*)al)->isActive()) && (((CustomGraphBase*)al)->type()) == TETRISBLOCKTYPE) {      //要区别组合俄罗斯方块本身的block与其它的block
			return (CustomGraphTetrisBlock*)al;  //返回方块，提供给清除行操作用
		}
	}
	return nullptr;
}
```
rotate函数进行俄罗斯方块的旋转
```
bool Tetris::rotate()
{
    int i, j, t, lenHalf = sideLen / 2, lenJ;
    for (i = 0; i < lenHalf; i++)
    {
        lenJ = sideLen - i - 1;
		for (j = i; j < lenJ; j++)
		{        //先行判断是否能旋转，要移动的点不为0时，判断目标点是否已经有block存在
			int lenI = sideLen - j - 1;
			if (data[i][j] && this->hasTetrisBlock(pos.x() + lenJ, pos.y() + j) ||
				data[lenI][i] && this->hasTetrisBlock(pos.x() + j, pos.y() + i) ||
				data[lenJ][lenI] && this->hasTetrisBlock(pos.x() + i, pos.y() + lenI) ||
				data[j][lenJ] && this->hasTetrisBlock(pos.x() + lenI, pos.y() + lenJ)){
                return false;
            }
        }
    }
    for (i = 0; i < lenHalf; i++)
	{       //选择了顺时针90度旋转，使用了螺旋移动算法，网上可以容易搜索到说明。
		lenJ = sideLen - i - 1;
		for (j = i; j < lenJ; j++)
		{
            int lenI = sideLen - j - 1;
			t = data[i][j];
            data[i][j] = data[lenI][i];
            data[lenI][i] = data[lenJ][lenI];
            data[lenJ][lenI] = data[j][lenJ];
            data[j][lenJ] = t;
		}
	}
	this->relocate();
	return true;
}
```
cleanRow函数实现行清除
```
int Tetris::cleanRow()
{       //该清除算法效率不高，是以一行来处理的，这块以后可以优化。
	int h = 19, levelCount = 0;
	while (h >= 0) {
		int count = 0;
		for (int i = 0; i < 10; i++) {  //判断是否行满
			if (!this->hasTetrisBlock(i, h)) {
				count++;
			}
		}
		if (count == 0) {               //行满，需要清除并整体下移
			int level = h;
			levelCount++;
			bool first = true;
			while (level >= 0) {
				int ct = 0;
				for (int j = 0; j < 10; j++) {
					if(first)      //第一个外循环删除满行上的图元，后面是整体下移
						this->erase(j, level);
					CustomGraphTetrisBlock* block = this->hasTetrisBlock(j, level - 1);
					if (!block) {
						ct++;
					}
					else {
						block->relocate(QPoint(j, level));     //下移一个位置
					}
				}
				first = false;
				if (ct == 10) {           //一行上都没有图元，工作完成，提前结束
					break;
				}
				else {
					level--;
				}
			}
		}
		else if (count == 10) {
			break;
		}
		else {
			h--;
		}
	}
	return levelCount;
}
```
## 源代码及运行方法

项目采用cmake组织，请安装cmake3.10以上版本。

```
cmake -Bbuild .
cd build
cmake --build . --config Release
```
注：本项目采用方案能跨平台运行，已经适配过windows，linux，mac。


源代码：

```
https://gitee.com/zhoutk/qtetris.git
```
或
```
https://gitee.com/zhoutk/qtdemo/tree/master/tetrisGraphicsItem
```
或
```
https://github.com/zhoutk/qtDemo/tree/master/tetrisGraphicsItem
```