/*
    uses the sweep line algorithm of Fortune to determine the voronoi diagram


    * sites / places

               A y              |
               |   *            | direction of
          *    |                | the
    --------------------->x     | sweep line
               |  *             |
             * |               \|/

*/



#include "Voronoi.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <cmath>

using namespace vor;

Voronoi::Voronoi()
{
  edges = 0;
}

void Voronoi::calculate(Vertices* v, Edges* edg,  double w,  double h)
{
	places = v;
	width = w;
	height = h;
	root = 0;

  edges = edg;

  //clean up for next round
  for(Vertices::iterator i = points.begin(); i != points.end(); ++i) delete (*i);
	for(Edges::iterator i = edges->begin(); i != edges->end(); ++i) delete (*i);
	points.clear();
  edges->clear();


  std::set<Vector2d ,CompareSites> sites;
  std::pair<std::set<Vector2d,CompareSites>::iterator,bool> iter_sites;

  //remove multiply occurring points
  Vertices::iterator i = places->begin();
  while(i!=places->end())
  {
    iter_sites = sites.insert(*(*i));

    // if *(*i) exists delete it
    if(!iter_sites.second)
    {
      delete (*i);
      i = places->erase(i);
    }
    else
    {
      ++i;
    }
  }//end while


  // feed the queue using the y values
	for(Vertices::iterator i = places->begin(); i!=places->end(); ++i)
	{
		queue.push(new VEvent( *i, true));
	}
    // now are only site/place events inside the queue, site events will add a parabola to the beach line

	VEvent * e;
	while(!queue.empty())
	{
        //get first event from the queue
		e = queue.top();
		queue.pop();

        //ly is the current y coordinate of the sweep line
		ly = e->point->y;

        //deleted is the set of false or deleted events
        //deleted is needed because events are not deleted from the queue, e.g. watch (**) in InsertParabola
		if(deleted.find(e) != deleted.end()) { delete(e); deleted.erase(e); continue;}

        // if a place/site event a new parabola is added
		if(e->pe) InsertParabola(e->point);
        // else a parabola has to be removed
		else RemoveParabola(e);
		delete(e);
	}
	
  FinishEdge(root);

  for(Edges::iterator i = edges->begin(); i != edges->end(); ++i)
	{
		if( (*i)->neighbour) 
		{
      (*i)->start = (*i)->neighbour->end;
			delete (*i)->neighbour;
		}
  }
}

void Voronoi::InsertParabola(Vector2d* p)
{
  bool mustHaveCircleEvent=false;

  //if no parabola exists, build the first - the root
  //the parabolas are managed in a binary tree. Each parabola has a reference to its parent(public) and its kids(private)
	if(!root){root = new VParabola(p); return;}

  // it is true for the second site event if they are realy close
  if(root->isLeaf && root->site->y - p->y < 1) // Google: The case degenerate - sacrifice the lower points in the same high (original: degenerovan ppad - ob spodn msta ve stejn vce)
	{
    // TODO: nach x Koordinate pruefen und entsprechend in den Baum schmeien
    Vector2d* fp = root->site;
		root->isLeaf = false;
    if(fp->x < p->x){
      root->SetLeft( new VParabola(fp) );
      root->SetRight(new VParabola(p)  );
    } else {
      root->SetLeft( new VParabola(p) );
      root->SetRight(new VParabola(fp)  );
    }
    Vector2d* s = new Vector2d((p->x + fp->x)/2, height); //Google: The beginning of the middle edge points (original zatek hrany uprosted mst)
		points.push_back(s);
		if(p->x > fp->x) root->edge = new VEdge(s, fp, p); // rozhodnu, kter vlevo, kter vpravo
		else root->edge = new VEdge(s, p, fp);
		edges->push_back(root->edge);
		return;
	}

  //now the parabola under the new site is needed
	VParabola* par = GetParabolaByX(p->x);
	
  // (**) if par has a circle event (parabola will be remove while the circle event)
  // the circle event has to be remove from the priority queue because par will be "replaced"
  // but the left and right parabola of the new sequenz must have a circle event
  if(par->cEvent)
  {
    deleted.insert(par->cEvent);
    par->cEvent = 0;
    //mustHaveCircleEvent=true;
  }
	
  // remember par is the parabola under the new site p
  // start is the new beginning of a new edge of the voronoi diagram
  // GetY returns the y-coordinate of the intersection of par with the
  // straight line which is parallel to the y-axis and goes through the
  // x-coordinate of point p
  Vector2d* start = new Vector2d(p->x, GetY(*(par->site), p->x));

  // start is added to the list of new points which are determined during the algorithm
	points.push_back(start);

  // 2 new half edges => will be later combined to one edge
  // different "directions":
  // el: the part of the edge which goes to the left
  // er: the part of the edge which goes to the right
  VEdge* el = new VEdge(start, par->site, p);
  VEdge* er = new VEdge(start, p, par->site);

  // neighbour from el is er
  // may be needed to combine both at the end?
  el->neighbour = er;
	edges->push_back(el);

  // tree has to be rebuild because a new parabola is added (original: pestavuju strom .. vkldm novou parabolu)
  // par become a inner node of the tree which describes the edge between new site p and the site of par
  par->edge = er;
	par->isLeaf = false;

    // par to the left of ps parabola
	VParabola * p0 = new VParabola(par->site);
    // ps parabola
	VParabola * p1 = new VParabola(p);
    // par to the right of ps parabola
	VParabola * p2 = new VParabola(par->site);

    // pars child to the right in the tree is p2
	par->SetRight(p2);
    // pars child to the left is a subtree...
  par->SetLeft(new VParabola());
    //... its root is the edge el ...
	par->Left()->edge = el;
    //... left child of its root is the old part of par ...
	par->Left()->SetLeft(p0);
    //... and the right child of its root is the new parabola of p
	par->Left()->SetRight(p1);
	
  // now check then p0 or p2 have their circle event
  CheckCircle(p0,mustHaveCircleEvent);
  CheckCircle(p2,mustHaveCircleEvent);
}

