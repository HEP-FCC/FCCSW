#ifndef FKDTREE_FKDTREE_H_
#define FKDTREE_FKDTREE_H_

#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <utility>

#include "FKDPoint.h"
#include "FQueue.h"
#define FLOOR_LOG2(X) ((unsigned int) (31 - __builtin_clz(X | 1)))
#define CEIL_LOG2(X) ((unsigned int)(X <= 1 ? 0 : 32 - __builtin_clz(X - 1)))
template<class TYPE, unsigned int numberOfDimensions>
class FKDTree
{

public:

	FKDTree(const unsigned int nPoints)
	{
		theNumberOfPoints = nPoints;

		theDepth = FLOOR_LOG2(nPoints);
		for (auto& x : theDimensions)
			x.resize(theNumberOfPoints);
		theIntervalLength.resize(theNumberOfPoints, 0);
		theIntervalMin.resize(theNumberOfPoints, 0);
		theIds.resize(theNumberOfPoints);
		thePoints.reserve(theNumberOfPoints);

	}

	FKDTree(const unsigned int nPoints,
			const std::vector<FKDPoint<TYPE, numberOfDimensions> >& points)
	{
		theNumberOfPoints = nPoints;
		theDepth = FLOOR_LOG2(nPoints);
		for (auto& x : theDimensions)
			x.resize(theNumberOfPoints);
		theIntervalLength.resize(theNumberOfPoints, 0);
		theIntervalMin.resize(theNumberOfPoints, 0);
		theIds.resize(theNumberOfPoints, 0);
		thePoints = points;

	}

	FKDTree()
	{
		theNumberOfPoints = 0;
		theDepth = 0;
		for (auto& x : theDimensions)
			x.clear();
		theIntervalLength.clear();
		theIntervalMin.clear();
		theIds.clear();
		thePoints.clear();
	}

	FKDTree(const FKDTree<TYPE, numberOfDimensions>& v);

	FKDTree(FKDTree<TYPE, numberOfDimensions> && other)
	{
		theNumberOfPoints(std::move(other.theNumberOfPoints));
		theDepth(std::move(other.theDepth));

		theIntervalLength.clear();
		theIntervalMin.clear();
		theIds.clear();
		thePoints.clear();
		for (auto& x : theDimensions)
			x.clear();

		theIntervalLength = std::move(other.theIntervalLength);
		theIntervalMin = std::move(other.theIntervalMin);
		theIds = std::move(other.theIds);

		thePoints = std::move(other.thePoints);
		for (unsigned int i = 0; i < numberOfDimensions; ++i)
			theDimensions = std::move(other.theDimensions);
	}

	FKDTree<TYPE, numberOfDimensions>& operator=(
			FKDTree<TYPE, numberOfDimensions> && other)
	{

		if (this != &other)
		{
			theNumberOfPoints(std::move(other.theNumberOfPoints));
			theDepth(std::move(other.theDepth));

			theIntervalLength.clear();
			theIntervalMin.clear();
			theIds.clear();
			thePoints.clear();
			for (auto& x : theDimensions)
				x.clear();

			theIntervalLength = std::move(other.theIntervalLength);
			theIntervalMin = std::move(other.theIntervalMin);
			theIds = std::move(other.theIds);

			thePoints = std::move(other.thePoints);
			for (unsigned int i = 0; i < numberOfDimensions; ++i)
				theDimensions = std::move(other.theDimensions);
		}
		return *this;

	}

	void push_back(const FKDPoint<TYPE, numberOfDimensions>& point)
	{

		thePoints.push_back(point);
		for (unsigned int i = 0; i < numberOfDimensions; ++i)
			theDimensions.at(i).push_back(point[i]);
		theIds.push_back(point.getId());
	}

	void push_back(FKDPoint<TYPE, numberOfDimensions> && point)
	{

		thePoints.push_back(point);
		for (unsigned int i = 0; i < numberOfDimensions; ++i)
			theDimensions.at(i).push_back(point[i]);
		theIds.push_back(point.getId());
	}

