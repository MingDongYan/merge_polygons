#include "mainwindow.h"
#include <QApplication>

#include <QtDebug>

#include <QTimer>

#include "dbconnect.h"
#include "jzjtypemanager.h"
#include "routemapmanager.h"
#include "routesolver.h"
#include "alt.h"
#include "polygonsmanager.h"

#include "ptstoupolygon.h"
#include "clipengine.h"


#include "mergepolygons.h"
#include "area_sys.h"
#include "polygonsdis.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // my test
    /*PtsToUPolygon change;

    DbConnect db;
    db.openDb( "QSQLITE" ,  "localhost" ,  3306 , "./yyy.db" ,  "root" ,  "root"  );

    RouteMapManager::getInstance()->initMaps();

    JZJTypeManager::getInstance()->Init();

    PolygonsManager::getInstance()->init( "9a645865-d960-42bc-9d49-d17f1ec0" );

    RouteSolver solver;
    QVector<QPair<QPointF, double> > rt_pts = solver.getPointVec( RouteMapManager::getInstance()->getRt( "8802a0ce-d996-44ef-8921-ce4060df" ) );
    QVector<QPointF> area;

    QTime t;
    t.start();
    MergePolygons m;
    m.merge( area , rt_pts );

    qDebug()<<"cal use time:"<<t.elapsed();
    for( QPointF p : area )
    {
        qDebug()<<"x:"<<p.x()<<"y:"<<p.y();
    }
    */
    return a.exec();
}