void Voronoi::RemoveParabola(VEvent* e)
{
	VParabola* p1 = e->arch;

	VParabola* xl = VParabola::GetLeftParent(p1);
	VParabola* xr = VParabola::GetRightParent(p1);

	VParabola* p0 = VParabola::GetLeftChild(xl);
	VParabola* p2 = VParabola::GetRightChild(xr);

  bool p0MustHaveCircleEvent=false;
  bool p2MustHaveCircleEvent=false;

  //Fehler - rechte und linke Parabel haben den gleichen Fokus
  //if(p0 == p2) std::cout << "chyba - prav a lev parabola m stejn ohnisko!\n";

  // delet p0's and p2's circle event <- p0 and p2 can't have a circle event because one of the squeezing parabola would be p1
  if(p0->cEvent){ deleted.insert(p0->cEvent); p0->cEvent = 0; p0MustHaveCircleEvent=true;}
  if(p2->cEvent){ deleted.insert(p2->cEvent); p2->cEvent = 0; p2MustHaveCircleEvent=true;}

  // TODO: Schnittpunktkoordinaten in dem Event speichern -> brauch nicht neu berechnet werden (Fehler?)
  // determine the point where p1 disappears / intersection point of 2 edges
  Vector2d* p = new Vector2d(e->intersecitonPoint);
	points.push_back(p);

  // p is the end point of the two edges xl and xr
  xl->edge->end = *p;
  xr->edge->end = *p;
	
  VParabola * higher=NULL;
	VParabola * par = p1;

    // the edge which is higher in the tree (xr or xl) becomes the new edge starting in p
	while(par != root)
	{
		par = par->parent;
		if(par == xl) higher = xl;
		if(par == xr) higher = xr;
	}
	higher->edge = new VEdge(p, p0->site, p2->site);
	edges->push_back(higher->edge);

  // delete p1 and the lower edge (xl or xr)
	VParabola * gparent = p1->parent->parent;
	if(p1->parent->Left() == p1)
	{
		if(gparent->Left()  == p1->parent) gparent->SetLeft ( p1->parent->Right() );
		if(gparent->Right() == p1->parent) gparent->SetRight( p1->parent->Right() );
	}
	else
	{
		if(gparent->Left()  == p1->parent) gparent->SetLeft ( p1->parent->Left()  );
		if(gparent->Right() == p1->parent) gparent->SetRight( p1->parent->Left()  );
	}

	delete p1->parent;
	delete p1;

  CheckCircle(p0,p0MustHaveCircleEvent);
  CheckCircle(p2,p2MustHaveCircleEvent);
}

