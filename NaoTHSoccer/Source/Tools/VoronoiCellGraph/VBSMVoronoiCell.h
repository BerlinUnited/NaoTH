#ifndef _VBSMVoronoiCell_h_
#define _VBSMVoronoiCell_h_

#include <vector>
#include "VoronoiCell.h"


class VBSMData
{
public:
	VBSMData() {
	  fValue = 0;
	  gValue = 0;
	  hValue = 0;
	  parentNode = 0;
	  expanded = false;
	  ownNumber =0;
	  attractionValue = 0;
	  expansionRadius = 0;
	  wasTouched = 0;
	  dominatedBy = -1;
	  timeToArrival = -1;
	}

	/**********
     members used for dominant
    **********/
    double  timeToArrival; // may be not needed
    int     dominatedBy;   //-1= nobody; 0=border cell; 1=me; 2=somebody else;

    /**********
     properties used for A* search
    **********/
    /** own node number*/
    unsigned int ownNumber;

    /** The key to the parentNode*/
    int parentNode;

    /** children keys in the search tree*/
    std::vector<VoronoiCellT<VBSMData>*> children;

    /** The value of the node*/
    double fValue;

    /** The path costs to the node*/
    double gValue;

    /** The heuristic of the node */
    double hValue;

    /** The node was expanded during the A* search*/
    bool expanded;

    /** The attraction value at the position of the node
    * could be the potential field value or just some value
    * that describes how attractive is this point for the
    * search algorithm (it is used with the cost - function)
    */
    double attractionValue;

    /** The expansion radius of the node*/
    double expansionRadius;

    /** The node was added to the search tree*/
    bool wasTouched;


	/**********
     functions for A* search
    **********/

    /** Sets the index of the parent node of this node
    * @param parentNode The index of the parent node
    */
     void setParentNode(unsigned int parentNode)
    { this->parentNode = parentNode;}

    /** Returns the index of the parent node
    * @return The parent node
    */
    unsigned int getParentNode() const 
    { return parentNode; }

    /** Sets the function values
    * @param g The costs of the path to this node
    * @param h The heuristic costs from this node to the goal
    */
    void setFunctionValues(double g, double h)
    { gValue = g; hValue = h;fValue = g+h;}

    /** Returns the value of this node
    * @return The value f
    */
    double f() const
    { return fValue;}

    /** Returns the costs of the path to this node
    * @return The path costs g
    */
    double g() const
    { return gValue;}

      /** Returns the heuristics of the node
    * @return The heuristic h
    */
    double h() const
    { return hValue;}

    /** Retruns the attraction value of the node
    * @return The attraction value
    */
    double getAttractionValue() const
    { return attractionValue;}

    /** Checks if this node has been expanded
    * @return true, if the node has been expanded
    */
    bool hasBeenExpanded() const
    { return expanded;}
};

typedef VoronoiCellT<VBSMData> VoronoiCell;


#endif // _VBSMVoronoiCell_h_