	void add_at_position(const FKDPoint<TYPE, numberOfDimensions>& point,
			const unsigned int position)
	{
		for (unsigned int i = 0; i < numberOfDimensions; ++i)
			theDimensions[i][position] = point[i];
		theIds[position] = point.getId();

	}

	void add_at_position(FKDPoint<TYPE, numberOfDimensions> && point,
			const unsigned int position)
	{
		for (unsigned int i = 0; i < numberOfDimensions; ++i)
			theDimensions[i][position] = point[i];
		theIds[position] = point.getId();

	}

	FKDPoint<TYPE, numberOfDimensions> getPoint(unsigned int index) const
	{

		FKDPoint<TYPE, numberOfDimensions> point;

		for (unsigned int i = 0; i < numberOfDimensions; ++i)
			point.setDimension(i, theDimensions[i][index]);

		point.setId(theIds[index]);

		return point;
	}

	std::vector<unsigned int> search_in_the_box_branchless(
			const FKDPoint<TYPE, numberOfDimensions>& minPoint,
			const FKDPoint<TYPE, numberOfDimensions>& maxPoint)
	{
		FQueue<unsigned int> indecesToVisit(256);
		std::vector<unsigned int> foundPoints;
		foundPoints.reserve(16);
		indecesToVisit.push_back(0);
		unsigned int index;
		bool intersection;
		int dimension;
		unsigned int numberOfIndecesToVisitThisDepth;
		int maxNumberOfSonsToVisitNext;
		int numberOfSonsToVisitNext;
		unsigned int firstSonToVisitNext;
		for (unsigned int depth = 0; depth < theDepth + 1; ++depth)
		{

			dimension = depth % numberOfDimensions;
			numberOfIndecesToVisitThisDepth = indecesToVisit.size();
			for (unsigned int visitedIndecesThisDepth = 0;
					visitedIndecesThisDepth < numberOfIndecesToVisitThisDepth;
					visitedIndecesThisDepth++)
			{

				index = indecesToVisit[visitedIndecesThisDepth];
				intersection = intersects(index, minPoint, maxPoint, dimension);
				firstSonToVisitNext = leftSonIndex(index);
				maxNumberOfSonsToVisitNext = (firstSonToVisitNext
						< theNumberOfPoints)
						+ ((firstSonToVisitNext + 1) < theNumberOfPoints);

				if (intersection)
				{
					if (is_in_the_box(index, minPoint, maxPoint))
					{
						foundPoints.emplace_back(theIds[index]);
					}
					numberOfSonsToVisitNext = maxNumberOfSonsToVisitNext;
				}
				else
				{

					numberOfSonsToVisitNext = std::min(
							maxNumberOfSonsToVisitNext, 1);
					firstSonToVisitNext += (theDimensions[dimension][index]
							< minPoint[dimension]);
				}

				for (int whichSon = 0; whichSon < numberOfSonsToVisitNext;
						++whichSon)
					indecesToVisit.push_back(firstSonToVisitNext + whichSon);
			}

			indecesToVisit.pop_front(numberOfIndecesToVisitThisDepth);
		}
		return foundPoints;
	}

