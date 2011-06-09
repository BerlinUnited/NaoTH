/* 
 * File:   BlobList.h
 * Author: claas
 *
 * Created on 15. Februar 2010, 00:23
 */

#ifndef _BlobList_H
#define	_BlobList_H

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Moments2.h>
#include <Tools/ColorClasses.h>
#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Container.h>

#include "Blob.h"

class BlobList : public Printable, public Container<Blob>
{
public:
  BlobList()
  {
    blobNumber = 0;
  }

  virtual ~BlobList(){}

  enum {maxNumberOfBlobs = 20}; /**< The maximum number of blobs. */
  //enum {maxNumberOfMoments = 4}; /**< The maximum number of moments. */

  Blob blobs[maxNumberOfBlobs];

  int blobNumber; /**< The actual number of blobs */


  void add(const Blob& blob)
  {
    if(blobNumber < maxNumberOfBlobs)
    {
      blobs[blobNumber++] = blob;
    }
  }//end add

  void remove(int blobIndex)
  {
    if(blobIndex >= 0)
    {
      for(int n = blobIndex; n < maxNumberOfBlobs; n++)
      {
        blobs[n] = blobs[n + 1];
      }
      blobNumber--;
    }//end if
  }//end add

  int getLargestBlob() const
  {
    int largest = -1;
    double size = 0;
    for(int n = 0; n < blobNumber; n++)
    {
      if(blobs[n].moments.getRawMoment(0, 0) > size)
      {
        largest = n;
        size = blobs[n].moments.getRawMoment(0, 0);
      }
    }
    return largest;
  }//end getLargestBlob

  void reset()
  {
    blobNumber = 0;
  }//end reset

  virtual void print(ostream& stream) const
  {
    stream << "Count: " << blobNumber << endl << "--------------" << endl;
    for(int n = 0; n < blobNumber; n++)
    {
      stream << "Blob Nr." << (n + 1) << endl;
      stream << "Center = " << blobs[blobNumber].centerOfMass << endl;
      stream << "Mass =  " << blobs[blobNumber].blobMass << endl;
      stream << "Color = " << blobs[blobNumber].color << endl;
      stream << "upper Left = " << blobs[blobNumber].upperLeft << endl;
      stream << "upper Right = " << blobs[blobNumber].upperRight << endl;
      stream << "lower Left = " << blobs[blobNumber].lowerLeft << endl;
      stream << "lower Right = " << blobs[blobNumber].lowerRight << endl;
//        blobs[blobNumber].moments
    }
  };

};//end class BlobPercept

#endif	/* _BlobList_H */

