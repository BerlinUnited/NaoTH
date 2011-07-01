/**
* @file CanopyClustering.cpp
*
* @author <a href="mailto:holzhaue@informatik.hu-berlin.de">Florian Holzhauer</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class CanopyClustering
*/

#include "CanopyClustering.h"
#include <cmath>
/* WARNING: This is a really dirty implementation due to my lack of proper  
            C-Skills. Needs to be redone, currently only prototype status.
            Technically works, but from a code point of view really ugly */

template<class C>
void CanopyClustering<C>::cluster()
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
}//end cluster


template<class C>
int CanopyClustering<C>::getClusterSize(const Vector2<double> start)
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
}//end getClusteSize

template<class C>
bool CanopyClustering<C>::isInCluster(const CanopyCluster& cluster, const Sample2D& sample) const
{
  return cluster.distance(sample) < parameters.thresholdCanopy;
}//end isInCluster

template<class C>
void CanopyClustering<C>::CanopyCluster::add(const Sample2D& sample) 
{
  size++;
  clusterSum += sample.getPos();
  center = (clusterSum / size);
}//end add

// TODO: make it switchable
template<class C>
double CanopyClustering<C>::CanopyCluster::distance(const Sample2D& sample) const
{
  return euclideanDistance(sample);
  //return manhattanDistance(sample);
}//end distance

template<class C>
double CanopyClustering<C>::CanopyCluster::manhattanDistance(const Sample2D& sample) const
{
  return std::fabs(center.x - sample.getPos().x)
       + std::fabs(center.y - sample.getPos().y);
}//end manhattanDistance

template<class C>
double CanopyClustering<C>::CanopyCluster::euclideanDistance(const Sample2D& sample) const
{
  return (center - sample.getPos()).abs();
}//end euclideanDistance