	std::vector<unsigned int> search_in_the_box_BFS(
			const FKDPoint<TYPE, numberOfDimensions>& minPoint,
			const FKDPoint<TYPE, numberOfDimensions>& maxPoint)
	{

		//the queue could become a data member in case we don't want to run this in parallel
		FQueue<unsigned int> indecesToVisit(256);
		std::vector<unsigned int> foundPoints;
		foundPoints.reserve(16);
		indecesToVisit.push_back(0);
		unsigned int index;
		bool intersection;
		int dimension;
		int depth;
		int maxNumberOfSonsToVisitNext;
		int numberOfSonsToVisitNext;
		unsigned int firstSonToVisitNext;

		while (!indecesToVisit.empty())
		{

			index = indecesToVisit.pop_front();
			depth = FLOOR_LOG2(index + 1);

			dimension = depth % numberOfDimensions;

			intersection = intersects(index, minPoint, maxPoint, dimension);
			firstSonToVisitNext = leftSonIndex(index);
			maxNumberOfSonsToVisitNext = (firstSonToVisitNext
					< theNumberOfPoints)
					+ ((firstSonToVisitNext + 1) < theNumberOfPoints);

			if (intersection)
			{
				if (is_in_the_box(index, minPoint, maxPoint))
				{
					foundPoints.emplace_back(theIds[index]);
				}
				numberOfSonsToVisitNext = maxNumberOfSonsToVisitNext;
			}
			else
			{
				bool willVisitOnlyRightSon = theDimensions[dimension][index]
																		< minPoint[dimension];
				numberOfSonsToVisitNext = willVisitOnlyRightSon == maxNumberOfSonsToVisitNext? 0: std::min(maxNumberOfSonsToVisitNext,
						1);

				firstSonToVisitNext += willVisitOnlyRightSon;
			}

			for (int whichSon = 0; whichSon < numberOfSonsToVisitNext;
					++whichSon)
				indecesToVisit.push_back(firstSonToVisitNext + whichSon);

		}
		return foundPoints;
	}

	std::vector<unsigned int> search_in_the_box(
			const FKDPoint<TYPE, numberOfDimensions>& minPoint,
			const FKDPoint<TYPE, numberOfDimensions>& maxPoint) const
	{
		FQueue<unsigned int> indecesToVisit(256);
		std::vector<unsigned int> result;
		result.reserve(16);
		indecesToVisit.push_back(0);
		int dimension;
		unsigned int numberOfIndecesToVisitThisDepth;
		unsigned int index;
		bool intersection;
		bool isLowerThanBoxMin;
		int startSon;
		int endSon;
		unsigned int indexToAdd;
		for (unsigned int depth = 0; depth < theDepth + 1; ++depth)
		{

			dimension = depth % numberOfDimensions;
			numberOfIndecesToVisitThisDepth = indecesToVisit.size();
			for (unsigned int visitedIndecesThisDepth = 0;
					visitedIndecesThisDepth < numberOfIndecesToVisitThisDepth;
					visitedIndecesThisDepth++)
			{

				index = indecesToVisit[visitedIndecesThisDepth];
				intersection = intersects(index, minPoint, maxPoint, dimension);

				if (intersection && is_in_the_box(index, minPoint, maxPoint))
					result.push_back(theIds[index]);

				isLowerThanBoxMin = theDimensions[dimension][index]
						< minPoint[dimension];

				startSon = isLowerThanBoxMin; //left son = 0, right son =1

				endSon = isLowerThanBoxMin || intersection;

				for (int whichSon = startSon; whichSon < endSon + 1; ++whichSon)
				{
					indexToAdd = leftSonIndex(index) + whichSon;

					if (indexToAdd < theNumberOfPoints)
					{
						indecesToVisit.push_back(indexToAdd);
					}

				}

			}

			indecesToVisit.pop_front(numberOfIndecesToVisitThisDepth);
		}
		return result;
	}

