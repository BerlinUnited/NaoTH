/**
 * @file RingBufferWithSum.h
 *
 * Declaration of template class RingBufferWithSum
 *
 * @author Matthias Jüngel
 * @author Tobias Oberlies
 */

#ifndef _RingBufferWithSum_h_
#define _RingBufferWithSum_h_

#include <algorithm>
#include <assert.h>

/**
 * @class RingBufferWithSum
 *
 * Template class for cyclic buffering of the last n values of the type C
 * and with a function that returns the sum of all entries.
 */
template <class C, int n> class RingBufferWithSum
{
  public:
    /** Constructor */
    RingBufferWithSum() {init();}

    /**
     * initializes the RingBufferWithSum
     */
    void init () {current = n - 1; numberOfEntries = 0; sum = C();}

    /**
     * adds an entry to the buffer
     * \param value value to be added
     */
    void add (C value) 
    {
      if(numberOfEntries == n) sum -= (*this)[numberOfEntries - 1];
      sum += value;
      current++;
      if (current==n) current=0;
      if (++numberOfEntries >= n) numberOfEntries = n;
      buffer[current] = value;
    }

    C getSum() {
      return sum;
    }

    C getMinimum()
    {
      // Return 0 if buffer is empty
      if (0==numberOfEntries) return C();

      C min = buffer[0];
      for(int i = 0; i < numberOfEntries;i++)
      {
        if(buffer[i] < min) min = buffer[i];
      }
      return min;
    }

    /**
     * returns the average value of all entries
     * \return the average value
     */
    C getAverage()
    {
      // Return 0 if buffer is empty
      if (0==numberOfEntries) return C();
      return (sum / numberOfEntries);
    }

    // copied from AustinVilla 2012
    C getMedian() 
    {
      C temp[n];
      memcpy(temp,buffer,n * sizeof(C));
      int mid = (int)(n/2);
      std::nth_element(temp,temp+mid,temp+n);
      return temp[mid];
    }

    const C& last () const {
      return buffer[current];
    }

    const C& first () const {
      return getEntry(numberOfEntries - 1);
    }

    /**
     * returns a constant entry.
     * \param i index of entry counting from last added (last=0,...)
     * \return a reference to the buffer entry
     */
    const C& getEntry (int i) const {
      assert(i >= 0 && i < numberOfEntries);
      return buffer[i > current ? n + current - i : current - i];
    }

    const C& operator[] (int i) const {
      return buffer[i > current ? n + current - i : current - i];
    }

    inline int size() const {
      return numberOfEntries;
    }

    /**
    * Returns the maximum entry count.
    * \return The maximum entry count.
    */
    inline int getMaxEntries() const {
      return n;
    }

  private:
    int current;
    int numberOfEntries;
    C buffer[n];
    C sum;
};

#endif // _RingBufferWithSum_h_

