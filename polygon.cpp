/**
 * All function in file are written on the base of algorithms from
 * Weiler-Atherton Clipping Applet
 * Author: Christopher Andrews
 * http://people.cs.vt.edu/~cpa/clipping/index.html
 */

#include "polygon.h"
#include <stdio.h>


/*
        This calculates the intersection point between two lines, AB and CD.
        The function is somewhat specialized for the purposes of the clipping
        visualization. The return value is NULL if the lines are parallel or a
        tuple of the form (t, u, inbound). inbound is a boolean value if
        we know that CD is a line that belongs to a polygon specified in CCW
        direction. The further optimization is that u and inbound are set to
        None if t is not within 0 and 1. All of the uses of this function
        in this application are only interested in line segments and this saves
        a couple of extra calculations.
*/
IntersectionParameters* intersection(PointNode A, PointNode B, PointNode C, PointNode D)
{
    IntersectionParameters* param = new IntersectionParameters;
    param->t = NAN;
    param->u = NAN;
    param->inbound = -1;

        Vector b = B - A;
        Vector d_perp = (D-C).perp();
        int denom = d_perp.dot(b);
        if (denom != 0) {
            Vector c = C - A;
            int numer =  d_perp.dot(c);
            float t = (float)numer / (float)denom;
            if (0 <= t && t <= 1) {

                Vector b_perp = b.perp();
                int numer = b_perp.dot(c);
                float u = (float)numer / (float)denom;
                param->t = t;
                param->u = u;
                param->inbound = denom < 0;
            }
            else {
                // the intersetion isn't within segment AB, so bail out
                param->t = t;
                return param;
            }
        }
return param;
}


bool are_intersected(PointNode A, PointNode B, PointNode C, PointNode D)
{
    bool result = false;
    IntersectionParameters* param = intersection(A,B, C,D);
    if (!isnan(param->t) && !isnan(param->u)) {
       // not parallel and within segment AB
       if((0 <= param->t && param->t <= 1) &&
          (0 <= param->u && param->u <= 1))
         result = true;
    }
delete param;
return result;
}

UPolygon::UPolygon (NAME name) {
  this->valid = false;
  this->name = name;
  this->pointList = NULL;
  this->lastPoint[0] = -1;//None
}

UPolygon::~UPolygon () {
/*
  PointNode* pt = this->pointList;
  while (pt != NULL)
  {

     PointNode* temp = pt->next[this->name];
     delete pt;
     pt = temp;
  }
  this->pointList = NULL;
*/
}
void UPolygon::addPoint(int x, int y) {

  if ((this->lastPoint[0] != -1) &&
     (x == this->lastPoint[0])  &&
     (y == this->lastPoint[1]))
     return;

  this->lastPoint[0] = x;
  this->lastPoint[1] = y;

  PointNode* p = new PointNode(x,y);


  p->setT(1.0, this->name);
  p->setNext(NULL, this->name);

  if (this->pointList == NULL) {
    this->valid = true;
    this->pointList = p;
  }
  else {
    PointNode* plist = this->pointList;
    while (plist->next[this->name] != NULL)
        plist = plist->next[this->name];
    plist->setNext(p, this->name);
  }
}



bool UPolygon::contains(int x, int y) {
    bool yes = false;

   PointNode* A = this->pointList;
    while (A != NULL) {
        PointNode* B = A->next[this->name];
        if (B == NULL) B = this->pointList;
        if(are_intersected(*A, *B, PointNode(x, y), PointNode( 0, 10000)))
            yes = !yes;
        A = A->next[this->name];
     }
return yes;
}

void UPolygon::reversePoints() {
            PointNode* prev = NULL;
            PointNode* current = this->pointList;

            while (current != NULL) {
                PointNode* next = current->next[this->name];
                current->next[this->name] = prev;
                prev = current;
                current = next;
            }
            this->pointList = prev;
}

bool UPolygon::validate() {

          ///  This makes sure the polygon is simple, reverses it if necessary
          ///  and applies labels based on the string names.


            if (!isSimple())
                return false;

           /// reverse if necessary
            if (isCCW())
                reversePoints();

            return true;
}

