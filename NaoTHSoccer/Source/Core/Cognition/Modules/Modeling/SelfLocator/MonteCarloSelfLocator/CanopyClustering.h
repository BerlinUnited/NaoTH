/**
* @file CanopyClustering.h
*
* @author <a href="mailto:holzhaue@informatik.hu-berlin.de">Florian Holzhauer</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CanopyClustering
*/

#ifndef __CanopyClustering_h_
#define __CanopyClustering_h_

#include "SampleSet.h"
#include "MCSLParameters.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

template<class C>
class CanopyClustering
{
public: 
  CanopyClustering(C& sampleSet, MCSLParameters& parameters)
    :
    sampleSet(sampleSet),
    parameters(parameters)
  {
  }

  ~CanopyClustering() {}

  void cluster()
  {
    //vector<CanopyCluster> clusters;
    int numOfClusters = 0;

    for (unsigned int j = 0; j < sampleSet.size(); j++)
    {
      sampleSet[j].cluster = -1; // no cluster

      // look for a cluster with the smallest distance
      double minDistance = 10000; // 10m
      int minIdx = -1;

      for (int k = 0; k < numOfClusters; k++) { //FIXME, static number
        double dist = clusters[k].distance(sampleSet[j]);
        if(dist < minDistance)
        {
          minIdx = k;
          minDistance = dist;
        }
      }//end for

      // try to add to the nearedst cluster
      if(minIdx != -1 && isInCluster(clusters[minIdx], sampleSet[j]))
      {
        sampleSet[j].cluster = minIdx;
        clusters[minIdx].add(sampleSet[j]);
      }
      // othervise create new cluster
      else if(numOfClusters < maxNumberOfClusters)
      {
        // initialize a new cluster
        clusters[numOfClusters].center = sampleSet[j].getPos();
        clusters[numOfClusters].clusterSum = sampleSet[j].getPos();
        clusters[numOfClusters].size = 1;
        sampleSet[j].cluster = numOfClusters;
        numOfClusters++;
      }//end if
    }//end for

    // merge close clusters
    for(int k=0; k< numOfClusters; k++)
    {
      if(clusters[k].size < 4) {
        continue;
      }
      for(int j=k+1;j<numOfClusters;j++) {
        if ( clusters[j].size < 4) {
          continue;
        }
        // merge the clusters k and j
        if((clusters[k].center - clusters[j].center).abs() < 500)
        {
          clusters[k].size += clusters[j].size;
          clusters[j].size = 0;
          //this is kind of pointless because we wont use the new center afterwards.
          //the merge will be more accurate, but it is not that important because we only
          //merge close clusters, so the error is small
          //remove in case of performance issues:
          clusters[k].center = (clusters[k].center + clusters[j].center) * 0.5;
          for (unsigned int i = 0; i < sampleSet.size(); i++)
          {
            if(sampleSet[i].cluster == j) {
                sampleSet[i].cluster = k;
            }
          } //end for i
        } //end if abs < 500
      } // end for j
    } //end for k
  }

  int getClusterSize(const Vector2<double> start)
  {
    CanopyCluster cluster;
    cluster.center = start;
    cluster.clusterSum = start;
    cluster.size = 1;

    for (unsigned int j = 0; j < sampleSet.size(); j++)
    {
      sampleSet[j].cluster = -1;
      if(isInCluster(cluster, sampleSet[j]))
      {
        sampleSet[j].cluster = 0;
        cluster.add(sampleSet[j]);
      }
    }//end for j

    return (int)cluster.size;
  }

private:
  C& sampleSet;
  
  class CanopyCluster
  {
  public:
    ~CanopyCluster(){}
    CanopyCluster():size(0){}

    double size;
    Vector2<double> clusterSum;
    Vector2<double> center;

    void add(const Sample2D& sample)
    {
      size++;
      clusterSum += sample.getPos();
      center = (clusterSum / size);
    }

    // TODO: make it switchable
    double distance(const Sample2D& sample) const
    {
      return euclideanDistance(sample);
      //return manhattanDistance(sample);
    }
  
  private:
    double manhattanDistance(const Sample2D& sample) const
    {
      return std::fabs(center.x - sample.getPos().x)
           + std::fabs(center.y - sample.getPos().y);
    }

    double euclideanDistance(const Sample2D& sample) const
    {
        return (center - sample.getPos()).abs();
    }
  };

  bool isInCluster(const CanopyCluster& cluster, const Sample2D& sample) const
  {
    return cluster.distance(sample) < parameters.thresholdCanopy;
  }

  static const int maxNumberOfClusters = 100;
  CanopyCluster clusters[maxNumberOfClusters];  //FIXME

  MCSLParameters& parameters;

};

#endif //__CanopyClustering_h_
