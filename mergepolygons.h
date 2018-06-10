#ifndef MERGEPOLYGONS_H
#define MERGEPOLYGONS_H

#include <QVector>
#include <QPointF>

class MergePolygons
{
public:
    MergePolygons();

    void merge(QVector<QPointF>& wp_area , const QVector<QPair<QPointF, double> > &rt_pts );

    void merge(QVector<QPointF>& wp_area , const QPair<QPointF, double> &pos );
};

#endif // MERGEPOLYGONS_H
