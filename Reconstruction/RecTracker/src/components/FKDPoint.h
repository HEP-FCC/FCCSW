/*
 * FFKDPoint.h
 *
 *  Created on: Feb 10, 2016
 *      Author: fpantale
 *
 */

#ifndef FKDTREE_KDPOINT_H_
#define FKDTREE_KDPOINT_H_
#include <array>
#include <iostream>
#include <utility>
template <class TYPE, int numberOfDimensions>
class FKDPoint {

public:
  FKDPoint() : theElements(), theId(0) {}

  FKDPoint(const FKDPoint<TYPE, numberOfDimensions>& other) : theElements(other.theElements), theId(other.theId) {}

  template <class T>
  FKDPoint<TYPE, numberOfDimensions>& operator=(const FKDPoint<TYPE, numberOfDimensions>& other) {
    if (this != &other) {
      theId = other.theId;
      theElements = other.theElements;
    }
    return *this;
  }

  FKDPoint(TYPE x, TYPE y, unsigned int id) {
    theId = id;
    theElements[0] = x;
    theElements[1] = y;
  }

  FKDPoint(TYPE x, TYPE y, TYPE z, unsigned int id) {
    theId = id;
    theElements[0] = x;
    theElements[1] = y;
    theElements[2] = z;
  }

  FKDPoint(TYPE x, TYPE y, TYPE z, TYPE w, unsigned int id) {
    theId = id;
    theElements[0] = x;
    theElements[1] = y;
    theElements[2] = z;
    theElements[3] = w;
  }

  // the user should check that i < numberOfDimensions
  TYPE& operator[](int const i) { return theElements[i]; }

  TYPE const& operator[](int const i) const { return theElements[i]; }

  void setDimension(int i, const TYPE& value) { theElements[i] = value; }

  void setId(const unsigned int id) { theId = id; }

  unsigned int getId() const { return theId; }

  void print() const {
    std::cout << "point id: " << theId << std::endl;
    for (auto i : theElements) {
      std::cout << i << std::endl;
    }
  }

private:
  std::array<TYPE, numberOfDimensions> theElements;
  unsigned int theId;
};

/* Utility functions to create 1-, 2-, 3-, or 4-Points from values. */
template <typename TYPE>
FKDPoint<TYPE, 1> make_FKDPoint(TYPE x, unsigned int id) {
  FKDPoint<TYPE, 1> result;
  result.setDimension(0, x);
  result.setId(id);
  return result;
}

template <typename TYPE>
FKDPoint<TYPE, 2> make_FKDPoint(TYPE x, TYPE y, unsigned int id) {
  FKDPoint<TYPE, 2> result;
  result.setDimension(0, x);
  result.setDimension(1, y);
  result.setId(id);
  return result;
}

template <typename TYPE>
FKDPoint<TYPE, 3> make_FKDPoint(TYPE x, TYPE y, TYPE z, unsigned int id) {
  FKDPoint<TYPE, 3> result;
  result.setDimension(0, x);
  result.setDimension(1, y);
  result.setDimension(2, z);
  result.setId(id);
  return result;
}

template <typename TYPE>
FKDPoint<TYPE, 4> make_FKDPoint(TYPE x, TYPE y, TYPE z, TYPE w, unsigned int id) {
  FKDPoint<TYPE, 4> result;
  result.setDimension(0, x);
  result.setDimension(1, y);
  result.setDimension(2, z);
  result.setDimension(3, w);
  result.setId(id);
  return result;
}

#endif /* FKDTREE_KDPOINT_H_ */
