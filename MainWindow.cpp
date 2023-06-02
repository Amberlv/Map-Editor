#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <shapefil.h>
#include<QPainterPath>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
 {
  
    ui->setupUi(this);
    m_iCount = 0;//初始化鼠标点击次数为0
    choose_count = 0;
    // Create a graphics scene and set it to the view
   
   // 创建场景对象并设置坐标系

    scene = new QGraphicsScene(this);
    scene->setSceneRect(-300, -300, 600, 600);

    // 创建视图对象并关联场景
    view = new QGraphicsView(scene, this);
    setCentralWidget(view);

    // 设置视图显示内容以填充场景
    view->setRenderHint(QPainter::Antialiasing, true);
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
   
    view->setMouseTracking(true); // 开启鼠标跟踪
    

    // Connect signals and slots
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
    QMenu* addFeatureMenu = ui->menuEdit->addMenu(tr("Add Feature"));

    // Add a sub-menu for adding a point feature
    QAction* addPointAction = new QAction(tr("Add Point"), addFeatureMenu);
    addFeatureMenu->addAction(addPointAction);
    connect(addPointAction, &QAction::triggered, this, &MainWindow::addPoint);

    // Add a sub-menu for adding a line feature
    QAction* addLineAction = new QAction(tr("Add Line"), addFeatureMenu);
    addFeatureMenu->addAction(addLineAction);
    connect(addLineAction, &QAction::triggered, this, &MainWindow::addLine);

    // Add a menu for deleting features
    QMenu* deleteFeatureMenu = ui->menuEdit2->addMenu(tr("Delete Feature"));

    // Add a sub-menu for deleting a point feature
    QAction* deletePointAction = new QAction(tr("Delete Point"), deleteFeatureMenu);
    deleteFeatureMenu->addAction(deletePointAction);
    connect(deletePointAction, &QAction::triggered, this, &MainWindow::deletePoint);

    // Add a sub-menu for deleting a line feature
    QAction* deleteLineAction = new QAction(tr("Delete Line"), deleteFeatureMenu);
    deleteFeatureMenu->addAction(deleteLineAction);
    connect(deleteLineAction, &QAction::triggered, this, &MainWindow::deleteLine);
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
    double adfMinBound[4], adfMaxBound[4];
    SHPGetInfo(hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound);

    for (int i = 0; i < nEntities; ++i) {
        SHPObject* object = SHPReadObject(hSHP, i);
        if (!object) continue;

        switch (object->nSHPType) {
        case SHPT_POINT:
            scene->addEllipse(object->padfX[0], object->padfY[0], 2, 2,
                QPen(Qt::NoPen), QBrush(Qt::SolidPattern));
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
            scene->addPolygon(polygon, QPen(Qt::NoPen), QBrush(Qt::SolidPattern));
        }
        break;
        case SHPT_ARC:
        {
            //If the shape type is SHPT_ARC, add code to render it as a polyline object
            const int nParts = object->nParts;
            QVector<QPointF> arcPoints;

            // Extract coordinate data points for each segment of the line and form a polyline object
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

            //Add line segments between consecutive points to form the polyline
            for (int i = 0; i < arcPoints.count() - 1; ++i) {
                QPointF startPoint = arcPoints.at(i);
                QPointF endPoint = arcPoints.at(i + 1);
                path.moveTo(startPoint);
                path.lineTo(endPoint);
            }

            //Render polyline object
            scene->addPath(path, QPen(Qt::NoPen), QBrush(Qt::SolidPattern));
        }

        break;
        default:
            break;
        }

        SHPDestroyObject(object);
    }

    SHPClose(hSHP);

    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
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

void MainWindow::deletePoint()
{
    //TODO: Add code to delete point feature
}

void MainWindow::deleteLine()
{
    //TODO: Add code to delete line feature
}

void MainWindow::onLoadMapClicked()
{
   

    const QString fileName = QFileDialog::getOpenFileName(this, tr("打开地图文件"),
        "", tr("Shapefile (*.shp)"));
    if (!fileName.isNull())
    {
        loadMap(fileName);
    }
}

