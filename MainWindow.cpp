#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"Factory.h"
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include "shapefil.h"
#include<QPainterPath>
#include<QColor>
#include<QtGlobal> 
#include <QGraphicsItem>
#include <QGraphicsRectItem>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
 {
  
    ui->setupUi(this);
   
    // 创建视图
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
  
    


    // 关联信号和槽
    connect(ui->actionLoadMap, &QAction::triggered, this, &MainWindow::onLoadMapClicked);
    connect(ui->actionSaveMap, &QAction::triggered, this, &MainWindow::onSaveMapClicked);
   
   
    // Create menus for editing features
    createMenus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenus()
{
    // Add a menu for adding features
    QMenu* addFeatureMenu = ui->menuBar->addMenu(tr("添加要素"));

    // Add a sub-menu for adding a point feature
    QAction* addPointAction = new QAction(tr("Add Point"), addFeatureMenu);
    addFeatureMenu->addAction(addPointAction);
    connect(addPointAction, &QAction::triggered, this, &MainWindow::addPoint);

    // Add a sub-menu for adding a line feature
    QAction* addLineAction = new QAction(tr("Add Line"), addFeatureMenu);
    addFeatureMenu->addAction(addLineAction);
    connect(addLineAction, &QAction::triggered, this, &MainWindow::addLine);

    // Add a sub-menu for adding a Circle feature
    QAction* addCircleAction = new QAction(tr("Add Circle"), addFeatureMenu);
    addFeatureMenu->addAction(addCircleAction);
    connect(addCircleAction, &QAction::triggered, this, &MainWindow::on_actionCircle_triggered);

    // Add a sub-menu for adding a Rectangle feature
    QAction* addRectangleAction = new QAction(tr("Add Rectangle"), addFeatureMenu);
    addFeatureMenu->addAction(addRectangleAction);
    connect(addRectangleAction, &QAction::triggered, this, &MainWindow::on_actionRectangle_triggered);
    // Add a sub-menu for adding a Triangle feature
    QAction* addTriangleAction = new QAction(tr("Add Triangle"), addFeatureMenu);
    addFeatureMenu->addAction(addTriangleAction);
    connect(addTriangleAction, &QAction::triggered, this, &MainWindow::on_actionTriangle_triggered);
    // Add a sub-menu for adding a Polygon feature
    QAction* addPolygonAction = new QAction(tr("AddPolygon"), addFeatureMenu);
    addFeatureMenu->addAction(addPolygonAction);
    connect(addPolygonAction, &QAction::triggered, this, &MainWindow::on_actionPolygon_triggered);
    // Add a menu for edit feature
    QMenu* editMenu = ui->menuBar->addMenu(tr("编辑选中要素"));

   
    //添加删除要素选项
        QAction * deleteAction = new QAction(tr("删除选中要素"), this);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::on_actiondelete_2_triggered);
    
    editMenu->addAction(deleteAction);
   
    //添加填充红色选项
    QAction* redAction = new QAction(tr("填充红色"), this);
    editMenu->addAction(redAction);
    connect(redAction, &QAction::triggered, this, &MainWindow::on_actionred_triggered);
    //添加填充蓝色选项
    QAction* blueAction = new QAction(tr("填充蓝色"), this);
    editMenu->addAction(blueAction);
    connect(blueAction, &QAction::triggered, this, &MainWindow::on_actionblue_triggered);
    //添加填充绿色选项
    QAction* greenAction = new QAction(tr("填充绿色"), this);
    editMenu->addAction(greenAction);
    connect(greenAction, &QAction::triggered, this, &MainWindow::on_actiongreen_triggered);
    //添加清除填充选项
    QAction* deletefillAction = new QAction(tr("清除填充"), this);
    editMenu->addAction(deletefillAction);
    connect(deletefillAction, &QAction::triggered, this, &MainWindow::on_actiondeletefill_triggered);
  
  
     //添加缩放工具按钮
    QAction* zoomInAction = new QAction(tr("放大"), this);
    zoomInAction->setIcon(QIcon("C:/Users/86177/Desktop/zoom-out.png"));
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

    QAction* zoomOutAction = new QAction(tr("缩小"), this);
    zoomOutAction->setIcon(QIcon("C:/Users/86177/Desktop/zoom-in.png"));
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
    ui->menuBar->addAction(zoomInAction);
    ui->menuBar->addAction(zoomOutAction);
    //添加选中要素选项
    QAction* addonchooseAction = new QAction(tr("选中要素"), this);
    connect(addonchooseAction, &QAction::triggered, this, &MainWindow::on_actionchoose_triggered);
    ui->menuBar->addAction(addonchooseAction);
    addonchooseAction->setIcon(QIcon("C:/Users/86177/Desktop/select.png"));
   
 
}

