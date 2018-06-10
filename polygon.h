#ifndef POLYGON_H
#define POLYGON_H

#include <math.h>

class Vector;
class UPolygon;

typedef enum NAME {
    SUBJECT = 0, CLIP = 1
} NAME;



class PointNode {
public:
    PointNode(int x, int y);


    int x;
    int y;

    bool inbound;
    bool visited;
        // links to the next point in the list - this is a dictionary based on
        // the name of the polygon currently being traced. Intersections should
        // have two pointers, normal nodes will have one.
        PointNode* next[2];

        // this tells us the t value of an intersection. Again this is a
        // dictionary as we have different t values for the two polygons
        float t[2];


    void setNext(PointNode* pn, NAME polygon);

        ///Set the next pointer to the PointNode pn. The polygon variable tells us
        ///which polygon the point belongs to.

    void setT(float t, NAME polygon);

        ///This sets the t value of this point for polygon polygon

    bool isIntersection(NAME name);


    Vector operator-(PointNode other);
    PointNode operator-(Vector other);
    PointNode operator+(Vector other);

 };



typedef struct IntersectionParameters {
 float t;
 float u;
 int inbound;//inbound is boolean in real life, but we have to use int type to pass -1 as python's None
} IntersectionParameters;

IntersectionParameters* intersection(PointNode A, PointNode B, PointNode C, PointNode D);
bool are_intersected(PointNode A, PointNode B, PointNode C, PointNode D);



class UPolygon {

public:
    UPolygon(NAME name);
    ~UPolygon();

    NAME name;
    PointNode* pointList;

    void addPoint(int x, int y);
    bool validate();
    bool contains(int x, int y);

private:
    void reversePoints();
    bool isCCW();
    bool isSimple();


    bool valid;
    int lastPoint[2];
};


class Vector {
public:
    Vector(int x, int y);
    int x, y;

    int dot(Vector v);
    Vector perp();
    float magnitude();
    void normalize();
    PointNode operator+(PointNode other);
    Vector operator+(Vector other);
    PointNode operator-(PointNode other);
    Vector operator-(Vector other);
    Vector operator*(float other);
    Vector operator/(float other);

 };


#endif // POLYGON_H
