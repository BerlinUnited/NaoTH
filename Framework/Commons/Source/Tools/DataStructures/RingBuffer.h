/**
 * @file RingBuffer.h
 *
 * Declaration of class RingBuffer
 *
 * @author Max Risler
 */

#ifndef _RingBuffer_h_
#define _RingBuffer_h_

#include <assert.h>

/**
 * @class RingBuffer
 *
 * template class for cyclic buffering of the last n values of Type V
 */
template <class V, int n> class RingBuffer
{
public:
  
  typedef int size_type;

  RingBuffer() { clear(); }

  /**
    * adds an entry to the buffer
    * \param v value to be added
    */
  void add (const V& v) 
  {
    add();
    buffer[current] = v;
  }

  /**
    * adds an entry to the buffer.
    * The new head is not initialized, but can be changed afterwards.
    */
  void add () 
  {
    current++;
    if (current==n) current=0;
    if (++numberOfEntries >= n) numberOfEntries = n;
  }

  /**
    * removes the first added entry to the buffer
    */
  void removeFirst () {
    --numberOfEntries;
  }

  V& last () {
    return buffer[current];
  }

  const V& last () const {
    return buffer[current];
  }

  V& first () {
    return getEntry(numberOfEntries - 1);
  }

  const V& first () const {
    return getEntry(numberOfEntries - 1);
  }

  /**
    * returns an entry
    * \param v the value the entry i shall be updated with
    * \param i index of entry counting from last added (last=0,...)
    */
  void updateEntry(const V& v, int i)
  {
    int j = current - i;
    j %= n;
    if (j < 0) j += n;
    buffer[j] = v;
  }

  V& getEntry (int i) {
    assert(i >= 0 && i < numberOfEntries);
    return buffer[i > current ? n + current - i : current - i];
  }

  const V& getEntry (int i) const {
    assert(i >= 0 && i < numberOfEntries);
    return buffer[i > current ? n + current - i : current - i];
  }

  /**
    * returns an entry
    * \param i index of entry counting from last added (last=0,...)
    * \return a reference to the buffer entry
    */
  V& operator[] (int i) {
    return buffer[i > current ? n + current - i : current - i];
  }

  /**
    * returns a constant entry.
    * \param i index of entry counting from last added (last=0,...)
    * \return a reference to the buffer entry
    */
  const V& operator[] (int i) const {
    return buffer[i > current ? n + current - i : current - i];
  }

  /**
  * clears the buffer
  */
  void clear() { 
    current = n - 1; 
    numberOfEntries = 0;
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

  bool isFull() const {
    return numberOfEntries == n;
  }

protected:
  int current;
  int numberOfEntries;
  V buffer[n];
};


#endif // _RingBuffer_h_