bool MainWindow::loadMap(const QString& fileName)
{
    //使用 shapelib 库加载 shapefile 文件
    SHPHandle hSHP = SHPOpen(fileName.toLocal8Bit(), "rb");
    if (!hSHP) {
        QMessageBox::warning(this, tr("错误"), tr("打开文件失败!"));
        return false;
    }

    int nEntities, nShapeType;
    int pointSize = 16;

    QPen pen(Qt::NoPen);
    double adfMinBound[4], adfMaxBound[4];
    SHPGetInfo(hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
    QColor penColor(Qt::darkGray);

    QGraphicsRectItem* cover = scene->addRect(scene->sceneRect(), Qt::NoPen, QColor(255, 255, 255, 127));
    for (int i = 0; i < nEntities; ++i) {
        SHPObject* object = SHPReadObject(hSHP, i);
        QPointF center(object->padfX[0], object->padfY[0]);  // 点的中心坐标
        if (!object) {
            QMessageBox::warning(this, tr("错误"), tr("对象为空!"));
            break;
        }

        switch (object->nSHPType) {
        case SHPT_POINT:

            pen.setWidthF(0.5);  // 设置笔刷宽度
            pen.setColor(QColor(Qt::yellow).darker(150));  // 调整笔刷颜色
            scene->addEllipse(object->padfX[0] - pointSize / 2, object->padfY[0] - pointSize / 2, pointSize, pointSize, pen, QBrush(Qt::yellow, Qt::Dense2Pattern));
            break;
        case SHPT_POLYGON:
        {
            const int nParts = object->nParts;
            QVector<QPointF> polygonPoints;

            //提取每个面的坐标数据点并组成多边形对象
            for (int j = 0; j < nParts; ++j) {
                int startIndex = object->panPartStart[j];
                int endIndex;
                if (j != nParts - 1)
                    endIndex = object->panPartStart[j + 1];
                else
                    endIndex = object->nVertices;
                for (int k = startIndex; k < endIndex; ++k) {
                    polygonPoints.append(QPointF(object->padfX[k], object->padfY[k]));
                }
            }

            QPolygonF polygon(polygonPoints);
            scene->addPolygon(polygon, QPen(Qt::NoPen), QBrush(Qt::yellow));
        }
        break;
        case SHPT_ARC:
        {
            const int nParts = object->nParts;
            QVector<QPointF> arcPoints;

            for (int j = 0; j < nParts; ++j) {
                int startIndex = object->panPartStart[j];
                int endIndex;
                if (j != nParts - 1)
                    endIndex = object->panPartStart[j + 1];
                else
                    endIndex = object->nVertices;
                for (int k = startIndex; k < endIndex; ++k) {
                    arcPoints.append(QPointF(object->padfX[k], object->padfY[k]));
                }
            }

            QPainterPath path;

            for (int i = 0; i < arcPoints.count() - 1; ++i) {
                QPointF startPoint = arcPoints.at(i);
                QPointF endPoint = arcPoints.at(i + 1);
                path.moveTo(startPoint);
                path.lineTo(endPoint);
            }

            // 修改此处的Qt::NoPen为其他线条格式，比如Qt::SolidLine
            scene->addPath(path, QPen(Qt::blue, 2, Qt::SolidLine), QBrush());
        }
        break;
        default:
            break;
        }

        SHPDestroyObject(object);
    }

    SHPClose(hSHP);
    QRectF itemsBoundingRect = scene->itemsBoundingRect();
    QSize viewportSize = ui->graphicsView->viewport()->size();
    QPointF center = itemsBoundingRect.center();
    double scaleX = viewportSize.width() / itemsBoundingRect.width();
    double scaleY = viewportSize.height() / itemsBoundingRect.height();
    double scaleFactor = qMin(scaleX, scaleY);
    ui->graphicsView->setTransform(QTransform::fromScale(scaleFactor, -scaleFactor), false);
    ui->graphicsView->centerOn(center);

    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);



    scene->removeItem(qgraphicsitem_cast<QGraphicsItem*>(cover));
    delete cover;
    return true;
}
void MainWindow::saveMap(const QString& fileName)
{
    QPixmap pixmap = ui->graphicsView->grab();

    if (fileName.endsWith(".bmp"))
        pixmap.save(fileName, "BMP");
    else if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg"))
        pixmap.save(fileName, "JPEG");
    else if (fileName.endsWith(".png"))
        pixmap.save(fileName, "PNG");
    else
        QMessageBox::warning(this, tr("Warning"), tr("Invalid image format: ") + fileName);
}

