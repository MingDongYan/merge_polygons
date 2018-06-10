#include "ptstoupolygon.h"
#include <QtDebug>

#include "polygon.h"

PtsToUPolygon::PtsToUPolygon()
{

}
/* convert */
UPolygon* PtsToUPolygon::Pts2UPolygon (const QVector< QPointF >& pts , NAME nm )
{
//    UPolygon* polygon = new UPolygon(qpolygon == &father ? SUBJECT:CLIP);
    UPolygon* polygon = new UPolygon( nm );
    for ( QPointF p : pts )
    {
        polygon->addPoint( p.x(), p.y() );
    }
    return polygon;
}

/* convert */
QVector< QPointF > PtsToUPolygon::UPolygon2Pts (UPolygon* polygon)
{
    QVector< QPointF > pts;
    PointNode* pt = polygon->pointList;
    while (pt != NULL) {
        pts.append(QPoint(pt->x, pt->y));
        pt = pt->next[polygon->name];
    }
    return pts;
}
