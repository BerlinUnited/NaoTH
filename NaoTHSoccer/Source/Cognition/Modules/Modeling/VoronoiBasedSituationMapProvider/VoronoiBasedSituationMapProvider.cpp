/**
* @file VoronoiBasedSituationMapProvider.cpp
*/

#include "VoronoiBasedSituationMapProvider.h"

using namespace vor;

VoronoiBasedSituationMapProvider::VoronoiBasedSituationMapProvider()
	:
	backgroundpattern(15,15,getFieldInfo().xFieldLength,getFieldInfo().yFieldLength,true)
{
	DEBUG_REQUEST_REGISTER("VBSMP:plotVoronoi", "plot the voronoi diagram", true);
    
	voronoiCellGraph.setSize(getFieldInfo().xFieldLength,getFieldInfo().yFieldLength);

	for(std::vector<Vector2d>::iterator it=backgroundpattern.pattern.begin();it!=backgroundpattern.pattern.end();it++){
		//ver.push_back(new Vector2d(*it));
		voronoiCellGraph.addVertex(*it);
	}

	voronoiCellGraph.calculate();
}

VoronoiBasedSituationMapProvider::~VoronoiBasedSituationMapProvider()
{
  // clean some stuff here
}

void VoronoiBasedSituationMapProvider::execute()
{
 // DEBUG_REQUEST("VBSMP:plotVoronoi",{

            FIELD_DRAWING_CONTEXT;
            PEN("000000", 20);

            //draw sites
            for(vor::Vertices::const_iterator it=voronoiCellGraph.getVertices().begin(); it!=voronoiCellGraph.getVertices().end(); ++it ){
                CIRCLE((double)((*it)->x),(double)((*it)->y),20);   //(x,y,radius)
            }


            //draw raw edges of the diagram
            for(Edges::const_iterator it=voronoiCellGraph.getEdges().begin();it!=voronoiCellGraph.getEdges().end();it++){
                PEN("000000", 10);

                double x1 =(double)((*it)->start.x);
                double y1 =(double)((*it)->start.y);
                double x2 =(double)((*it)->end.x);
                double y2 =(double)((*it)->end.y);
                LINE(x1,y1,x2,y2);
            }

            //}
   //);

}
