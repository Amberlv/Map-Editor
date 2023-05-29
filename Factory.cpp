#include"Factory.h"
#include<QPainterPath>
double dis_PointtoLine(QPoint p1, QPoint p2, QPoint p3)
{
    double n = fabs((p2.y() - p3.y()) * p1.x() + (p3.x() - p2.x()) * p1.y() + p2.x() * p3.y() - p2.y() * p3.x()) / sqrt(pow(p2.y() - p3.y(), 2) + pow(p3.x() - p2.x(), 2));
    return n;
}
double dis_PointtoPoint(QPoint p1, QPoint p2)
{
    double n = sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
    return n;
}
double cos_tangle(QPoint p1, QPoint p2, QPoint p3)
{
    double n = pow(dis_PointtoPoint(p1, p2), 2) + pow(dis_PointtoPoint(p1, p3), 2) - pow(dis_PointtoPoint(p2, p3), 2);
    return n;
}

void Lineconnect_fill(QVector<QPoint>vertices, QPainterPath& path)
{
    //边相关扫描线算法
    int maxY = 0;
    Edge* AET;
    for (int i = 0; i < vertices.size(); i++)
    {
        if (vertices[i].y() > maxY)
            maxY = vertices[i].y();
    }



    Edge** pET = new Edge * [maxY] {};
    for (int i = 0; i < maxY; i++)
    {
        pET[i] = new Edge();
        pET[i]->next = nullptr;
    }

    AET = new Edge();
    AET->next = nullptr;




    //建立边表ET
    for (int i = 0; i < vertices.size(); i++)
    {
        //取出当前点1前后相邻的共4个点，点1与点2的连线作为本次循环处理的边，另外两个点点0和点3用于计算奇点
        int x0 = vertices[(i - 1 + vertices.size()) % vertices.size()].x();
        int x1 = vertices[i].x();
        int x2 = vertices[(i + 1) % vertices.size()].x();
        int x3 = vertices[(i + 2) % vertices.size()].x();
        int y0 = vertices[(i - 1 + vertices.size()) % vertices.size()].y();
        int y1 = vertices[i].y();
        int y2 = vertices[(i + 1) % vertices.size()].y();
        int y3 = vertices[(i + 2) % vertices.size()].y();
        //水平线直接舍弃
        if (y1 == y2)
            continue;
        //分别计算下端点y坐标、上端点y坐标、下端点x坐标和斜率倒数
        int ymin = y1 > y2 ? y2 : y1;
        int ymax = y1 > y2 ? y1 : y2;
        float x = y1 > y2 ? x2 : x1;
        float dx = (x1 - x2) * 1.0f / (y1 - y2);
        //奇点特殊处理，若点2->1->0的y坐标单调递减则y1为奇点，若点1->2->3的y坐标单调递减则y2为奇点
        if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3)))
        {
            ymin++;
            x += dx;
        }
        //创建新边，插入边表ET
        Edge* p = new Edge();
        p->ymax = ymax;
        p->x = x;
        p->dx = dx;
        p->next = pET[ymin]->next;
        pET[ymin]->next = p;
    }

    //扫描线从下往上扫描，y坐标每次加1
    for (int i = 0; i < maxY; i++)
    {
        //取出ET中当前扫描行的所有边并按x的递增顺序（若x相等则按dx的递增顺序）插入AET
        while (pET[i]->next)
        {
            //取出ET中当前扫描行表头位置的边
            Edge* pInsert = pET[i]->next;
            Edge* p = AET;
            //在AET中搜索合适的插入位置
            while (p->next)
            {
                if (pInsert->x > p->next->x)
                {
                    p = p->next;
                    continue;
                }
                if (pInsert->x == p->next->x && pInsert->dx > p->next->dx)
                {
                    p = p->next;
                    continue;
                }
                //找到位置
                break;
            }
            //将pInsert从ET中删除，并插入AET的当前位置
            pET[i]->next = pInsert->next;
            pInsert->next = p->next;
            p->next = pInsert;
        }

        //AET中的边两两配对并填色
        Edge* p = AET;
        while (p->next && p->next->next)
        {
            for (int x = p->next->x; x < p->next->next->x; x++)
            {
                path.lineTo(x, i);
            }
            p = p->next->next;
        }

        //删除AET中满足y=ymax的边
        p = AET;
        while (p->next)
        {
            if (p->next->ymax == i)
            {
                Edge* pDelete = p->next;
                p->next = pDelete->next;
                pDelete->next = nullptr;
                delete pDelete;
            }
            else
            {
                p = p->next;
            }
        }

        //更新AET中边的x值，进入下一循环
        p = AET;
        while (p->next)
        {
            p->next->x += p->next->dx;
            p = p->next;
        }

    }

}
void _Line::draw_shape(QPoint P1, QPoint P2, QPainterPath& path)//画线算法
{
    path.moveTo(P1);
    path.lineTo(P2);

}
bool _Line::Liang_Barsky(QVector<QVector<QPoint>>& s, int xmin, int ymin, int xmax, int ymax)
{
    for (int n = 0; n < s.size();)
    {
        int dx = s[n][1].x() - s[n][0].x();
        int dy = s[n][1].y() - s[n][0].y();
        double u1 = 0, u2 = 1;
        int p[5], q[5];
        p[1] = -dx; q[1] = s[n][0].x() - xmin;
        p[2] = dx; q[2] = xmax - s[n][0].x();
        p[3] = -dy; q[3] = s[n][0].y() - ymin;
        p[4] = dy; q[4] = ymax - s[n][0].y();
        bool judge = true;
        //初始时，u1、u2分别表示(x1,y1)、(x2,y2)两个点的参数
        for (int i = 1; i <= 4; i++) {
            if (p[i] < 0) {
                u1 = std::max(u1, ((double)q[i]) / p[i]);
            }
            else if (p[i] > 0) {
                u2 = std::min(u2, ((double)q[i]) / p[i]);
            }
            else if (p[i] == 0 && q[i] < 0) {
                n++;
                judge = false;
                break;
            }
            //判断是否舍弃
            if (u1 > u2) {
                s[n][0].rx() = -1;
                s[n][0].ry() = -1;
                s[n][1].ry() = -1;
                s[n][1].rx() = -1;
                n++;
                break;
            }
        }
        if (u1 < u2 && judge)
        {
            int new_x1 = qRound(s[n][0].x() + u1 * dx);
            int new_y1 = qRound(s[n][0].y() + u1 * dy);
            int new_x2 = qRound(s[n][0].x() + u2 * dx);
            int new_y2 = qRound(s[n][0].y() + u2 * dy);
            s[n][0].rx() = new_x1;
            s[n][0].ry() = new_y1;
            s[n][1].rx() = new_x2;
            s[n][1].ry() = new_y2;
            n = n + 1;
        }
        //x1 = new_x1;
        //y1 = new_y1;
        //x2 = new_x2;
        //y2 = new_y2;
    }
    return true;
}
void _Circle::draw_shape(QPoint p1, QPoint p2, QPainterPath& path)//画圆算法
{
    //根据两点坐标计算圆心和半径
    QPoint center = QPoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
    int radius = int(sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2)) / 2);

    //添加圆到场景中
    path.addEllipse(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
}
void _Point::draw_shape(QPoint x1)
{

}
void _Triangle::draw_shape(QPoint x1, QPoint x2, QPoint x3, QPainterPath& path)//画三角形算法,通过画线来生成
{
    _Line::draw_shape(x1, x2, path);
    _Line::draw_shape(x2, x3, path);
    _Line::draw_shape(x3, x1, path);
}
void _Rectangle::draw_shape(QPoint x1, QPoint x2, QPoint x3, QPoint x4, QPainterPath& path)//画矩形算法，也是通过调用直线画线算法生成。
{
    //QPoint x3(x2.x(),x1.y());
    //QPoint x4(x1.x(),x2.y());
    _Line::draw_shape(x1, x2, path);
    _Line::draw_shape(x2, x3, path);
    _Line::draw_shape(x3, x4, path);
    _Line::draw_shape(x4, x1, path);
}
void _Polygon::draw_shape(QVector<QPoint> s, QPainterPath& path)//画多边形算法，通过存放点的QVector数组进行绘制
{
    // 创建一个 QPolygonF 对象，并将各个顶点添加进去
    QPolygonF polygon;
    for (int i = 0; i < s.size(); i++)
    {
        polygon << QPointF(s[i].x(), s[i].y());
    }

    // 在 QPainterPath 对象中添加这个 QPolygonF 对象
    path.addPolygon(polygon);
}
bool _Line::IsContain(QPoint& p, QPoint p1, QPoint p2)//选中时的具体实现（直线）。
{
    double cos1, cos2, dis;
    cos1 = cos_tangle(p1, p, p2);
    cos2 = cos_tangle(p2, p, p1);
    dis = dis_PointtoLine(p, p1, p2);
    if (cos1 >= 0 && cos2 >= 0 && dis <= 3) return true;
    else return false;
}
bool _Triangle::IsContain(QPoint& p, QPoint p1, QPoint p2, QPoint p3)//选中时的具体实现（三角形）
{
    double signOfTrig = (p2.x() - p1.x()) * (p3.y() - p1.y()) - (p2.y() - p1.y()) * (p3.x() - p1.x());
    double signOfAB = (p2.x() - p1.x()) * (p.y() - p1.y()) - (p2.y() - p1.y()) * (p.x() - p1.x());
    double signOfCA = (p1.x() - p3.x()) * (p.y() - p3.y()) - (p1.y() - p3.y()) * (p.x() - p3.x());
    double signOfBC = (p3.x() - p2.x()) * (p.y() - p3.y()) - (p3.y() - p2.y()) * (p.x() - p3.x());
    if (signOfAB * signOfTrig > 0 && signOfCA * signOfTrig > 0 && signOfBC * signOfTrig > 0)
    {
        return true;
    }
    else return false;
}
bool _Rectangle::IsContain(QPoint& p, QVector<QPoint> s)//矩形选中
{
    if (s[0].x() == s[1].x())
    {
        QPoint p1 = s[0];
        QPoint p2 = s[2];
        if (p1.x() < p2.x() && p1.y() < p2.y())
        {
            if (p.x() >= p1.x() && p.x() <= p2.x() && p.y() >= p1.y() && p.y() <= p2.y())
                return true;
            else return false;
        }
        else if (p1.x() < p2.x() && p1.y() > p2.y())
        {
            if (p.x() >= p1.x() && p.x() <= p2.x() && p.y() >= p2.y() && p.y() <= p1.y())
                return true;
            else return false;
        }
        else if (p1.x() > p2.x() && p1.y() > p2.y())
        {
            if (p.x() >= p2.x() && p.x() <= p1.x() && p.y() >= p2.y() && p.y() <= p1.y())
                return true;
            else return false;
        }
        else if (p1.x() > p2.x() && p1.y() < p2.y())
        {
            if (p.x() >= p2.x() && p.x() <= p1.x() && p.y() >= p1.y() && p.y() <= p2.y())
                return true;
            else return false;
        }
    }
    else
    {
        int i, j = s.size() - 1;
        bool oddNodes = false;
        for (i = 0; i < s.size(); i++)
        {
            if (s[i].ry() < p.ry() && s[j].ry() >= p.ry() || s[j].ry() < p.ry() && s[i].ry() >= p.ry())
            {
                if (s[i].x() + (p.y() - s[i].y()) / (s[j].y() - s[i].y()) * (s[j].x() - s[i].x()) < p.x())
                    oddNodes = !oddNodes;
            }
            j = i;
        }
        return oddNodes;
    }
    
}
bool _Circle::IsContain(QPoint& p, QPoint p1, QPoint p2)//圆选中
{
    double dis = dis_PointtoPoint(p, p1);
    double r = dis_PointtoPoint(p1, p2);
    if (dis <= r)
        return true;
    else return false;
}
bool _Polygon::IsContain(QPoint& p, QVector<QPoint> s)//多边形选中
{
    int i, j = s.size() - 1;
    bool oddNodes = false;
    for (i = 0; i < s.size(); i++)
    {
        if (s[i].ry() < p.ry() && s[j].ry() >= p.ry() || s[j].ry() < p.ry() && s[i].ry() >= p.ry())
        {
            if (s[i].x() + (p.y() - s[i].y()) / (s[j].y() - s[i].y()) * (s[j].x() - s[i].x()) < p.x())
                oddNodes = !oddNodes;
        }
        j = i;
    }
    return oddNodes;
}
void _Polygon::fill(QVector<QPoint> s, QPainterPath& path)
{
    Lineconnect_fill(s, path);
}
void _Triangle::fill(QVector<QPoint> s, QPainterPath& path)
{
    Lineconnect_fill(s, path);
}
void _Rectangle::fill(QVector<QPoint> s, QPainterPath& path)
{
    if (s[1].x() == s[2].x())
    {
        int xmin, xmax, ymin, ymax;
        if (s[0].x() > s[2].x())
        {
            xmin = s[2].x();
            xmax = s[0].x();
        }
        else
        {
            xmin = s[0].x();
            xmax = s[2].x();
        }
        if (s[0].y() > s[2].x())
        {
            ymin = s[2].y();
            ymax = s[0].y();
        }
        else
        {
            ymin = s[0].y();
            ymax = s[2].y();
        }
        for (int i = xmin + 1; i < xmax; i++)
        {
            for (int j = ymin + 1; j < ymax; j++)
            {
                path.lineTo(i, j);
            }
        }
    }
    else
    {
        Lineconnect_fill(s, path);
    }
}


void _Circle::fill(QVector<QPoint> s, QPainterPath& path)
{
    int R = dis_PointtoPoint(s[0], s[1]);
    int ymin = s[0].y() - R;
    int ymax = s[0].y() + R;
    for (int i = ymin + 1; i <= s[0].y(); i++)
    {
        int x1, x2;
        x1 = s[0].x() - sqrt(pow(R, 2) - pow(i - s[0].y(), 2));
        x2 = s[0].x() + sqrt(pow(R, 2) - pow(i - s[0].y(), 2));
        for (int j = x1 + 1; j < x2; j++)
        {
            path.lineTo(j, i);
        }
    }
    for (int i = s[0].y() + 1; i < ymax; i++)
    {
        int x3, x4;
        x3 = s[0].x() - sqrt(pow(R, 2) - pow(i - s[0].y(), 2));
        x4 = s[0].x() + sqrt(pow(R, 2) - pow(i - s[0].y(), 2));
        for (int j = x3 + 1; j < x4; j++)
        {
            path.lineTo(j, i);
        }
    }
}