	void search_in_the_box_recursive(
			const FKDPoint<TYPE, numberOfDimensions>& minPoint,
			const FKDPoint<TYPE, numberOfDimensions>& maxPoint,
			std::vector<unsigned int>& foundPoints, unsigned int index = 0,
			int dimension = 0) const
	{

		unsigned int firstSonToVisitNext = leftSonIndex(index);
		int maxNumberOfSonsToVisitNext = (firstSonToVisitNext
				< theNumberOfPoints)
				+ ((firstSonToVisitNext + 1) < theNumberOfPoints);
		bool intersection = intersects(index, minPoint, maxPoint, dimension);

		int numberOfSonsToVisitNext;
		if (intersection)
		{
			if (is_in_the_box(index, minPoint, maxPoint))
			{
				foundPoints.emplace_back(theIds[index]);
			}
			numberOfSonsToVisitNext = maxNumberOfSonsToVisitNext;
		}
		else
		{
			bool isLowerThanBoxMin = theDimensions[dimension][index]
					< minPoint[dimension];
			numberOfSonsToVisitNext =
					isLowerThanBoxMin && (maxNumberOfSonsToVisitNext == 1) ?
							0 : std::min(maxNumberOfSonsToVisitNext, 1);
			firstSonToVisitNext += isLowerThanBoxMin;

		}

		if (numberOfSonsToVisitNext != 0)

		{
			auto nextDimension = (dimension + 1) % numberOfDimensions;
			for (int whichSon = 0; whichSon < numberOfSonsToVisitNext;
					++whichSon)
				search_in_the_box_recursive(minPoint, maxPoint, foundPoints,
						firstSonToVisitNext + whichSon, nextDimension);
		}

	}

	bool test_correct_build(unsigned int index = 0, int dimension = 0) const
	{

		unsigned int leftSonIndexInArray = 2 * index + 1;
		unsigned int rightSonIndexInArray = leftSonIndexInArray + 1;
		if (rightSonIndexInArray >= theNumberOfPoints
				&& leftSonIndexInArray >= theNumberOfPoints)
		{
			return true;
		}
		else
		{
			if (leftSonIndexInArray < theNumberOfPoints)
			{
				if (theDimensions[dimension][index]
						>= theDimensions[dimension][leftSonIndexInArray])
				{
					test_correct_build(leftSonIndexInArray,
							(dimension + 1) % numberOfDimensions);

				}
				else
					return false;
			}

			if (rightSonIndexInArray < theNumberOfPoints)
			{
				if (theDimensions[dimension][index]
						<= theDimensions[dimension][rightSonIndexInArray])
				{
					test_correct_build(rightSonIndexInArray,
							(dimension + 1) % numberOfDimensions);

				}
				else
					return false;

			}

		}
		return true;

	}

	bool test_correct_search(const std::vector<unsigned int> foundPoints,
			const FKDPoint<TYPE, numberOfDimensions>& minPoint,
			const FKDPoint<TYPE, numberOfDimensions>& maxPoint) const
	{
		bool testGood = true;
		for (unsigned int i = 0; i < theNumberOfPoints; ++i)
		{

			bool shouldBeInTheBox = true;
			for (unsigned int dim = 0; dim < numberOfDimensions; ++dim)
			{
				shouldBeInTheBox &= (thePoints[i][dim] <= maxPoint[dim]
						&& thePoints[i][dim] >= minPoint[dim]);
			}

			bool foundToBeInTheBox = std::find(foundPoints.begin(),
					foundPoints.end(), thePoints[i].getId())
					!= foundPoints.end();

			if (foundToBeInTheBox == shouldBeInTheBox)
			{

				testGood &= true;
			}
			else
			{
				if (foundToBeInTheBox)
					std::cerr << "Point " << thePoints[i].getId()
							<< " was wrongly found to be in the box."
							<< std::endl;
				else
					std::cerr << "Point " << thePoints[i].getId()
							<< " was wrongly found to be outside the box."
							<< std::endl;

				testGood &= false;

			}
		}

		return testGood;
	}

	std::vector<TYPE> getDimensionVector(const int dimension) const
	{
		if (dimension < numberOfDimensions)
			return theDimensions[dimension];
	}

	std::vector<unsigned int> getIdVector() const
	{
		return theIds;
	}

