/**
* @file CanopyClustering.h
*
* @author <a href="mailto:holzhaue@informatik.hu-berlin.de">Florian Holzhauer</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CanopyClustering
*/

#ifndef _CanopyClustering_h_
#define _CanopyClustering_h_

#include "SampleSet.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

#include <vector>

template<class C>
class CanopyClustering
{
public: 
  CanopyClustering(double clusterThreshold = 0, int maxNumberOfClusters = 100)
    :
    numOfClusters(0),
    largestCluster(-1),
    clusters(maxNumberOfClusters),
    clusterThreshold(clusterThreshold)
  {
  }

  ~CanopyClustering() {}


  class CanopyCluster
  {
  protected:
    unsigned int _size;
    Vector2d _clusterSum;
    Vector2d _center;

    void add(const Vector2d& point)
    {
      _size++;
      _clusterSum += point;
      _center = _clusterSum / static_cast<double>(_size);
    }

    void set(const Vector2d& point)
    {
      _size = 1;
      _clusterSum = point;
      _center = point;
    }

  public:
    CanopyCluster() : _size(0){}
    virtual ~CanopyCluster(){}

    unsigned int size() const { return _size; }
    const Vector2d& clusterSum() const { return _clusterSum; }
    const Vector2d& center() const { return _center; }
  };//end class CanopyCluster


  unsigned int size() const { return numOfClusters; }
  const CanopyCluster& operator[](int index) const { ASSERT(index >= 0 && (unsigned int)index < this->numOfClusters); return clusters[index];}
  const CanopyCluster& getLargestCluster() const { ASSERT(largestCluster >= 0 && (unsigned int)largestCluster < this->numOfClusters); return (*this).clusters[largestCluster];  }
  int getLargestClusterID() const {return largestCluster;}

  void setClusterThreshold(const double clusterThreshold) {this->clusterThreshold = clusterThreshold;}

  void cluster(C& sampleSet)
  {
    numOfClusters = 0;
    largestCluster = -1;

    for (int j = 0; j < (int)sampleSet.size(); j++)
    {
      Sample2D& sample = sampleSet[j];
      sample.cluster = -1; // no cluster

      // look for a cluster with the smallest distance
      double minDistance = 10000; // 10m
      int minIdx = -1;

      for (unsigned int k = 0; k < numOfClusters; k++) {
        double dist = clusters[k].distance(sample.getPos());
        if(dist < minDistance)
        {
          minIdx = (int)k;
          minDistance = dist;
        }
      }//end for

      // try to add to the nearest cluster
      if(minIdx != -1 && isInCluster(clusters[minIdx], sample))
      {
        sample.cluster = minIdx;
        clusters[minIdx].add(sample.getPos());

        if(clusters[minIdx].size() > clusters[largestCluster].size()) {
          largestCluster = minIdx;
        }
      }
      // othervise create new cluster
      else if(numOfClusters < clusters.size()) // ACHTUNG: don't resize clusters
      {
        // initialize a new cluster
        clusters[numOfClusters].set(sample.getPos());
        sample.cluster = (int)numOfClusters;
        
        if(largestCluster == -1) {
          largestCluster = numOfClusters;
        }

        numOfClusters++;
      }//end if
    }//end for


    // merge close clusters
    // note: don't consider clusters smaller than minClusterSize
    const int minClusterSize = 4;
    for(unsigned int k = 0; k < numOfClusters; k++)
    {
      if((int)clusters[k].size() < minClusterSize) {
        continue;
      }
      for(unsigned int j = k+1; j < numOfClusters; j++) 
      {
        if ( (int)clusters[j].size() < minClusterSize) {
          continue;
        }
        // merge the clusters k and j
        if((clusters[k].center() - clusters[j].center()).abs() < 500)
        {
          clusters[k].merge(clusters[j]);
          clusters[j].clear();

          if(clusters[k].size() > clusters[largestCluster].size())
            largestCluster = (int)k;
          
          // TODO: make it more effivient
          for (int i = 0; i < (int)sampleSet.size(); i++)
          {
            if(sampleSet[i].cluster == (int)j) {
              sampleSet[i].cluster = (int)k;
            }
          } //end for i
        } //end if abs < 500
      } // end for j
    } //end for k
  }//end cluster


  unsigned int cluster(C& sampleSet, const Vector2d& start)
  {
    numOfClusters = 1;
    largestCluster = 0;
    CanopyClusterBuilder& cluster = clusters[0];
    cluster.set(start);

    for (unsigned int j = 0; j < sampleSet.size(); j++)
    {
      sampleSet[j].cluster = -1;
      if(isInCluster(cluster, sampleSet[j]))
      {
        sampleSet[j].cluster = 0;
        cluster.add(sampleSet[j].getPos());
      }
    }//end for j

    return cluster.size();
  }//end cluster



private:
  
  class CanopyClusterBuilder: public CanopyCluster
  {
  public:
    virtual ~CanopyClusterBuilder(){}
    CanopyClusterBuilder(){}
    CanopyClusterBuilder(const Vector2d& point) {
      set(point);
    }

    void add(const Vector2d& point) {
      CanopyCluster::add(point);
    }

    void set(const Vector2d& point) {
      CanopyCluster::set(point);
    }

    void merge(const CanopyCluster& other) {
      this->_size += other.size();
      this->_clusterSum = (this->_clusterSum + other.clusterSum()) * 0.5;
      this->_center = (this->_center + other.center()) * 0.5;
    }

    void clear() {
      this->_size = 0;
    }

    // TODO: make it switchable
    double distance(const Vector2d& point) const {
      return (center() - point).abs2();
    }
  };//end class CanopyClusterBuilder


  bool isInCluster(const CanopyClusterBuilder& cluster, const Sample2D& sample) const {
    return cluster.distance(sample.getPos()) < clusterThreshold;
  }

  // results of the clustering
  unsigned int numOfClusters;
  int largestCluster;
  std::vector<CanopyClusterBuilder> clusters;  //FIXME

  // parameter of clustering
  double clusterThreshold;
};

#endif //_CanopyClustering_h_
