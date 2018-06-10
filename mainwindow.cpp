#include "mainwindow.h"
#include "polygon.h"
#include "file.h"
#include "database.h"
#include "clipengine.h"
#include <QPrinter>
#include <QToolTip>
#include <QMessageBox>
#include <QMenu>
#include <QPrintDialog>
#include <QFileDialog>
#include <QtGui>

/*
 *  getVacancy:
 * returns empty space for new polygon or NULL if there is not free space
 * it also can be used as indicator when we make items in context menu sensitive/unsensitive
 */
inline QPolygon* MainWindow::getVacancy ()
{
    if (father.isEmpty()) return &father;
    else if (mother.isEmpty()) return &mother;
    else return NULL;
}

/* used to detect selected vertex of polygon */
bool MainWindow::point_close_to_point(QPoint A, QPoint B)
{
 if((abs(A.x() - B.x()) < 15) &&
    (abs(A.y() - B.y()) < 15))
  return true;
return false;
}
/* used to detect selected line of polygon */
bool MainWindow::point_close_to_line(QPoint M, QPoint A, QPoint B)
{
    int max_line =  pow(A.x() - B.x(),2) +  pow(A.y() - B.y(),2);
    int min_line1 = pow(A.x() - M.x(),2) +  pow(A.y() - M.y(),2);
    int min_line2 = pow(B.x() - M.x(),2) +  pow(B.y() - M.y(),2);

    if(sqrt(min_line1) +sqrt(min_line2) - sqrt(max_line) < 2)
    return true;
return false;
}
/* used to find clicked polygon */
bool MainWindow::polygon_contains_point(QPolygon* polygon, int x, int y)
{

       bool yes = false;
       int size = polygon->size();
       for (int i = 0; i < size; i++) {
            QPoint A = polygon->at(i);
            QPoint B = polygon->at((i+1)%size);

            if(are_intersected(PointNode(A.x(), A.y()), PointNode(B.x(), B.y()),
                               PointNode(x, y), PointNode( 0, 10000)))
                yes = !yes;
       }
    return yes;
}
/* convert */
UPolygon* MainWindow::QPolygon2UPolygon (QPolygon* qpolygon)
{
    UPolygon* polygon = new UPolygon(qpolygon == &father ? SUBJECT:CLIP);
    for (int i = 0; i < qpolygon->size(); i++)
        polygon->addPoint(qpolygon->at(i).x(), qpolygon->at(i).y());
    return polygon;
}

/* convert */
QPolygon MainWindow::UPolygon2QPolygon (UPolygon* polygon)
{
    QPolygon qpolygon;
    PointNode* pt = polygon->pointList;
    while (pt != NULL) {
        qpolygon.append(QPoint(pt->x, pt->y));
        pt = pt->next[polygon->name];
    }
    return qpolygon;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
         active = NULL;
         cLine = -1;
         cPoint = -1;
         self_intersected = false;
         edit_mode = false;
         setMouseTracking(true);
         createActions();

         setWindowTitle(tr("Объединение полигонов"));
         setMinimumSize(150, 150);
         resize(500, 500);
}

MainWindow::~MainWindow()
{

}
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QMenu openMenu("Загрузить полигон");
    QMenu saveMenu("Сохранить полигон");


    menu.addAction(newAct);
    menu.addMenu(&openMenu);
    menu.addMenu(&saveMenu);

    openMenu.addAction(openFileAct);
    openMenu.addAction(openDbAct);

    saveMenu.addAction(saveFileAct);
    saveMenu.addAction(saveDbAct);


    newAct->setEnabled (getVacancy() && !edit_mode);
    openMenu.setEnabled(getVacancy() && !edit_mode);
    saveMenu.setEnabled(active && !edit_mode);


    if (active) menu.addAction(removePolygonAct);

    if (edit_mode) menu.addAction(closeAct);

    if (active) {
        menu.addAction(removePointAct);
        removePointAct->setEnabled(cPoint >= 0);
    }

    menu.addSeparator();
    menu.addAction(calculateChildrenAct);
    calculateChildrenAct->setEnabled(!getVacancy() && !edit_mode);

    menu.addSeparator();
    menu.addAction(printAct);


    menu.exec(event->globalPos());
}

