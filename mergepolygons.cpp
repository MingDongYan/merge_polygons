#include "mergepolygons.h"
#include <QVector>
#include <QPointF>
#include "./alt.h"

#include "./area_sys.h"
#include "./polygonsmanager.h"

#include "ptstoupolygon.h"
#include "clipengine.h"

MergePolygons::MergePolygons()
{

}

void MergePolygons::merge(QVector<QPointF> &wp_area,const QVector<QPair<QPointF, double> >& rt_pts )
{
    if( rt_pts.isEmpty() ) return ;

    wp_area = alt::getRealCurPts( PolygonsManager::getInstance()->getPoints( area_sys::area_type::wp ) , rt_pts.first() ,0.0,0.0,0.0);

    if( rt_pts.size() == 1 ) return;

    for( auto i_rt_pt = ( rt_pts.begin() + 1 ) ; i_rt_pt != rt_pts.end() ; i_rt_pt++ )
    {
        this->merge( wp_area , (*i_rt_pt) );
    }
}

void MergePolygons::merge(QVector<QPointF> &wp_area, const QPair<QPointF, double> &pos)
{
    QVector<QPointF> real_wp_box = alt::getRealCurPts( PolygonsManager::getInstance()->getPoints( area_sys::area_type::wp ) , pos ,0.0,0.0,0.0);

    PtsToUPolygon change;
    std::vector<UPolygon*> ch = clip( change.Pts2UPolygon( wp_area , NAME::SUBJECT ) , change.Pts2UPolygon( real_wp_box , NAME::CLIP ) );

    QVector<QPointF> t_pts;
    for (uint i = 0; i<ch.size(); i++)
    {
        UPolygon* polygon = ch.at(i);
        t_pts = t_pts + change.UPolygon2Pts( polygon  );
    }

    wp_area = t_pts;
}
