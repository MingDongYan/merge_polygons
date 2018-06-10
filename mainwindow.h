#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class UPolygon;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
     void contextMenuEvent(QContextMenuEvent *event);
     void paintEvent(QPaintEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);
     void keyReleaseEvent(QKeyEvent *event);

private slots:
     void newPolygon();
     void openFilePolygon();
     void saveFilePolygon();
     void openDbPolygon();
     void saveDbPolygon();
     void print();
     void closePolygon();
     void removePoint();
     void removePolygon();
     void calculateChildren();


private:
     void createActions();
     void draw_polygon(QPolygon* polygon);
     inline QPolygon* getVacancy ();

     UPolygon *QPolygon2UPolygon(QPolygon* qpolygon);
     QPolygon UPolygon2QPolygon(UPolygon* polygon);

     bool point_close_to_point(QPoint A, QPoint B);
     bool point_close_to_line(QPoint M, QPoint A, QPoint B);
     bool polygon_contains_point (QPolygon* polygon, int x, int y);

     QPoint cursor;

     QPolygon  father, mother;
     QList<QPolygon> children;
     QPolygon *active;//pointer to father, mother or one of the children

     //---State of active polygon:---//
     bool edit_mode;
     int cPoint;// number of selected point(default -1)
     int cLine; // number of selected line (default -1)
     bool self_intersected;//in edit_mode: self_intersected = true - when potential line from cursor to last added point
                           //intersects one of the polygon's sides
     bool aim_visible;
     //______________________________//

     QAction *newAct;
     QAction *openFileAct;
     QAction *saveFileAct;
     QAction *openDbAct;
     QAction *saveDbAct;
     QAction *printAct;
     QAction *exitAct;
     QAction *closeAct;
     QAction *removePointAct;
     QAction *removePolygonAct;
     QAction *calculateChildrenAct;
};

#endif // MAINWINDOW_H