void MainWindow::newPolygon()
{
    if (active && edit_mode) return;
    active = getVacancy();
    if (!active) return;

    edit_mode = true;
    update();
}

void MainWindow::openFilePolygon()
{
    active = getVacancy();
    if (!active) return;

    QString filename = QFileDialog::getOpenFileName(
            this,
            tr("Открыть файл"),
            QDir::currentPath(),
            tr("Document files (*.dat)") );
    if( !filename.isNull() )
    {
        filename.toLatin1();
       if( load_from_file(filename, active))
         update();
       else goto error;
    aim_visible = false;
     return;
    }
error:
   QToolTip::showText(this->mapToGlobal(QPoint(0,0)), "Ошибка открытия файла");
}

void MainWindow::saveFilePolygon()
{
    if (!active) return;

    QString filename = QFileDialog::getSaveFileName(
        this,
        tr("Сохранить в файл"),
        QDir::currentPath(),
        tr("Documents (*.dat)") );
    if( !filename.isNull() )
    {
       filename.toLatin1();
      save_to_file(active, filename);
    }
}


void MainWindow::openDbPolygon()
{
    active = getVacancy();
    if (!active) return;

    DbWidget d;
    d.readPolygon(active);
    aim_visible = false;
}

void MainWindow::saveDbPolygon()
{
  if (!active) return;

  DbWidget d;
  d.writePolygon(active);
}


void MainWindow::print()
{
    QPrinter printer ;
    printer.setOutputFormat(QPrinter::PdfFormat);

    QString path(QDir::currentPath());
    path.append(QDir::separator()).append("polygons.pdf");
    path = QDir::toNativeSeparators(path);

    printer.setOutputFileName(path);
    QPrintDialog* dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Печать"));

         if (dialog->exec() != QDialog::Accepted)
             return;

    QPainter painter(this);
    painter.begin(&printer);
    double xscale = printer.pageRect().width()/double(this->width());
    double yscale = printer.pageRect().height()/double(this->height());
    double scale = qMin(xscale, yscale);
    painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                      printer.paperRect().y() + printer.pageRect().height()/2);
    painter.scale(scale, scale);
    painter.translate(-width()/2, -height()/2);

    this->render(&painter);

    delete dialog;
}
/* замкнуть полигон, не закрыть */
void MainWindow::closePolygon()
{
    if(!active) return;

    int size = active->size();
    if (size < 2) {
        active->clear();
        active = NULL;
    }
    else {

    QPoint begin = active->point((cLine-1)%size);
    QPoint end = active->point(cLine%size);


    for(int i = 0; i < size; i++)
    {
        QPoint A = active->point(i);
        QPoint B = active->point((i+1)%size);
        if (are_intersected(PointNode(begin.x(),begin.y()), PointNode(end.x(), end.y()),
                            PointNode(A.x(), A.y()), PointNode(B.x(), B.y()))
            && begin != B && end != A)//fixme
        {
            QMessageBox msgBox;
            msgBox.setText("Замыкающая линия пересекает сторону полигона");
            msgBox.exec();
            return;
        }
    }
    }

    edit_mode = false;
    aim_visible = false;
    cLine = -1;

    update();
}

void MainWindow::removePoint()
{

   if (active == NULL ||
       cPoint < 0     ||
       cPoint >= active->size())
       return;

   active->remove(cPoint);
   cPoint--;
   if (cPoint < 0) cPoint = active->size() - cPoint;

   if (active->size() < 2) removePolygon();

   update();

}
void MainWindow::removePolygon()
{

   if (active == NULL)  return;

   active->clear();
   active = NULL;
   edit_mode = false;
   aim_visible = false;
   cLine = -1;
   cPoint = -1;
   update();


}



