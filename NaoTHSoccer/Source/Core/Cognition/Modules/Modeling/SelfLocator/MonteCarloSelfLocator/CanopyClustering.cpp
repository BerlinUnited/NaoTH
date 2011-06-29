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

void CanopyClustering::cluster()
{

  //vector<CanopyCluster> clusters;
  int numOfClusters = 0;

  for (int j = 0; j < sampleSet.numberOfParticles; j++) 
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
      clusters[numOfClusters].center = sampleSet[j];
      clusters[numOfClusters].clusterSum = sampleSet[j].translation;
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
      if((clusters[k].center.translation - clusters[j].center.translation).abs() < 500) 
      {
        clusters[k].size += clusters[j].size;
        clusters[j].size = 0;
        //this is kind of pointless because we wont use the new center afterwards.
        //the merge will be more accurate, but it is not that important because we only
        //merge close clusters, so the error is small
        //remove in case of performance issues:
        clusters[k].center.translation = (clusters[k].center.translation + clusters[j].center.translation) * 0.5;
        for (int i = 0; i < sampleSet.numberOfParticles; i++)
        {
          if(sampleSet[i].cluster == j) {
              sampleSet[i].cluster = k;
          }
        } //end for i
      } //end if abs < 500
    } // end for j
  } //end for k
}//end cluster


int CanopyClustering::getClusterSize(const Vector2<double> start)
{
  CanopyCluster cluster;
  cluster.center.translation = start;
  cluster.clusterSum = start;
  cluster.size = 1;

  for (int j = 0; j < sampleSet.numberOfParticles; j++) 
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

bool CanopyClustering::isInCluster(const CanopyCluster& cluster, const Sample& sample) const
{
  return cluster.distance(sample) < parameters.thresholdCanopy;
}//end isInCluster


void CanopyClustering::CanopyCluster::add(const Sample& sample) 
{
  size++;
  clusterSum += sample.translation;
  center.translation = (clusterSum / size);
}//end add

// TODO: make it switchable
double CanopyClustering::CanopyCluster::distance(const Sample& sample) const
{
  return euclideanDistance(sample);
  //return manhattanDistance(sample);
}//end distance

double CanopyClustering::CanopyCluster::manhattanDistance(const Sample& sample) const
{
  return std::fabs(center.translation.x - sample.translation.x)
       + std::fabs(center.translation.y - sample.translation.y);
}//end manhattanDistance

double CanopyClustering::CanopyCluster::euclideanDistance(const Sample& sample) const
{
  return (center.translation - sample.translation).abs();
}//end euclideanDistance