void MainWindow::addPoint()
{
    
       
        is_select = false;
        if (pt != &point)
        {
            m_iCount = 0;
            pt = &point;
        }
        else
        {
            pt = &point;
        }
    
}

void MainWindow::addLine()
{
    
    is_select = false;
    if (pt != &line)
    {
        m_iCount = 0;
        pt = &line;
    }
    else
    {
        pt = &line;
    }
   
}

void MainWindow::onLoadMapClicked()
{
   
 fileName = QFileDialog::getOpenFileName(this, tr("打开地图文件"),
        "", tr("Shapefile (*.shp)"));
    
}

void MainWindow::onSaveMapClicked()
{
    
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png *.bmp *.jpg)"));
    if (!fileName.isNull())
    {
        saveMap(fileName);
    }
}
void MainWindow::zoomIn()
{
    ui->graphicsView->scale(1.2, 1.2);
}

void MainWindow::zoomOut()
{
    ui->graphicsView->scale(1 / 1.2, 1 / 1.2);
}

void MainWindow::pan(bool on)
{
    if (on) {
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    }
    else {
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    }
}

void MainWindow::on_actionchoose_triggered()
{
    is_select = true;//选择模式状态
}
void MainWindow::on_actiondelete_2_triggered()//删除图形（其实是将vector容器中的点全部设置为（-1，-1））
{
    if (is_select == true && I.size() != 0)
    {
        QPoint p(-1, -1);
        if (I[c][0] == 1)
        {
            L[I[c][1]][0] = p;
            L[I[c][1]][1] = p;
        }
        else if (I[c][0] == 2)
        {
            C[I[c][1]][0] = p;
            C[I[c][1]][1] = p;
        }
        else if (I[c][0] == 3)
        {
            T[I[c][1]][0] = p;
            T[I[c][1]][1] = p;
            T[I[c][1]][2] = p;
        }
        else if (I[c][0] == 4)
        {
            R[I[c][1]][0] = p;
            R[I[c][1]][1] = p;
            R[I[c][1]][2] = p;
            R[I[c][1]][3] = p;
        }
        else if (I[c][0] == 5)
        {
            for (int i = 0; i < PL[I[c][1]].size(); i++)
            {
                PL[I[c][1]][i] = p;
            }
        }
        scene->update();//调用窗口重绘函数
    }
}
void MainWindow::on_actionTriangle_triggered()
{
    //m_iMenu = 4;
    is_select = false;//在画图时设置为非选择模式
    if (pt != &triangle)//如果在选择模式时上一个模式不是triangle，将鼠标点击计数记为0，防止vector容器读取错误
    {
        m_iCount = 0;
        pt = &triangle;
    }
    else
    {
        pt = &triangle;
    }
}


void MainWindow::on_actionCircle_triggered()
{
    //m_iMenu = 6;
    is_select = false;
    if (pt != &circle)
    {
        m_iCount = 0;
        pt = &circle;
    }
    else pt = &circle;
}
void MainWindow::on_actionRectangle_triggered()
{
    //m_iMenu = 5;
    is_select = false;
    if (pt != &rectangle)
    {
        m_iCount = 0;
        pt = &rectangle;
    }
    else pt = &rectangle;
}
void MainWindow::on_actionPolygon_triggered()
{
    
    is_select = false;
    if (pt != &polygon)
    {
        m_iCount = 0;
        pt = &polygon;
    }
    else
    {
        pt = &polygon;
    }
}


