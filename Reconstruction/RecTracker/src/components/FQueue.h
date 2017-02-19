/*
 * Queue.h
 *
 *  Created on: Mar 1, 2016
 *      Author: fpantale
 */

#ifndef FKDTREE_QUEUE_H_
#define FKDTREE_QUEUE_H_

#include <vector>
#include <cassert>

template<class T>

class FQueue
{
public:
	FQueue()
	{
#ifdef USE_VECTOR
		theBuffer.resize(0);
#endif

		theSize = 0;
		theFront = 0;
		theTail = 0;
		theCapacity = 0;

	}

	FQueue(unsigned int initialCapacity)
	{
#ifdef USE_VECTOR
		theBuffer.resize(initialCapacity);
#endif
		theSize = 0;
		theFront = 0;
		theTail = 0;

		theCapacity = initialCapacity;
	}

	FQueue(const FQueue<T> & v)
	{
		theSize = v.theSize;
		theBuffer = v.theBuffer;
		theFront = v.theFront;
		theTail = v.theTail;
		theCapacity = v.theCapacity;
	}

	FQueue(FQueue<T> && other) :
			theSize(0), theFront(0), theTail(0)
	{
#ifdef USE_VECTOR
		theBuffer.clear();
#endif
		theCapacity = other.theCapacity;
		theSize = other.theSize;
		theFront = other.theFront;
		theTail = other.theTail;
		theBuffer = other.theBuffer;
		other.theSize = 0;
		other.theFront = 0;
		other.theTail = 0;
	}

	FQueue<T>& operator=(FQueue<T> && other)
	{

		if (this != &other)
		{
#ifdef USE_VECTOR
			theBuffer.clear();
#endif
			theSize = other.theSize;
			theFront = other.theFront;
			theTail = other.theTail;
			theBuffer = other.theBuffer;
			other.theSize = 0;
			other.theFront = 0;
			other.theTail = 0;
		}
		return *this;

	}

	FQueue<T> & operator=(const FQueue<T>& v)
	{
		if (this != &v)
		{
#ifdef USE_VECTOR
			theBuffer.clear();
#endif
			theSize = v.theSize;
			theBuffer = v.theBuffer;
			theFront = v.theFront;
			theTail = v.theTail;
		}
		return *this;

	}
	~FQueue()
	{
	}

	unsigned int size() const
	{
		return theSize;
	}
	bool empty() const
	{
		return theSize == 0;
	}
	T & front()
	{

		return theBuffer[theFront];

	}
	T & tail()
	{
		return theBuffer[theTail];
	}

  constexpr unsigned int wrapIndex(unsigned int i) {
    return i & (theBuffer.size()-1);
  }

	void push_back(const T & value)
	{
#ifdef USE_VECTOR
		if (theSize >= theCapacity)
		{
			theBuffer.reserve(theCapacity + theTail);
			if (theFront != 0)
			{
				for (unsigned int i = 0; i < theTail; ++i)
				{
					theBuffer.push_back(theBuffer[i]);
				}
				theCapacity += theTail;

				theTail = 0;
			}
			else
			{

				theBuffer.resize(theCapacity + 16);
				theTail += theCapacity;
				theCapacity += 16;

			}

		}
#endif
                assert(theSize < theBuffer.size());
		theBuffer[theTail] = value;
		theTail = wrapIndex(theTail + 1);

		theSize++;


	}

	void print()
	{
		std::cout << "printing the content of the queue:" << std::endl;
		for(unsigned int i = theFront;  i  != theTail; i = ( i+ 1) % theCapacity)
			std::cout << theBuffer[i] << " at position " << i << std::endl;

	}
	T pop_front()
	{

		if (theSize > 0)
		{
			T element = theBuffer[theFront];
			theFront = wrapIndex(theFront + 1);
			theSize--;


			return element;
		}
	}

	void pop_front(const unsigned int numberOfElementsToPop)
	{
		unsigned int elementsToErase =
				theSize > numberOfElementsToPop ?
						numberOfElementsToPop : theSize;
		theSize -= elementsToErase;
		theFront = wrapIndex(theFront + elementsToErase);
	}

	void reserve(unsigned int capacity)
	{
#ifdef USE_VECTOR
		theBuffer.reserve(capacity);
#endif
	}
	void resize(unsigned int capacity)
	{
#ifdef USE_VECTOR
		theBuffer.resize(capacity);
#endif

	}

	T & operator[](unsigned int index)
	{
		return theBuffer[wrapIndex(theFront + index)];
	}

	void clear()
	{
#ifdef USE_VECTOR
		theBuffer.clear();
#endif
		theSize = 0;
		theFront = 0;
		theTail = 0;
	}
private:
	unsigned int theSize;
	unsigned int theFront;
	unsigned int theTail;
#ifdef USE_VECTOR
	std::vector<T> theBuffer;
#endif
        std::array<T, 1024> theBuffer;
	unsigned int theCapacity;

};

#endif
