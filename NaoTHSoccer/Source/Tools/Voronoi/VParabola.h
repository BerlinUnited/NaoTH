#ifndef VParabola_h
#define VParabola_h

#include <Tools/Math/Vector2.h>
#include "VEdge.h"

/*
	Parabolas and events have pointers to each other, so we declare class VEvent, which will be defined later.
*/

class VEvent;

/*
	A class that stores information about an item in beachline sequence (see Fortune's algorithm). 
	It can represent an arch of parabola or an intersection between two archs (which defines an edge).
	In my implementation I build a binary tree with them (internal nodes are edges, leaves are archs).
*/

class VParabola
{
public:

	/*
		isLeaf		: flag whether the node is Leaf or internal node
		site		: pointer to the focus point of parabola (when it is parabola)
		edge		: pointer to the edge (when it is an edge)
		cEvent		: pointer to the event, when the arch disappears (circle event)
		parent		: pointer to the parent node in tree
	*/

	bool isLeaf;
  Vector2d*	site;
	VEdge* edge;
	VEvent*	cEvent;
	VParabola * parent;

	/*
		Constructors of the class (empty for edge, with focus parameter for an arch).
	*/
	
	VParabola	();
  VParabola (Vector2d* s);

	/*
		Access to the children (in tree).
	*/
	void SetLeft (VParabola * p) {_left  = p; p->parent = this;}
	void SetRight(VParabola * p) {_right = p; p->parent = this;}

	VParabola* Left () const { return _left;  }
	VParabola* Right() const { return _right; }

	/*
		Some useful tree operations

		GetLeft			: returns the closest left leave of the tree
		GetRight		: returns the closest right leafe of the tree
		GetLeftParent	: returns the closest parent which is on the left
		GetLeftParent	: returns the closest parent which is on the right
		GetLeftChild	: returns the closest leave which is on the left of current node
		GetRightChild	: returns the closest leave which is on the right of current node
	*/

	static VParabola* GetLeft       (VParabola * p);
	static VParabola* GetRight      (VParabola * p);
	static VParabola* GetLeftParent (VParabola * p);
	static VParabola* GetRightParent(VParabola * p);
	static VParabola* GetLeftChild  (VParabola * p);
	static VParabola* GetRightChild (VParabola * p);
	
private:
	VParabola* _left;
	VParabola* _right;

};

#endif