void Voronoi::FinishEdge(VParabola * n)
{
  // my
  if(n==0){return;}//shouldn't be NULL -> have to finde the error
  if(n->isLeaf) {delete n; return;}
  double mx=0;
  double my=0;

  if(n->edge->direction->x > 0.0)	{
      mx = std::max(width/2.0,n->edge->start.x);
      my = n->edge->start.y + n->edge->direction->y * (mx - n->edge->start.x)/n->edge->direction->x;
  }
  if(n->edge->direction->x <0.0) {
      mx = std::min(-width/2.0,n->edge->start.x);
      my = n->edge->start.y + n->edge->direction->y * (mx - n->edge->start.x)/n->edge->direction->x;
  }
  if(n->edge->direction->x == 0 && n->edge->direction->y > 0){
      mx = n->edge->start.x;
      my = height/2.0;
  }
  if(n->edge->direction->x == 0 && n->edge->direction->y < 0){
      mx = n->edge->start.x;
      my = -height/2.0;
  }

  if(n->edge->direction->x == 0 && n->edge->direction->y == 0){
      mx = 0;
      my = 0;
  }

  Vector2d* end = new Vector2d(mx, my);

  n->edge->end = *end;
  points.push_back(end);

  FinishEdge(n->Left() );
  FinishEdge(n->Right());
  delete n;
}

double Voronoi::GetXOfEdge(VParabola* par,  double y) const
{
    VParabola* left = VParabola::GetLeftChild(par);
    VParabola* right= VParabola::GetRightChild(par);

    Vector2d* l = left->site;
    Vector2d* r = right->site;

    // if the edge is vertical then return its x-coordinate
    if(par->edge->direction->x==0){
      return par->edge->start.x;
    }

    if (fabs(l->y - y)<1e-8){
      return l->x;
    }

    if (fabs(r->y - y)<1e-8){
      return r->x;
    }

    // Use the quadratic formula.
    double z0 = 2*(r->y - y);
    double z1 = 2*(l->y - y);

    double a = 1/z0 - 1/z1;
    double b = -2*(r->x/z0 - l->x/z1);
    double c = ((r->x)*(r->x) + (r->y)*(r->y) - y*y)/z0
                   - ((l->x)*(l->x) + (l->y)*(l->y) - y*y)/z1;

    return ( -b - sqrt(b*b - 4*a*c) ) / (2*a);


//     double a1 = 1.0 / dp;
//     double b1 = -2.0 * l->x / dp;
//     double c1 = y + dp / 4 + l->x * l->x / dp;
			
//    dp = 2.0 * (r->y - y);
//    // if sweep line did not move downwards
//    if(dp==0) return r->x;

//     double a2 = 1.0 / dp;
//     double b2 = -2.0 * r->x/dp;
//     double c2 = ly + dp / 4 + r->x * r->x / dp;
			
//     double a = a1 - a2;
//     double b = b1 - b2;
//     double c = c1 - c2;
			
//     double disc = b*b - 4 * a * c;
//     double x1 = (-b + std::sqrt(disc)) / (2*a);
//     double x2 = (-b - std::sqrt(disc)) / (2*a);

//     double ry;
//    if(l->y < r->y ) ry =  std::max(x1, x2);
//    else ry = std::min(x1, x2);

//    return ry;
}

VParabola* Voronoi::GetParabolaByX(double xx) const
{
	VParabola* par = root;
  double x = 0.0;

	while(!par->isLeaf) // projdu stromem dokud nenarazm na vhodn list
	{
		x = GetXOfEdge(par, ly);
		if(x>xx) par = par->Left();
		else par = par->Right();				
	}
	return par;
}

double Voronoi::GetY(const Vector2d& p,  double x) const // focus, x-coordinate (original: ohnisko, x-souadnice)
{
  double dp = 2 * (p.y - ly);
  // double a1 = 1;
  double b1 = -2 * p.x;
  double c1 = p.x * p.x;
	
  return (x*x + b1*x + c1)/dp+ly+dp/4;
}