	void build()
	{
		//gather kdtree building
		int dimension;
		theIntervalMin[0] = 0;
		theIntervalLength[0] = theNumberOfPoints;

		for (unsigned int depth = 0; depth < theDepth; ++depth)
		{

			dimension = depth % numberOfDimensions;
			unsigned int firstIndexInDepth = (1 << depth) - 1;
			for (unsigned int indexInDepth = 0; indexInDepth < (1 << depth);
					++indexInDepth)
			{
				unsigned int indexInArray = firstIndexInDepth + indexInDepth;
				unsigned int leftSonIndexInArray = 2 * indexInArray + 1;
				unsigned int rightSonIndexInArray = leftSonIndexInArray + 1;

				unsigned int whichElementInInterval = partition_complete_kdtree(
						theIntervalLength[indexInArray]);
				std::nth_element(
						thePoints.begin() + theIntervalMin[indexInArray],
						thePoints.begin() + theIntervalMin[indexInArray]
								+ whichElementInInterval,
						thePoints.begin() + theIntervalMin[indexInArray]
								+ theIntervalLength[indexInArray],
						[dimension](const FKDPoint<TYPE,numberOfDimensions> & a, const FKDPoint<TYPE,numberOfDimensions> & b) -> bool
						{
							if(a[dimension] == b[dimension])
							return a.getId() < b.getId();
							else
							return a[dimension] < b[dimension];
						});
				add_at_position(
						thePoints[theIntervalMin[indexInArray]
								+ whichElementInInterval], indexInArray);

				if (leftSonIndexInArray < theNumberOfPoints)
				{
					theIntervalMin[leftSonIndexInArray] =
							theIntervalMin[indexInArray];
					theIntervalLength[leftSonIndexInArray] =
							whichElementInInterval;
				}

				if (rightSonIndexInArray < theNumberOfPoints)
				{
					theIntervalMin[rightSonIndexInArray] =
							theIntervalMin[indexInArray]
									+ whichElementInInterval + 1;
					theIntervalLength[rightSonIndexInArray] =
							(theIntervalLength[indexInArray] - 1
									- whichElementInInterval);
				}
			}
		}

		dimension = theDepth % numberOfDimensions;
		unsigned int firstIndexInDepth = (1 << theDepth) - 1;
		for (unsigned int indexInArray = firstIndexInDepth;
				indexInArray < theNumberOfPoints; ++indexInArray)
		{
			add_at_position(thePoints[theIntervalMin[indexInArray]],
					indexInArray);

		}

	}

private:

	unsigned int partition_complete_kdtree(unsigned int length)
	{
		if (length == 1)
			return 0;
		unsigned int index = 1 << (FLOOR_LOG2(length));
//		unsigned int index = 1 << ((int) log2(length));

		if ((index / 2) - 1 <= length - index)
			return index - 1;
		else
			return length - index / 2;

	}

	unsigned int leftSonIndex(unsigned int index) const
	{
		return 2 * index + 1;
	}

	unsigned int rightSonIndex(unsigned int index) const
	{
		return 2 * index + 2;
	}

	bool intersects(unsigned int index,
			const FKDPoint<TYPE, numberOfDimensions>& minPoint,
			const FKDPoint<TYPE, numberOfDimensions>& maxPoint,
			int dimension) const
	{
		return (theDimensions[dimension][index] <= maxPoint[dimension]
				&& theDimensions[dimension][index] >= minPoint[dimension]);
	}

	bool is_in_the_box(unsigned int index,
			const FKDPoint<TYPE, numberOfDimensions>& minPoint,
			const FKDPoint<TYPE, numberOfDimensions>& maxPoint) const
	{
		for (unsigned int i = 0; i < numberOfDimensions; ++i)
		{
			if ((theDimensions[i][index] <= maxPoint[i]
					&& theDimensions[i][index] >= minPoint[i]) == false)
				return false;
		}

		return true;;
	}

	unsigned int theNumberOfPoints;
	unsigned int theDepth;
	std::vector<FKDPoint<TYPE, numberOfDimensions> > thePoints;
	std::array<std::vector<TYPE>, numberOfDimensions> theDimensions;
	std::vector<unsigned int> theIntervalLength;
	std::vector<unsigned int> theIntervalMin;
	std::vector<unsigned int> theIds;
};

#endif /* FKDTREE_FKDTREE_H_ */
