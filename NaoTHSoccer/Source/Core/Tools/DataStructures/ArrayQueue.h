/**
* @file ArrayQueue.h
*
* Definition of class ArrayQueue
*/

#ifndef __ArrayQueue_H_
#define __ArrayQueue_H_

#include <algorithm>
#include <cassert>

using namespace std;

template<typename T> class ArrayQueue
{
public:
	ArrayQueue();
	~ArrayQueue();
	bool empty() const;
  void clear();
	T& front();
	void push(const T& value);
	void pop();

private:
	// Disable copy constructor and assignment operator.
	ArrayQueue(const ArrayQueue& other);
	ArrayQueue& operator=(const ArrayQueue& other);

	ArrayQueue(size_t capacity_);
	void swap(ArrayQueue& other);
	size_t size() const;
	T& elementAt(size_t index);
	void expand();

	size_t capacity_;
	T* elements_;
	size_t beginIndex_;
	size_t size_;
};//end class ArrayQueue

template<typename T> ArrayQueue<T>::ArrayQueue()
	: capacity_(1), elements_(new T[capacity_]), beginIndex_(0), size_(0)
{
}

template<typename T> ArrayQueue<T>::~ArrayQueue()
{
	delete[] elements_;
}

template<typename T> bool ArrayQueue<T>::empty() const
{
	return size() == 0;
}

template<typename T> void ArrayQueue<T>::clear()
{
	size_ = 0;
}

template<typename T> T& ArrayQueue<T>::front()
{
	return elementAt(0);
}

template<typename T> void ArrayQueue<T>::push(const T& value)
{
	if (size() >= capacity_)
	{
		expand();
	}
	assert(size() < capacity_);

	elementAt(size()) = value;
	assert(size_ < capacity_);
	size_++;
}

template<typename T> void ArrayQueue<T>::pop()
{
	assert(!empty());

	beginIndex_++;
	// Wrap around to beginning.
	beginIndex_ %= capacity_;
	assert(beginIndex_ < capacity_);
	assert(size_ > 0);
	size_--;
}

template<typename T> ArrayQueue<T>::ArrayQueue(size_t capacity)
	: capacity_(capacity), elements_(new T[capacity_]), beginIndex_(0),
	size_(0)
{
}

template<typename T> void ArrayQueue<T>::swap(ArrayQueue& other)
{
	std::swap(capacity_, other.capacity_);
	std::swap(elements_, other.elements_);
	std::swap(beginIndex_, other.beginIndex_);
	std::swap(size_, other.size_);
}

template<typename T> size_t ArrayQueue<T>::size() const
{
	return size_;
}

template<typename T> T& ArrayQueue<T>::elementAt(size_t index)
{
	// Allow writing to one-past-the-last element.
	assert(index <= size());
	size_t elementsIndex = (beginIndex_ + index) % capacity_;
	assert(elementsIndex < capacity_);
	return elements_[elementsIndex];
}

template<typename T> void ArrayQueue<T>::expand()
{
	const int EXPANSION_FACTOR = 2;
	ArrayQueue newObject(capacity_ * EXPANSION_FACTOR);
	for (size_t index = 0; index < size(); index++)
	{
		newObject.push(elementAt(index));
	}
	swap(newObject);
}

#endif // __ArrayQueue_H_
