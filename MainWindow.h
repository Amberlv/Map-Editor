#pragma once


#include <QtWidgets/QMainWindow>
#include "Factory.h"
#include<QMouseEvent>
#include<math.h>
#include<stdlib.h>
#include <QGraphicsScene>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    int m_iCount;//记录鼠标点击次数
    int c;//选择图形标识其中记录的为1-5，即图形编号。
    int choose_count;//选择记录，用于选中时的判断
    QPoint p1, p2;//平移记录点
    QPoint init_1;//初始点击记录点。
    int color;//填充颜色标识:1为红,2为蓝，3为绿色。
private:
    Ui::MainWindow* ui;
    QGraphicsScene*scene;
    QGraphicsView* view;
    QPen pen;
    void createMenus();
    bool loadMap(const QString& fileName);
    void saveMap(const QString& fileName);
    void addPoint();
    void addLine();
   
public:

    _Shape* pt = NULL;//基类指针
    _Line line;//派生：line对象.
    _Point point;//派生：point对象
    _Circle circle;//派生：circle对象
    _Triangle triangle;	//派生：triangle对象
    _Rectangle rectangle;//派生：rectangle对象
    _Polygon polygon;//派生： polygon对象
    QVector<QVector<QPoint>> C;//存放圆
    QVector<QVector<QPoint>> L;//存放线
    QVector<QPoint> P;//存放点
    QVector<QVector<QPoint>> T;//存放三角形。
    QVector<QVector<QPoint>> R;//存放矩形。
    QVector<QVector<QPoint>> PL;//存放多边形。
    QVector<QVector<int>>col_c;//存放圆的填充颜色。第一位为行号,第二位为颜色标识。
    QVector<QVector<int>>col_l;//存放线的填充颜色。第一位为行号,第二位为颜色标识。
    QVector<QVector<int>>col_t;//存放三角形的填充颜色。第一位为行号，第二位为颜色标识。
    QVector<QVector<int>>col_r;//存放矩形的填充颜色。第一位为行号,第二位为颜色标识。
    QVector<QVector<int>>col_pl;//存放多边形的填充颜色。第一位为行号，第二位为颜色标识。
    QVector<QVector<int>> I;//存储选中信息,1为line,2为circle,3为triangle,4为rectangle,5为polygon,第二位则为数组行编号。
    int k_avg, m_avg;//图形中心计算。
    bool is_select = false;//是否为选中状态
     QString fileName;
protected:
    void paintEvent(QPaintEvent*event);//重写窗体重绘事件。
    void mousePressEvent(QMouseEvent* event);//重写鼠标按下事件。
    void mouseMoveEvent(QMouseEvent* event);//重写鼠标移动事件。
    void mouseReleaseEvent(QMouseEvent* event);//重写鼠标释放事件。
    void keyPressEvent(QKeyEvent* event);//键盘按下事件。
    void get_center(QVector<QPoint>& s);//获得图形中心。
    void Rotate(QVector<QPoint>& s, double angle);//图形旋转事件。
   
private slots:
    void onLoadMapClicked();
    void onSaveMapClicked();
    void zoomIn();
    void zoomOut();
    void pan(bool on); 
    void on_actionTriangle_triggered();//三角形槽函数
    void on_actionCircle_triggered();//圆槽函数
    void on_actionRectangle_triggered();//矩形槽函数
    void on_actionPolygon_triggered();//多边形槽函数
    void on_actionchoose_triggered();//选择槽函数
    void on_actiondelete_2_triggered();//删除槽函数
    void on_actionred_triggered();//填充红色函数。
    void on_actionblue_triggered();//填充蓝色函数。
    void on_actiongreen_triggered();//填充绿色函数。
    void on_actiondeletefill_triggered();//清除填充。
  
    
    
};