void MainWindow::onSaveMapClicked()
{
    
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png *.bmp *.jpg)"));
    if (!fileName.isNull())
    {
        saveMap(fileName);
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
        update();//调用窗口重绘函数
    }
}
void MainWindow::on_actionbreak_triggered()
{
    this->close();//关闭界面
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
    //m_iMenu = 7;
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
void MainWindow::on_actioncurve_triggered()
{
    //m_iMenu = 3;
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
            //col_c[I[c][1]][1] = 0;
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
void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (is_select == true && I.size() != 0)//处于选中状态且选中了图形
    {
        double x, y;
        if (event->buttons() & Qt::LeftButton)//左键一直按下
        {
            p2 = event->pos();//p2与p1有次序，p1每次会记录p2上次的位置，作为平移的x,y小量增加基准。
            if (p1.x() != 0 && p1.y() != 0)//跳过第一次，即让p1记录第一次p2的位置。
            {
                x = p2.x() - p1.x();//标识平移距离
                y = p2.y() - p1.y();//标识平移距离
                if (I[c][0] == 1)
                {
                    L[I[c][1]][0].rx() += x;
                    L[I[c][1]][0].ry() += y;
                    L[I[c][1]][1].rx() += x;
                    L[I[c][1]][1].ry() += y;
                }
                else if (I[c][0] == 2)
                {
                    C[I[c][1]][0].rx() += x;
                    C[I[c][1]][0].ry() += y;
                    C[I[c][1]][1].rx() += x;
                    C[I[c][1]][1].ry() += y;
                }
                else if (I[c][0] == 3)
                {
                    T[I[c][1]][0].rx() += x;
                    T[I[c][1]][0].ry() += y;
                    T[I[c][1]][1].rx() += x;
                    T[I[c][1]][1].ry() += y;
                    T[I[c][1]][2].rx() += x;
                    T[I[c][1]][2].ry() += y;
                }
                else if (I[c][0] == 4)
                {
                    R[I[c][1]][0].rx() += x;
                    R[I[c][1]][0].ry() += y;
                    R[I[c][1]][1].rx() += x;
                    R[I[c][1]][1].ry() += y;
                    R[I[c][1]][2].rx() += x;
                    R[I[c][1]][2].ry() += y;
                    R[I[c][1]][3].rx() += x;
                    R[I[c][1]][3].ry() += y;
                }
                else if (I[c][0] == 5)
                {
                    for (int i = 0; i < PL[I[c][1]].size(); i++)
                    {
                        PL[I[c][1]][i].rx() += x;
                        PL[I[c][1]][i].ry() += y;
                    }
                }
            }
            p1 = p2;
            update();//窗口重绘函数
        }
    }
    if (is_cut == true)
    {
        if (event->buttons() & Qt::LeftButton)//左键一直按下
        {
            p2 = event->pos();
            update();
        }
    }
    return QWidget::mouseMoveEvent(event);
}
void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (is_cut == true)
    {
        //line.Liang_Barsky(L)
        int xmin, xmax, ymin, ymax;
        xmin = std::min(init_1.x(), p2.x());
        xmax = std::max(init_1.x(), p2.x());
        ymin = std::min(init_1.y(), p2.y());
        ymax = std::max(init_1.y(), p2.y());
        line.Liang_Barsky(L, xmin, ymin, xmax, ymax);
        update();
        is_cut = false;
    }
    p1.rx() = 0; p1.ry() = 0;
    p2.rx() = 0; p2.ry() = 0;
}
void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (is_select == false)
        {
            m_iCount++;
            QPoint p(event->x(), event->y());//记录鼠标左键点下的点位
            //QPainter painter(this);
            //painter.setPen(QPen(Qt::black, 3));
            if (pt == &point)//绘制点
            {
                P.append(p);
                update();
                m_iCount = 0;
            }
            if (pt == &line)//绘制线
            {
                if (m_iCount == 1)
                {
                    L.push_back(QVector<QPoint>());
                    L.back().push_back(p);
                }
                else if (m_iCount == 2)
                {
                    L.back().push_back(p);
                    update();
                    m_iCount = 0;
                }
            }
            if (pt == &circle)//绘制圆
            {
                if (m_iCount == 1)
                {
                    C.push_back(QVector<QPoint>());
                    C.back().push_back(p);
                }
                else if (m_iCount == 2)
                {
                    C.back().push_back(p);
                    update();
                    m_iCount = 0;
                }
            }
            if (pt == &triangle)//绘制三角形
            {
                if (m_iCount == 1)
                {
                    T.push_back(QVector<QPoint>());
                    T.back().push_back(p);
                }
                else if (m_iCount == 2)
                {
                    T.back().push_back(p);
                }
                else if (m_iCount == 3)
                {
                    T.back().push_back(p);
                    update();
                    m_iCount = 0;
                }
            }
            if (pt == &rectangle)//绘制矩形
            {
                if (m_iCount == 1)
                {
                    R.push_back(QVector<QPoint>());
                    R.back().push_back(p);
                }
                else if (m_iCount == 2)
                {
                    QPoint p2(p.x(), R.back()[0].y());
                    QPoint p4(R.back()[0].x(), p.y());
                    R.back().push_back(p2);
                    R.back().push_back(p);
                    R.back().push_back(p4);
                    update();
                    m_iCount = 0;
                }
            }
            if (pt == &polygon)//绘制多边形
            {
                if (m_iCount == 1)
                {
                    PL.push_back(QVector<QPoint>());
                    PL.back().push_back(p);
                }
                else
                {
                    PL.back().push_back(p);
                    update();
                }
            }
            init_1 = p;
        }
        else if (is_select == true)//点击进行选中判断
        {
            pt = NULL;
            QPoint p(event->x(), event->y());
            QVector<QVector<int>> temp;
            I = temp;//清空上一次的存储数组。
            /*for (int i = 0; i < I.size(); i++)
            {
                I.pop_back();
            }*/
            for (int i = 0; i < L.size(); i++)
            {
                if (line.IsContain(p, L[i][0], L[i][1]))
                {
                    I.push_back(QVector<int>());
                    I.back().push_back(1);
                    I.back().push_back(i);
                }
            }
            for (int i = 0; i < C.size(); i++)
            {
                if (circle.IsContain(p, C[i][0], C[i][1]))
                {
                    I.push_back(QVector<int>());
                    I.back().push_back(2);
                    I.back().push_back(i);
                }
            }
            for (int i = 0; i < T.size(); i++)
            {
                if (triangle.IsContain(p, T[i][0], T[i][1], T[i][2]))
                {
                    I.push_back(QVector<int>());
                    I.back().push_back(3);
                    I.back().push_back(i);
                }
            }
            for (int i = 0; i < R.size(); i++)
            {
                if (rectangle.IsContain(p, R[i]))
                {
                    I.push_back(QVector<int>());
                    I.back().push_back(4);
                    I.back().push_back(i);
                }
            }
            for (int i = 0; i < PL.size(); i++)
            {
                if (polygon.IsContain(p, PL[i]))
                {
                    I.push_back(QVector<int>());
                    I.back().push_back(5);
                    I.back().push_back(i);
                }
            }
            choose_count = 1;//选中之后
            update();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        m_iCount = 0;
    }
   
}
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_D)
    {
        if (is_select == true && I.size() != 0)
        {
            if (I[c][0] == 1)
            {
                get_center(L[I[c][1]]);
                Rotate(L[I[c][1]], -5);
                update();
            }
            if (I[c][0] == 3)
            {
                get_center(T[I[c][1]]);
                Rotate(T[I[c][1]], -5);
                update();
            }
            if (I[c][0] == 4)
            {
                get_center(R[I[c][1]]);
                Rotate(R[I[c][1]], -5);
                update();
            }
            if (I[c][0] == 5)
            {
                get_center(PL[I[c][1]]);
                Rotate(PL[I[c][1]], -5);
                update();
            }
        }
    }
    if (event->key() == Qt::Key_A)
    {
        if (is_select == true && I.size() != 0)
        {
            if (I[c][0] == 1)
            {
                Rotate(L[I[c][1]], 5);
                update();
            }
            if (I[c][0] == 3)
            {
                Rotate(T[I[c][1]], 5);
                update();
            }
            if (I[c][0] == 4)
            {
                Rotate(R[I[c][1]], 5);
                update();
            }
            if (I[c][0] == 5)
            {
                Rotate(PL[I[c][1]], 5);
                update();
            }
        }
    }
}
void MainWindow::paintEvent(QPaintEvent*)
{
   
    //// Create a new QPainter and set it to draw on the scene
    //QPainter painter(ui->graphicsView->viewport());
    //painter.setRenderHint(QPainter::Antialiasing);
    //painter.setViewport(ui->graphicsView->viewport()->geometry());
    //painter.setWindow(ui->graphicsView->viewport()->geometry());
    //// Draw all the shapes and fill colors
    //for (int i = 0; i < P.size(); i++)
    //{
    //    painter.drawPoint(P.at(i));
    //    scene->addEllipse(P.at(i).x(), P.at(i).y(), 1, 1, *pen);
    //}
    //for (int i = 0; i < L.size(); i++)
    //{
    //    if (L[i].size() != 2)
    //    {
    //        
    //        L.pop_back();
    //        break;
    //    }
    //    if (pt == &line && L.at(i).size() == 2)
    //    {
    //        painter.drawLine(L.at(i).at(0), L.at(i).at(1));
    //    }
    //    else if (pt != &line && L.at(i).size() == 2)
    //    {
    //        line.draw_shape(L[i][0], L[i][1], *scene);
    //    }
    //}
    //for (int i = 0; i < C.size(); i++)
    //{
    //    if (C[i].size() != 2)
    //    {
    //        //C[i].clear();
    //        C.pop_back();
    //        break;
    //    }
    //    if (C.at(i).size() == 2)
    //    {
    //        circle.draw_shape(C[i][0], C[i][1], *scene);
    //    }
    //}
    //for (int i = 0; i < T.size(); i++)
    //{
    //    if (T[i].size() != 3)
    //    {
    //        
    //        T.pop_back();
    //        break;
    //    }
    //    if (T.at(i).size() == 3)
    //    {
    //        triangle.draw_shape(T[i][0], T[i][1], T[i][2], *scene);
    //    }
    //}
    //for (int i = 0; i < R.size(); i++)
    //{
    //    if (R[i].size() != 4)
    //    {
    //        R.pop_back();
    //        break;
    //    }
    //    if (R[i].size() == 4)
    //    {
    //        rectangle.draw_shape(R[i][0], R[i][1], R[i][2], R[i][3], *scene);
    //    }
    //}
    //for (int i = 0; i < PL.size(); i++)
    //{
    //    polygon.draw_shape(PL[i], *scene);
    //}
    //for (int i = 0; i < col_pl.size(); i++)
    //{
    //    switch (col_pl[i][1])
    //    {
    //    case 1:
    //    {
    //        painter.setPen(QPen(Qt::red, 3));
    //        polygon.fill(PL[col_pl[i][0]], *scene);
    //        break;
    //    }
    //    case 2:
    //    {
    //        painter.setPen(QPen(Qt::blue, 3));
    //        polygon.fill(PL[col_pl[i][0]], *scene);
    //        break;
    //    }
    //    case 3:
    //    {
    //        painter.setPen(QPen(Qt::green, 3));
    //        polygon.fill(PL[col_pl[i][0]], *scene);
    //        break;
    //    }
    //    }
    //}
    //for (int i = 0; i < col_t.size(); i++)
    //{
    //    switch (col_t[i][1])
    //    {
    //    case 1:
    //    {
    //        painter.setPen(QPen(Qt::red, 3));
    //        polygon.fill(T[col_t[i][0]], *scene);
    //        break;
    //    }
    //    case 2:
    //    {
    //        painter.setPen(QPen(Qt::blue, 3));
    //        polygon.fill(T[col_t[i][0]], *scene);
    //        break;
    //    }
    //    case 3:
    //    {
    //        painter.setPen(QPen(Qt::green, 3));
    //        polygon.fill(T[col_t[i][0]], *scene);
    //        break;
    //    }
    //    }
    //}
    //for (int i = 0; i < col_r.size(); i++)
    //{
    //    switch (col_r[i][1])
    //    {
    //    case 1:
    //    {
    //        painter.setPen(QPen(Qt::red, 3));
    //        rectangle.fill(R[col_r[i][0]], *scene);
    //        break;
    //    }
    //    case 2:
    //    {
    //        painter.setPen(QPen(Qt::blue, 3));
    //        rectangle.fill(R[col_r[i][0]], *scene);
    //        break;
    //    }
    //    case 3:
    //    {
    //        painter.setPen(QPen(Qt::green, 3));
    //        rectangle.fill(R[col_r[i][0]], *scene);
    //        break;
    //    }
    //    }
    //}
    //for (int i = 0; i < col_t.size(); i++)
    //{
    //    switch (col_t[i][1])
    //    {
    //    case 1:
    //    {
    //        painter.setPen(QPen(Qt::red, 3));
    //        polygon.fill(T[col_t[i][0]], *scene);
    //        break;
    //    }
    //    case 2:
    //    {
    //        painter.setPen(QPen(Qt::blue, 3));
    //        polygon.fill(T[col_t[i][0]], *scene);
    //        break;
    //    }
    //    case 3:
    //    {
    //        painter.setPen(QPen(Qt::green, 3));
    //        polygon.fill(T[col_t[i][0]], *scene);
    //        break;
    //    }
    //    }
    //}
    //for (int i = 0; i < col_c.size(); i++)
    //{
    //    switch (col_c[i][1])
    //    {
    //    case 1:
    //    {
    //        painter.setPen(QPen(Qt::red, 3));
    //        circle.fill(C[col_c[i][0]], *scene);
    //        break;
    //    }
    //    case 2:
    //    {
    //        painter.setPen(QPen(Qt::blue, 3));
    //        circle.fill(C[col_c[i][0]], *scene);
    //        break;
    //    }
    //    case 3:
    //    {
    //        painter.setPen(QPen(Qt::green, 3));
    //        circle.fill(C[col_c[i][0]], *scene);
    //        break;
    //    }
    //    }
    //}
    //if (is_select && choose_count == 1 && I.size() != 0)//选中图形高亮显示
    //{
    //    painter.setPen(QPen(Qt::yellow, 3));
    //    c = rand() % I.size();//随机数，几个包含当前选中点的图形随机高亮显示一个。
    //    if (I[c][0] == 1)
    //    {
    //        line.draw_shape(L[I[c][1]][0], L[I[c][1]][1], *scene);
    //    }
    //    else if (I[c][0] == 2)
    //    {
    //        circle.draw_shape(C[I[c][1]][0], C[I[c][1]][1], *scene);
    //    }
    //    else if (I[c][0] == 3)
    //    {
    //        triangle.draw_shape(T[I[c][1]][0], T[I[c][1]][1], T[I[c][1]][2], *scene);
    //    }
    //    else if (I[c][0] == 4)
    //    {
    //        rectangle.draw_shape(R[I[c][1]][0], R[I[c][1]][1], R[I[c][1]][2], R[I[c][1]][3], *scene);
    //    }
    //    else if (I[c][0] == 5)
    //    {
    //        polygon.draw_shape(PL[I[c][1]], *scene);
    //    }
    //    //}
    //    choose_count = 0;//归零。
    //}
    //if (is_cut == true && p2.x() != 0)
    //{
    //    painter.drawRect(init_1.x(), init_1.y(), p2.x() - init_1.x(), p2.y() - init_1.y());
    //}
}
void MainWindow::get_center(QVector<QPoint>& s)
{
    int k = 0, m = 0;
    for (int i = 0; i < s.size(); i++)
    {
        k += s[i].x();
        m += s[i].y();
    }
    k_avg = int(int(k / s.size() + 0.5) + 0.5);
    m_avg = int(int(m / s.size() + 0.5) + 0.5);
}
void MainWindow::Rotate(QVector<QPoint>& s, double angle)
{
    for (int i = 0; i < s.size(); i++)
    {
        int x1 = cos(angle / 180 * PI) * (s[i].x() - k_avg) + sin(angle / 180 * PI) * (s[i].y() - m_avg) + k_avg;
        int y1 = cos(angle / 180 * PI) * (s[i].y() - m_avg) + sin(angle / 180 * PI) * (k_avg - s[i].x()) + m_avg;
        s[i].setX(int(x1 + 0.5));
        s[i].setY(int(y1 + 0.5));
    }
}
void MainWindow::on_actionPolygoncut_triggered()
{
    pt = NULL;
    is_select = false;
    is_cutP = true;
}
void MainWindow::on_actionLinecut_triggered()
{
    pt = NULL;
    is_select = false;
    is_cut = true;
}