void Voronoi::CheckCircle(VParabola* b, bool mustHaveCircleEvent)
{
    // have to check if they are rigth, they look confusing
	VParabola* lp = VParabola::GetLeftParent(b);
	VParabola* rp = VParabola::GetRightParent(b);

	VParabola* a  = VParabola::GetLeftChild (lp);
	VParabola* c  = VParabola::GetRightChild(rp);

  // a or c don't exist
  if(!a || !c) return;
  // both have the same site and are so parts of the same parabola which was divided by b
  if(a->site == c->site) return;
  // or a's and c's site are above b's site, so a's and c's parabola can't squeeze b's one
  if((a->site->y > b->site->y) && (c->site->y > b->site->y)) return;

  Vector2d V1=(*(a->site)-*(b->site)).normalize();
  Vector2d V2=(*(b->site)-*(c->site)).normalize();
  // ignore collinear sites
  if(V1==V2) return;

//    if((a->site->x != b->site->x)&&(a->site->x != c->site->x)&&(b->site->x!=c->site->x)){
//        // a's site is above b's one, b's one is above c's one
//        if((a->site->y>b->site->y)&&(b->site->y>c->site->y)){
//            mustHaveCircleEvent = true;
//        }

//        // other way around
//        if((a->site->y<b->site->y)&&(b->site->y<c->site->y)){
//            mustHaveCircleEvent = true;
//        }
//    }

  // b's site is above a's and c's one
  if((a->site->y<b->site->y)&&(c->site->y<b->site->y)){
    mustHaveCircleEvent = true;
  }

  bool mustIntersect = false;

  if(mustHaveCircleEvent) mustIntersect= true;

  Vector2d* s = GetEdgeIntersection(lp->edge, rp->edge, mustIntersect);
  // no intersection
  if(s == NULL) return;
  // ACHTUNG: intersection added to points
  // (moved here from GetEdgeIntersection)
  points.push_back(s);

  // vector from a's site to the intersection point s
  Vector2d dv = *(a->site) - *s;
  // determine the distance between the a's site and s
  double d = dv.abs();

  // remember that the sweep line moves form top to bottom
  // the event there b is "squeezed" has to be behinde or on the sweepline
  //if(s->y - d > ly){return;} //--> need epsilon environment (problem test case 2h1a, 2h1b)

  //add new circle event to the queue and same question one comment above
  VEvent* e = new VEvent(new Vector2d(s->x, s->y - d), false);
  e->intersecitonPoint = *s;
  points.push_back(e->point);
  b->cEvent = e;
  e->arch = b;
  queue.push(e);

  return;
}//end CheckCircle

Vector2d* Voronoi::GetEdgeIntersection(VEdge* a, VEdge* b, bool mustIntersect) const
{
  double atimes = a->line->intersection(*(b->line));
  double btimes = b->line->intersection(*(a->line));

  // lines are parallel
  if(atimes==std::numeric_limits<double>::infinity()) return NULL;

  if((atimes<0||btimes<0)&&(!mustIntersect)) return NULL;

  // TODO
  //Vector2< double> asec =a->line->point(atimes);
  //Vector2< double> bsec =b->line->point(btimes);

  Vector2d* p = new Vector2d();

  if(mustIntersect&&(btimes<0)){
    *p = b->start;
  } else if(mustIntersect&&(atimes<0)){
    *p = a->start;
  } else {
    *p = a->line->point(atimes);
  }

  return p;

    /*
    // the slopes are similar, edges are parallel or on the same line
    if(a->f == b->f){
        return 0;
    }

     double y;
     double x;

    // a and b are not parallel to the y axis
    if(a->direction->x!=0 && b->direction->x!=0){
        x = (b->g-a->g) / (a->f - b->f);
        y = a->f * x + a->g;
    } else {
        // b is parallel to the y axis
        if (b->direction->x==0){
            x=b->start.x;
            y=a->f*x +a->g;
        } else { // a is parallel to the y axis
            x=a->start.x;
            y=b->f*x+b->g;
        }
    }

    // if the intersection point is equal to the start point
    // it might be a problem
    if((x - a->start.x)/a->direction->x < 0) return 0;
    if((y - a->start.y)/a->direction->y < 0) return 0;

    if((x - b->start.x)/b->direction->x < 0) return 0;
    if((y - b->start.y)/b->direction->y < 0) return 0;

    Vector2< double>  * p = new Vector2< double> (x, y);
	points.push_back(p);
	return p;
    */
}


	
