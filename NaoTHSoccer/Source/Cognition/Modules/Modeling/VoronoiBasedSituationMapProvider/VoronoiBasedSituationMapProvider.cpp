/**
* @file VoronoiBasedSituationMapProvider.cpp
*/

#include "VoronoiBasedSituationMapProvider.h"
#include <Tools/Debug/Color.h>

using namespace vor;

VoronoiBasedSituationMapProvider::VoronoiBasedSituationMapProvider()
  :
 backgroundpattern(15,15,getFieldInfo().xFieldLength,getFieldInfo().yFieldLength,true)
{
  DEBUG_REQUEST_REGISTER("VBSMP:plotVoronoi", "plot the voronoi diagram", true);
  DEBUG_REQUEST_REGISTER("VBSMP:drawPotentialField","", true);
    
  voronoiCellGraph.setSize(getFieldInfo().xFieldLength,getFieldInfo().yFieldLength);
  voronoiCellGraph.setPattern(backgroundpattern);
  voronoiCellGraph.calculate();
}
 VoronoiBasedSituationMapProvider::~VoronoiBasedSituationMapProvider(){

 }
void VoronoiBasedSituationMapProvider::execute()
{
  //Apply Potential Field
  for(CellGraph::iterator it=voronoiCellGraph.getCellGraph().begin(); it!=voronoiCellGraph.getCellGraph().end(); it++)
  {
    //(*it).second.attractionValue = Math::random();
    (*it).second.attractionValue = (*it).first.abs2();
  }

 // doDebugRequests();

}

void VoronoiBasedSituationMapProvider::doDebugRequests()
{
    DEBUG_REQUEST("VBSMP:plotVoronoi",{

            FIELD_DRAWING_CONTEXT;
            PEN("000000", 20);
         
            
            //draw sites
            for(vor::Vertices::const_iterator it=this->voronoiCellGraph.getVertices().begin(); it!=voronoiCellGraph.getVertices().end(); ++it )
            {
                CIRCLE((double)((*it)->x),(double)((*it)->y),20);   //(x,y,radius)
            }


            //draw raw edges of the diagram
            for(Edges::const_iterator it=voronoiCellGraph.getEdges().begin();it!=voronoiCellGraph.getEdges().end();it++)
            {
                PEN("000000", 10);

                double x1 =(double)((*it)->start.x);
                double y1 =(double)((*it)->start.y);
                double x2 =(double)((*it)->end.x);
                double y2 =(double)((*it)->end.y);
                LINE(x1,y1,x2,y2);
            }
    });


 DEBUG_REQUEST("VBSMP:drawPotentialField",{
            FIELD_DRAWING_CONTEXT;

            double maxPotential = (*voronoiCellGraph.getCellGraph().begin()).second.attractionValue;

            for(CellGraph::iterator it=voronoiCellGraph.getCellGraph().begin();it !=voronoiCellGraph.getCellGraph().end(); it++)
      {
                if((*it).second.attractionValue>maxPotential)
        {
                    maxPotential=(*it).second.attractionValue;
        }
      }
            if(maxPotential != 0){
        for(CellGraph::iterator it=voronoiCellGraph.getCellGraph().begin();it !=voronoiCellGraph.getCellGraph().end(); it++)
        {
          std::stringstream x;
          std::stringstream y;

          std::list<Vector2< double> >::iterator it2=(*it).second.pointsOfCell.begin();
          x << (*it2).x;
          y << (*it2).y;

          for(it2=((*it).second.pointsOfCell.begin()++); it2!=(*it).second.pointsOfCell.end(); it2++)
          {
            x << "," << (*it2).x;
            y << "," << (*it2).y;
          }
          
          Color f(1.0,1.0,0.0,((*it).second.attractionValue/maxPotential));
          PEN(f,10);
          FILLPOLYGON(x.str(),y.str());

          x.str("");
          y.str("");
        }
           }
    });

}