/*
            This function determines if a polygon is specified in clockwise (CW) or
            counterclockwise (CCW) direction, returning True if it is CCW and False
            otherwise.

            The test is performed by performing a cross product of the line
            segments BC x BA, where A,B and C are three sequential points in the
            polygon. As all points lie on the XY-plane, we can ignore
            the X and Y components of the resultant vector (they will be zero). We
            only care about the sign of the Z component - positive means CCW,
            while negative means CW [recall that the Y-axis is upside down].

            Unfortunately, this technique only works if the angle is convex. To
            handle this, we first find the leftmost point (any -most point would do,
            we just need an extreme point to guarantee that it is the apex of a
            convex angle). This will be the middle point.
*/
bool UPolygon::isCCW() {

   PointNode* pt = pointList;
   PointNode* A = NULL;
   PointNode* B = pt;//min index
   PointNode* C = NULL;
   //search point with min x
    while (pt != NULL) {
        if (B->x < pt->x)
            B = pt;
        pt = pt->next[this->name];
    }
    //next
    C = B->next[this->name];
    if (C == NULL) C = pointList;

    //search previous
    A = this->pointList;
    while (A->next[this->name] != NULL)
    {
      if (A->next[this->name]  == B) break;
      else
       A = A->next[this->name];
    }

   int z = ((A->x - B->x)*(C->y - B->y)) - ((A->y - B->y)*(C->x - B->x));

    return (z > 0);
}

//This function determines if the polygon is a simple one.
bool UPolygon::isSimple() {



  PointNode* A = this->pointList;
  PointNode* B = A->next[this->name];
  PointNode* C = B->next[this->name];
  while (C != NULL) {
    while (C != NULL) {

       PointNode* D = C->next[this->name];
       if (D == NULL) D = this->pointList;

       if (A != D) {
          IntersectionParameters* param = intersection(*A,*B, *C,*D);
          if (!isnan(param->t) && !isnan(param->u)) {
          // not parallel and within segment AB
              if((0 <= param->t && param->t <= 1) &&
                 (0 <= param->u && param->u <= 1))
                {
                   delete param;
                   return false;
                }
           }
        }
       C = C->next[this->name];
     }
     A = B;
     B = B->next[this->name];
     if (B != NULL)
        C = B->next[this->name];
  }
 return true;
}




Vector::Vector(int x, int y)
{
  this->x = x;
  this->y = y;
}

int Vector::dot(Vector v) {
  return this->x*v.x + this->y*v.y;
}
Vector Vector:: perp() {
  return Vector(-this->y, this->x);
}
float Vector::magnitude() {
  return sqrt(this->x*this->x + this->y*this->y);
}
void Vector::normalize() {
  float magnitude = this->magnitude();
  this->x /= magnitude;
  this->y /= magnitude;
}
PointNode Vector::operator+(PointNode other) {
  return PointNode(this->x + other.x, this->y + other.y);
}
Vector Vector::operator+(Vector other) {
  return Vector(this->x + other.x, this->y + other.y);
}


PointNode Vector::operator-(PointNode other) {
  return PointNode(this->x - other.x, this->y - other.y);
}
Vector Vector::operator-(Vector other) {
  return Vector(this->x - other.x, this->y - other.y);
}

Vector Vector::operator*(float other) {
  return Vector(this->x * other, this->y * other);
}

Vector Vector::operator/(float other) {
  return Vector(this->x / other, this->y / other);
}



PointNode::PointNode (int x, int y) {
  this->x = x;
  this->y = y;
  this->next[SUBJECT] = NULL;//
  this->next[CLIP] = NULL;//

  this->t[SUBJECT] = NAN;//
  this->t[CLIP] = NAN;//

  this->inbound = false;
  this->visited = false;
}

void PointNode::setNext(PointNode* pn, NAME polygon) {
  this->next[polygon] = pn;
}

void PointNode::setT(float t, NAME polygon) {
  this->t[polygon] = t;
}

bool PointNode::isIntersection(NAME name) {
//if (name==NONE)
//  return (next[0] != NULL || next[1] != NULL);
//else
  if (!isnan(this->t[name]))
    return (this->t[name] < 1);
  else
   return false;
}

Vector PointNode::operator-(PointNode other) {
  return Vector(this->x - other.x, this->y - other.y);
}

PointNode PointNode::operator-(Vector other) {
  return PointNode(this->x - other.x, this->y - other.y);
}

PointNode PointNode::operator+(Vector other) {
  return PointNode(this->x + other.x, this->y + other.y);
}
