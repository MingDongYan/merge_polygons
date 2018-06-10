#ifndef PTSTOUPOLYGON_H
#define PTSTOUPOLYGON_H

#include <QVector>
#include <QPointF>
#include "polygon.h"
class UPolygon;
class PtsToUPolygon
{
public:
    PtsToUPolygon();
    UPolygon *Pts2UPolygon(const QVector<QPointF> &pts, NAME nm);
    QVector<QPointF> UPolygon2Pts(UPolygon *polygon);
};

#endif // PTSTOUPOLYGON_H
