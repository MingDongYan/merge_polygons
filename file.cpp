#include <QFile>
#include <QDataStream>
#include <QPolygon>
#include "file.h"

  void save_to_file(QPolygon* polygon, QString name)
  {

    QFile file( name );
    if( !file.open( QIODevice::WriteOnly ) )
      return;

    QDataStream stream( &file );
    stream.setVersion( QDataStream::Qt_4_2 );

    stream << *polygon;

    file.close();
  }

  bool load_from_file(QString name, QPolygon* polygon)
  {
    QFile file( name );
    if( !file.open( QIODevice::ReadOnly ) )
      return false;

    QDataStream stream( &file );
    stream.setVersion( QDataStream::Qt_4_2 );

    stream >> *polygon;

    file.close();

//    for(int i = 0; i < polygon->size(); i++)
//      qDebug() << polygon->at(i).x() << "("
//               << polygon->at(i).y() << ")";
    return true;
  }