void MainWindow::calculateChildren()
{

   if (father.size() < 2 || mother.size() < 2)
       return;

   UPolygon* f = QPolygon2UPolygon(&father);
   UPolygon* m = QPolygon2UPolygon(&mother);

    if (!f->validate())  {
        QMessageBox msgBox;
        msgBox.setText("Синий полигон имеет самопересечения");
        msgBox.exec();
        return;
    }

    if (!m->validate())  {
        QMessageBox msgBox;
        msgBox.setText("Зеленый полигон имеет самопересечения");
        msgBox.exec();
        return;
    }
    children.clear();
    active = NULL;
     std::vector<UPolygon*> ch = clip(f, m);


     for (uint i = 0; i<ch.size(); i++)
     {
         UPolygon* polygon = ch.at(i);
         QPolygon qpolygon = UPolygon2QPolygon(polygon);
         children.append(qpolygon);
     }

     delete f; delete m;
   // ch->clear();

 update();

}

void MainWindow::createActions()
{
    newAct = new QAction(tr("newAct"), this);
    newAct->setShortcuts(QKeySequence::New);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newPolygon()));

    openFileAct = new QAction(tr("openFileAct"), this);
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFilePolygon()));

    saveFileAct = new QAction(tr("в файл"), this);
    connect(saveFileAct, SIGNAL(triggered()), this, SLOT(saveFilePolygon()));

    openDbAct = new QAction(tr("openDbAct"), this);
    connect(openDbAct, SIGNAL(triggered()), this, SLOT(openDbPolygon()));

    saveDbAct = new QAction(tr("saveDbAct"), this);
    connect(saveDbAct, SIGNAL(triggered()), this, SLOT(saveDbPolygon()));

    printAct = new QAction(tr("printAct"), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setStatusTip(tr("Print the document"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    closeAct = new QAction(tr("closeAct"), this);
    closeAct->setShortcuts(QKeySequence::Close);
    closeAct->setStatusTip(tr("Undo the last operation"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closePolygon()));

    removePointAct = new QAction(tr("removePointAct"), this);
    removePointAct->setShortcuts(QKeySequence::Delete);
    connect(removePointAct, SIGNAL(triggered()), this, SLOT(removePoint()));


    removePolygonAct = new QAction(tr("removePolygonAct"), this);
    connect(removePolygonAct, SIGNAL(triggered()), this, SLOT(removePolygon()));

    calculateChildrenAct = new QAction(tr("calculateChildrenAct"), this);
    connect(calculateChildrenAct, SIGNAL(triggered()), this, SLOT(calculateChildren()));

}


void MainWindow::draw_polygon(QPolygon* polygon)
{
   if (polygon == NULL) return;

   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   QColor color;
   int width = 1;
   QPen pen;

   if (polygon == &father) color = Qt::blue;
   else if (polygon == &mother) color = Qt::green;
   else {//children
       color = Qt::black;
       width =3;

   }

   pen.setWidth(width);
   pen.setColor(color);
   painter.setPen(pen);


    //draw all passive polygons and active polygon(if it is not in editing mode) as filled
   if (polygon != active || (polygon == active && !edit_mode)) {
        //now we understood that drawing children as filled is bad idea
     if(polygon == &father || polygon == &mother)
     {
       color.setAlpha(50);
       painter.setBrush(QBrush(color, Qt::SolidPattern));
     }
       painter.drawPolygon(*polygon);
       color.setAlpha(255);
   }


   int size = polygon->size();

   if (polygon == active)
   {
       if (aim_visible) painter.drawRect(cursor.x() - 5, cursor.y() - 5, 10, 10);

       //draw vertices for active polygon
       for(int i = 0; i < size; i++)
       {
          int a = 3;
          if (i == cPoint) a = 5;
          painter.drawRect(polygon->at(i).x() - a, polygon->at(i).y() - a, a*2, a*2);
          painter.drawText(polygon->at(i).x() - 5, polygon->at(i).y() - 5, QString::number(i)+"("+QString::number(active->point(i).x())+","+QString::number(active->point(i).y())+")" );
       }

    if (cLine >0)
    {
       if (edit_mode)// draw all lines except selected line and then draw two additional lines(instead of selected)  through cursor position
       {


           //control intersection and draw intersected sides in red color
           self_intersected = false;
           for(int i = 0; i < size; i++)
           {
               if (i == cLine -1) continue;

               QPoint A = active->point(i);
               QPoint B = active->point((i+1)%size);
               QPoint last = active->point((cLine-1)%size);

               if (B != last && // last != A && last != B
                   are_intersected(PointNode(cursor.x(), cursor.y()),
                                   PointNode(last.x(), last.y()),
                                   PointNode(A.x(), A.y()),
                                   PointNode(B.x(), B.y())))
               {
                  pen.setColor(Qt::red);
                  self_intersected = true;
               }
               else pen.setColor(color);

               painter.setPen(pen);
               painter.drawLine(A, B);

           }


           //draw line from cursor to the end(stipple line)
           pen.setColor(color);
           pen.setStyle(Qt::DashLine);
           painter.setPen(pen);
          // printf("%d\n", cLine);
           painter.drawLine(cursor, active->point((cLine)%size));


           //draw line from last_inserted to cursor(usual line)
           if (self_intersected) pen.setColor(Qt::red);
           pen.setStyle(Qt::SolidLine);
           painter.setPen(pen);
           painter.drawLine(cursor, active->point((cLine-1)%size));
       }
       else   //draw selected line(bold)
       {
           pen.setWidth(width*2);
           painter.setPen(pen);
           painter.drawLine(polygon->at((cLine)%size), polygon->at((cLine-1)%size));
       }
      }
    }


}

/*
 * draw polygons
 * active polygon shold be above others, so we draw in the end
 */
void MainWindow::paintEvent(QPaintEvent *event)
{
    if (&father != active) draw_polygon(&father);

    if (&mother != active) draw_polygon(&mother);

    for(int i = 0; i < children.size(); i++)
        if (&children[i] != active) draw_polygon(&children[i]);

     draw_polygon(active);
}

/*
 * mouseMoveEvent:
 * Used:
 * - for save cursor position
 * - for select side or vertex of the active polygon
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    cursor.setX(event->x());
    cursor.setY(event->y());

    if (active && !edit_mode)
    {
            cLine = -1;
            cPoint = -1;

            int size = active->size();
            if (size <= 0) return;

            for(int i = 0; i < size; i++)
            {
               if (point_close_to_point(cursor, active->point(i))) {
                   cPoint = i;
                   break;
               }
            }

            if (cPoint < 0)
            for(int i = 0; i < size; i++)
            {
                if (point_close_to_line(cursor, active->point(i), active->point((i+1)%size))) {
                    cLine = i+1;
                    break;
                }
            }
     }
     else if (edit_mode) aim_visible = true;

update();
}
/*
* mouseReleaseEvent:
* Used:
* - to enter an  editing mode for selected(active) polygon(click the line);
* - for addition new virtices to the active polygon in editing mode;
* - for select active polygon
*/
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
  if (active && edit_mode) {
      QPoint pt = QPoint(event->x(),event->y());
      if (!self_intersected) {
       if (cLine < 0) cLine = 0;
       active->insert(cLine, pt);
       cLine++;
       aim_visible = false;

      }
      else {
          QToolTip::showText( this->mapToGlobal(cursor), "Сюда нельзя: пересекающиеся стороны" );
      }
  }
  else if (cLine >= 0) {
      //cLine++;
      edit_mode = true;
  }
  else {
       active = NULL;


       for(int i = 0; i < children.size(); i++)
       {
          if(polygon_contains_point(&children[i], event->x(), event->y())) {
             active = &children[i];
             goto update;
          }
       }
        if(polygon_contains_point(&mother, event->x(), event->y()))
               active= &mother;
        else
        if(polygon_contains_point(&father, event->x(), event->y()))
               active= &father;

    }
 update:
 update();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{

   Qt::KeyboardModifiers modifiers = event->modifiers();
   if (modifiers & Qt::ControlModifier) {
       switch (event->key()) {
       case Qt::Key_N:
           newPolygon();
           break;
       case Qt::Key_W:
           closePolygon();
           break;
       case Qt::Key_P:
           print();
           break;
       default: break;

       }

   }
   switch (event->key()) {
   case Qt::Key_Delete:
       removePoint();
       break;
   case Qt::Key_Enter:
       closePolygon();
       break;

   }
update();
}
