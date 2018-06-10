/**
 * All function in file are written on the base of algorithms from
 * Weiler-Atherton Clipping Applet
 * Author: Christopher Andrews
 * http://people.cs.vt.edu/~cpa/clipping/index.html
 */

#include "clipengine.h"
#define NULL 0

/*
        This is looking for the intersection point between the line segments
        AB and CD. We are using the parametric equation A-bt = C - du, where
        b is the vector (B-A) and d is the vector (D-C). We want to solve for
        t and u - if they are both between 0 and 1, we have a valid
        intersection point.

        For insertion, we assume that AB comes from the subject polygon and
        that CD comes from the clipping region. This means that the
        intersection point can be inserted directly between CD but that we need
        to use the t value to find a valid position between A and B.
*/

static void insertIntersection(UPolygon *subject, UPolygon* clipping,
                               PointNode* A, PointNode* B, PointNode* C, PointNode* D)
{

        IntersectionParameters* param = intersection(*A,*B, *C,*D);
        if (isnan(param->t)) //the lines are parallel
            return;

        Vector b = *B - *A;
        PointNode intersection = *A+b*param->t;

        if (isnan(param->u))// we know that the point isn't within t
            return;

        PointNode *newPoint = NULL;

        // the point is on line segment AB
        if (0 < param->u && param->u < 1) {
            // we have a valid intersection point
            if (param->t == 0) {
                newPoint = A;
                newPoint->setT(param->t, subject->name);
            }
            else if (param->t==1) {
                newPoint = B;
                newPoint->setT(param->t, subject->name);
            }
            else {
                newPoint = new PointNode(intersection.x, intersection.y);
                newPoint->setT(param->t, subject->name);
                newPoint->inbound = param->inbound;

                // insert it into AB
                PointNode* prev = A;
                PointNode* next = A->next[subject->name];
                while (next != NULL && next->t[subject->name] < param->t) {
                    prev = next;
                    next = next->next[subject->name];
                }
                prev->setNext(newPoint, subject->name);
                newPoint->setNext(next, subject->name);
            }
            newPoint->setT(param->u, clipping->name);
            //insert into CD
            PointNode* prev = C;
            PointNode* next = C->next[clipping->name];
            while (next !=NULL && next->t[clipping->name] < param->u) {
                prev = next;
                next = next->next[clipping->name];
            }
            prev->setNext(newPoint, clipping->name);
            newPoint->setNext(next, clipping->name);

        }
        else if ((param->u == 0 ||  param->u == 1) &&
                 (0 < param->t  &&  param->t < 1)) {
           // we can ignore them if the point it an endpoint in both
           // polygons - any following we do will just wrap around
            if (param->u == 0)
                newPoint = C;
            else
                newPoint = D;

            newPoint->setT(param->t, subject->name);
            newPoint->setT(param->u, clipping->name);
            newPoint->inbound = param->inbound;

            // insert it into AB
            PointNode* prev = A;
            PointNode* next = A->next[subject->name];
            while (next != NULL && next->t[subject->name] < param->t) {
                prev = next;
                next = next->next[subject->name];

            }
            prev->setNext(newPoint, subject->name);
            newPoint->setNext(next, subject->name);
       }
       delete param;

}

static void findIntersections(UPolygon* subject, UPolygon* clipping)
{

        PointNode* A = subject->pointList;
        while (A != NULL) {
            PointNode* B = A->next[subject->name];
            PointNode* tmp = B;
            if (B == NULL) B = subject->pointList;
            PointNode* C =  clipping->pointList;
            while (C != NULL) {
               PointNode* D = C->next[clipping->name];
                while (D != NULL && D->isIntersection(clipping->name))
                    //skip past intersections
                    D = D->next[clipping->name];
                if (D != NULL) insertIntersection(subject, clipping,A,B,C,D);
                else insertIntersection(subject,clipping,A,B,C,clipping->pointList); // loopback on clipping
                C = D;
            }
            A = tmp;
         }
}

static PointNode* findEnteringIntersection(PointNode* plist, NAME name)
{
        if (plist == NULL)
            return NULL;
        PointNode* current = plist;

        while ((current != NULL) && !(current->inbound && !current->visited)) {
            //if current.inbound: point  has already been visited, so advance to the next point
            // else Point  is not an inbound intersection, so advance to the next point
            current = current->next[name];
        }
        return current;
}

   ///   The main clipping function
std::vector<UPolygon*> clip(UPolygon* p0, UPolygon* p1)
{
//Step one is to identify all of the intersection points between the two UPolygon;
findIntersections(p1,p0);

std::vector<UPolygon*> polygons;// = new std::vector<UPolygon*>();
      NAME names[2] = {p0->name, p1->name};

        // get next entering intersection
        PointNode* entering = findEnteringIntersection(p1->pointList, p1->name);

         while (entering != NULL) {

            UPolygon* newPoly = new UPolygon(CLIP);

            PointNode* current = entering;
            int currentPoly = 1;
            while (false == current->visited) {

                newPoly->addPoint(current->x, current->y);

                current->visited = true;


                if (current->isIntersection(names[currentPoly]) && current != entering)
                    currentPoly = (currentPoly + 1) % 2;


                current = current->next[names[currentPoly]];
                if (current == NULL) {
                    // time to wrap
                    if (currentPoly == 0)
                        current = p0->pointList;
                    else
                        current = p1->pointList;
              }
            }

            polygons.push_back(newPoly);
              entering = findEnteringIntersection(current->next[p1->name], p1->name);
      }



        if (polygons.size() == 0) {
            /*
            # we  found no polygons - possibly one is contained in the other
            # first check to see if our clipping region contains the other one
            # theoretically, any point will do, except that all of the end
            # points might intersect the boundary of the shape - and thus fail
            # the containment test. So, we pick a point that is halfway along
            # the first edge and one unit vector in the direction opposite the
            # perp vector
          */
            PointNode* A = p1->pointList;
            PointNode* B = p1->pointList->next[p1->name];
            Vector b = *B - *A;
            Vector b_perp = b.perp();
            b_perp.normalize();
            int x = .5 * (A->x + B->x) - b_perp.x;
            int y = .5 * (A->y + B->y) - b_perp.y;

            if (p0->contains(x,y)) {
                // make a copy of poly0
                UPolygon* newPoly = new UPolygon(SUBJECT);
                PointNode* pt = p0->pointList;
                while(pt != NULL) {
                    newPoly->addPoint(pt->x,pt->y);
                    pt = pt->next[p0->name];
                }
                polygons.push_back(newPoly);
            }
           // now check to see if the clipping region is enclosed by the other
            else {
                PointNode* A = p0->pointList;
                PointNode* B = p0->pointList->next[p0->name];
                Vector b = *B - *A;
                Vector b_perp = b.perp();
                b_perp.normalize();
                x = .5 * (A->x + B->x) - b_perp.x;
                y = .5 * (A->y + B->y) - b_perp.y;

                if (p1->contains(x,y)) {
                   // make a copy of poly0
                    UPolygon* newPoly = new UPolygon(SUBJECT);
                    PointNode* pt = p1->pointList;
                    while(pt != NULL) {
                        newPoly->addPoint(pt->x,pt->y);
                        pt = pt->next[p1->name];
                    }
                    polygons.push_back(newPoly);
                }
                else //return two base polygons
                {
                    polygons.push_back(p0);
                    polygons.push_back(p1);
                }


            }

        }
return polygons;
}