void MainWindow::on_actionred_triggered()
{
    if (is_select == true && I.size() != 0)
    {
        if (I[c][0] == 1)//为线
        {
            col_l.push_back(QVector<int>());
            col_l.back().push_back(I[c][1]);
            col_l.back().push_back(1);
        }
        else if (I[c][0] == 2)//为圆
        {
            col_c.push_back(QVector<int>());
            col_c.back().push_back(I[c][1]);
            col_c.back().push_back(1);
        }
        else if (I[c][0] == 3)//为三角形
        {
            col_t.push_back(QVector<int>());
            col_t.back().push_back(I[c][1]);
            col_t.back().push_back(1);
        }
        else if (I[c][0] == 4)//为矩形
        {
            col_r.push_back(QVector<int>());
            col_r.back().push_back(I[c][1]);
            col_r.back().push_back(1);
        }
        else if (I[c][0] == 5)//为多边形
        {
            col_pl.push_back(QVector<int>());
            col_pl.back().push_back(I[c][1]);
            col_pl.back().push_back(1);
        }
    }
    update();
}
void MainWindow::on_actionblue_triggered()
{
    if (is_select == true && I.size() != 0)
    {
        if (I[c][0] == 1)//为线
        {
            col_l.push_back(QVector<int>());
            col_l.back().push_back(I[c][1]);
            col_l.back().push_back(2);
        }
        else if (I[c][0] == 2)//为圆
        {
            col_c.push_back(QVector<int>());
            col_c.back().push_back(I[c][1]);
            col_c.back().push_back(2);
        }
        else if (I[c][0] == 3)//为三角形
        {
            col_t.push_back(QVector<int>());
            col_t.back().push_back(I[c][1]);
            col_t.back().push_back(2);
        }
        else if (I[c][0] == 4)//为矩形
        {
            col_r.push_back(QVector<int>());
            col_r.back().push_back(I[c][1]);
            col_r.back().push_back(2);
        }
        else if (I[c][0] == 5)//为多边形
        {
            col_pl.push_back(QVector<int>());
            col_pl.back().push_back(I[c][1]);
            col_pl.back().push_back(2);
        }
    }
    update();
}
void MainWindow::on_actiongreen_triggered()
{
    if (is_select == true && I.size() != 0)
    {
        if (I[c][0] == 1)//为线
        {
            col_l.push_back(QVector<int>());
            col_l.back().push_back(I[c][1]);
            col_l.back().push_back(3);
        }
        else if (I[c][0] == 2)//为圆
        {
            col_c.push_back(QVector<int>());
            col_c.back().push_back(I[c][1]);
            col_c.back().push_back(3);
        }
        else if (I[c][0] == 3)//为三角形
        {
            col_t.push_back(QVector<int>());
            col_t.back().push_back(I[c][1]);
            col_t.back().push_back(3);
        }
        else if (I[c][0] == 4)//为矩形
        {
            col_r.push_back(QVector<int>());
            col_r.back().push_back(I[c][1]);
            col_r.back().push_back(3);
        }
        else if (I[c][0] == 5)//为多边形
        {
            col_pl.push_back(QVector<int>());
            col_pl.back().push_back(I[c][1]);
            col_pl.back().push_back(3);
        }
    }
    update();
}
void MainWindow::on_actiondeletefill_triggered() {
    if (is_select == true && I.size() != 0)
    {
        if (I[c][0] == 1)//为线
        {
            for (int i = 0; i < col_l.size(); i++)
            {
                if (col_l[i][0] == I[c][1])
                {
                    col_l[i][1] = 0;
                }
            }
        }
        if (I[c][0] == 2)//为圆
        {
            for (int i = 0; i < col_c.size(); i++)
            {
                if (col_c[i][0] == I[c][1])
                {
                    col_c[i][1] = 0;
                }
            }
        
        }
        if (I[c][0] == 3)//为三角形
        {
            for (int i = 0; i < col_t.size(); i++)
            {
                if (col_t[i][0] == I[c][1])
                {
                    col_t[i][1] = 0;
                }
            }
            //col_t[I[c][1]][1] = 0;
        }
        if (I[c][0] == 4)//为矩形
        {
            for (int i = 0; i < col_r.size(); i++)
            {
                if (col_r[i][0] == I[c][1])
                {
                    col_r[i][1] = 0;
                }
            }
            //col_r[I[c][1]][1] = 0;
        }
        if (I[c][0] == 5)//为多边形
        {
            for (int i = 0; i < col_pl.size(); i++)
            {
                if (col_pl[i][0] == I[c][1])
                {
                    col_pl[i][1] = 0;
                }
            }
        }
    }
    update();
}
