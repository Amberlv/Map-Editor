#pragma once
#include<qvector.h>
#include<qpainter.h>
#include<math.h>
#include<qglobal.h>

#define PI 3.14159265358979323846
class _Shape//基类
{
public:
	virtual void draw_shape(QPoint x1) {};
	virtual void draw_shape(QPoint x1, QPoint x2, QPainterPath& path) {};
	virtual void draw_shape(QPoint x1, QPoint x2, QPoint x3, QPainterPath& path) {};
	virtual void draw_shape(QVector<QPoint> s, QPainterPath& path) {};
	virtual void fill(QVector<QPoint> s, QPainterPath& path) {};
};
class _Circle :public _Shape//圆类
{
	//public:
		//QVector<QVector<QPoint>> C;
public:
	void draw_shape(QPoint x1, QPoint x2, QPainterPath& path);
	bool IsContain(QPoint& p, QPoint p1, QPoint p2);//判断点是否在圆内
	void fill(QVector<QPoint> s, QPainterPath& path);
};
class _Line :public _Shape//线类
{
public:
	//QVector<QVector<QPoint>> L;
public:
	void draw_shape(QPoint x1, QPoint x2, QPainterPath& path);
	bool IsContain(QPoint& p, QPoint p1, QPoint p2);
	bool Liang_Barsky(QVector<QVector<QPoint>>& s, int xmin, int ymin, int xmax, int ymax);
	
};
class _Point : public _Shape//点类
{
	//public:
		//QVector<QPoint> P;
public:
	void draw_shape(QPoint x1);
};
class _Triangle : public _Line//三角形类
{
public:
	void draw_shape(QPoint x1, QPoint x2, QPoint x3, QPainterPath& path);
	bool IsContain(QPoint& p, QPoint p1, QPoint p2, QPoint p3);
	void fill(QVector<QPoint> s, QPainterPath& path);
};
class _Rectangle : public _Line//矩形类
{
public:
	void draw_shape(QPoint x1, QPoint x2, QPoint x3, QPoint x4, QPainterPath& path);
	bool IsContain(QPoint& p, QVector<QPoint> s);
	void fill(QVector<QPoint> s, QPainterPath& path);
};
class _Polygon : public _Line//多边形类
{
public:
	void draw_shape(QVector<QPoint> s, QPainterPath& path);
	bool IsContain(QPoint& p, QVector<QPoint> s);
	void fill(QVector<QPoint> s, QPainterPath& path);
	
};
class Edge
{
public:
	int ymax;
	float x;
	float dx;
	Edge* next;
};

//定义用于表示像素点坐标的类Point
class Point
{
public:
	int x;
	int y;
	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

double dis_PointtoLine(QPoint p, QPoint p1, QPoint p2);//点到线的距离
double dis_PointtoPoint(QPoint p1, QPoint p2);//点到点的距离
double cos_tangle(QPoint p1, QPoint p2, QPoint p3);//余弦值计算（p1为角的顶点,p2,p3为确定两条线段的端点）
void Lineconnect_fill(QVector<QPoint>s, QPainterPath& path);//边相关扫描线算法。
